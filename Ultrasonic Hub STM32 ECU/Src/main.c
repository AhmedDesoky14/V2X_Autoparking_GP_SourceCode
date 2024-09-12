/* USER CODE BEGIN Header */
/************************************************************************************************************************
 * 	main: Ultrasonic Hub STM32 ECU
 *  Authors: Ahmed Desoky
 *           Dina Hamed
 *	Date: 21/7/2024
 ***********************************************************************************************************************/
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ultrasonic.h"
#include "can_service_manager.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/*Pins setting*/
#define BUZZER_PORT                 GPIOA
#define BUZZER_PIN                  GPIO_PIN_1
/*This Device ID*/
#define DEVICE_ID					103
/*CAN Messages IDs*/
#define HaltMessageID             0x006
#define BreakHaltMessageID        0x011
#define US_0_Err_MessageID		  0x014
#define US_1_Err_MessageID		  0x015
#define US_2_Err_MessageID 		  0x016
#define US_3_Err_MessageID 		  0x017
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim4;

osThreadId defaultTaskHandle;
/* USER CODE BEGIN PV */
SemaphoreHandle_t xBuzzerSemaphore;
SemaphoreHandle_t xCANSemaphore;

uint32_t current_message_id = 0x00;
uint8_t current_distanceFlag = 0;
uint8_t Ultrasonic_err_flag = 0;
float Distance_Values[ULTRASONIC_NUMBERS];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM4_Init(void);
void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */
/************************************************************************************************************************
* Function Name: Err_Handler
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Description: Function to Reset the Micro-controller in case of error occurred
************************************************************************************************************************/
void Err_Handler(void)
{
	HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
	HAL_Delay(1000);
	HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
	NVIC_SystemReset();
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/************************************************************************************************************************
* Task Name: vBuzzerTask
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Description: Task to operate the buzzer for blind spot detection using ultra sonic feature
************************************************************************************************************************/
void vBuzzerTask(void *pvParameters)
{
	while(1)
	{
		if(xSemaphoreTake(xBuzzerSemaphore, portMAX_DELAY) == pdTRUE)
		{
			if(current_distanceFlag == 1)
			{
				// Turn on the buzzer
				HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
			}
			else
			{
				// Turn off the buzzer
				HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
			}
		}
	}
}
/************************************************************************************************************************
* Task Name: vUltrasonicTask
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Description: Task to read ultrasonic sensors readings
************************************************************************************************************************/
void vUltrasonicTask(void *pvParameters)
{
	uint8_t previousDistanceFlag = 0; // To store the previous flag state
	while(1)
	{
		for(uint8_t k=0;k<ULTRASONIC_NUMBERS;k++)
		{
			 if(ULTRASONIC_readDistance(k,&Distance_Values[k]) != US_OK)
			 {
				 Ultrasonic_err_flag = 1;
				 switch(k)
				 {
				 	 case 0:
				 		current_message_id = US_0_Err_MessageID;
						break;
				 	 case 1:
					 	current_message_id = US_1_Err_MessageID;
						break;
				 	 case 2:
					 	current_message_id = US_2_Err_MessageID;
						break;
				 	 case 3:
					 	current_message_id = US_3_Err_MessageID;
						break;
				 }
				 xSemaphoreGive(xCANSemaphore);	/*Start CAN Send*/
			 }
		}
		// Initialize the flag based on current distances
		current_distanceFlag = 0;
		for (int i = 0; i < ULTRASONIC_NUMBERS ; i++)
		{
			if(Distance_Values[i] < 35.0)
			{
				current_distanceFlag = 1;
				break;
			}
		}
		// Check if there's a change in the distanceFlag
		if (current_distanceFlag != previousDistanceFlag)
		{
			// Update the previous flag
			previousDistanceFlag = current_distanceFlag;
			/*Signal CAN Task*/
			if(current_distanceFlag == 1)
			{
				current_message_id = HaltMessageID;
			}
			else if(current_distanceFlag == 0)
			{
				current_message_id = BreakHaltMessageID;
			}
			xSemaphoreGive(xCANSemaphore);
			// Signal the buzzer task only if there is a change in the flag
			xSemaphoreGive(xBuzzerSemaphore);
		}
		vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100ms
	}
}
/************************************************************************************************************************
* Task Name: vCANTask
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Description: Task to send CAN messages whenever needed
************************************************************************************************************************/
void vCANTask(void *pvParameters)
{
	while(1)
	{
		if(xSemaphoreTake(xCANSemaphore,portMAX_DELAY) == pdTRUE)
		{
			if(current_message_id == HaltMessageID)
			{
				current_message_id = 0x00;
				CAN_SM_Send("HALT",HaltMessageID,4,AES_CIPHER_HMAC,DEVICE_ID);
			}
			else if(current_message_id == BreakHaltMessageID)
			{
				current_message_id = 0x00;
				CAN_SM_Send("!HALT",BreakHaltMessageID,5,AES_CIPHER_HMAC,DEVICE_ID);
			}
			if(Ultrasonic_err_flag == 1)
			{
				Ultrasonic_err_flag = 0;
				switch(current_message_id)
				{
					case US_0_Err_MessageID:
						CAN_SM_Send((uint8_t*)("Err"),US_0_Err_MessageID,3,HASH,DEVICE_ID);
						break;
					case US_1_Err_MessageID:
						CAN_SM_Send((uint8_t*)("Err"),US_1_Err_MessageID,3,HASH,DEVICE_ID);
						break;
					case US_2_Err_MessageID:
						CAN_SM_Send((uint8_t*)("Err"),US_2_Err_MessageID,3,HASH,DEVICE_ID);
						break;
					case US_3_Err_MessageID:
						CAN_SM_Send((uint8_t*)("Err"),US_3_Err_MessageID,3,HASH,DEVICE_ID);
						break;
				}
			}
		}
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  if(ULTRASONIC_init() != US_OK)
  {
	  Err_Handler();
  }
  if(Activate_CAN_Bus(hcan,CAN_BUS_MODE_SENDING_ONLY) != CAN_BUS_ACTIVATED)
  {
	  Err_Handler();
  }
  if(CAN_TP_Init() != CAN_TRANSPORT_OK)
  {
	  Err_Handler();
  }
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  xCANSemaphore = xSemaphoreCreateBinary();
  if (xCANSemaphore == NULL)
  {
	  Err_Handler();
  }

  xBuzzerSemaphore = xSemaphoreCreateBinary();
  if (xBuzzerSemaphore == NULL)
  {
	  Err_Handler();
  }
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  BaseType_t result;

  result = xTaskCreate(
		  vUltrasonicTask,
		  "UltrasonicTask",
		  512,
		  NULL,
		  tskIDLE_PRIORITY + 2,
		  NULL
  );
  if ((result != pdPASS)||(result == -1))
  {
	  Err_Handler();
  }

  // Create the buzzer task
  result = xTaskCreate(
		  vBuzzerTask,
		  "BuzzerTask",
		  128,
		  NULL,
		  tskIDLE_PRIORITY+3,
		  NULL
  );

  if ((result != pdPASS)||(result == -1)) {
	  Err_Handler();
  }

  // Create the CAN task
  result = xTaskCreate(
		  vCANTask,
		  "CANTask",
		  1024,
		  NULL,
		  tskIDLE_PRIORITY+4,
		  NULL
  );

  if ((result != pdPASS)||(result == -1)) {
	  Err_Handler();
  }
  /* add threads, ... */
  // Start the scheduler
  vTaskStartScheduler();
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 8;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_5TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 63;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 63;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, System_Reset_LED_Pin|CAN_Success_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Trigger0_Pin|Buzzer_Pin|Trigger1_Pin|Trigger2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, CAN_Warning_LED_Pin|Trigger3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : System_Reset_LED_Pin CAN_Success_LED_Pin */
  GPIO_InitStruct.Pin = System_Reset_LED_Pin|CAN_Success_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Trigger0_Pin Buzzer_Pin Trigger1_Pin Trigger2_Pin */
  GPIO_InitStruct.Pin = Trigger0_Pin|Buzzer_Pin|Trigger1_Pin|Trigger2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : CAN_Warning_LED_Pin Trigger3_Pin */
  GPIO_InitStruct.Pin = CAN_Warning_LED_Pin|Trigger3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
