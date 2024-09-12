/* USER CODE BEGIN Header */
/************************************************************************************************************************
 * 	main: Sensors Hub STM32 ECU
 *  Authors: Ahmed Desoky
 *           Dina Hamed
 *	Date: 21/7/2024
 *	*********************************************************************************************************************
 *	Application and OS Layer for Sensors Hub ECU
 *  Applications:
 *              - Reset the microcontroller whenever a hang occurs
 *              - Control Vehicle Light System according to CAN messages received from the Main RaspberryPi ECU
 *              - Continuously get IMU and Magnetometer readings from GY521 and GY271 via I2C and send them to
 *                the Main RaspberryPi ECU for computations
 *              - Callback function to execute Shock sensor ISR code if shock detected send SHOCK CAN message to 
 *                the Main Raspberry Pi ECU
 *              - If any error occured to any connected device, its reported to the Main Raspberry ECU via CAN Bus
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
#include "gy271.h"
#include "gy521.h"
#include "Can_service_manager.h"
#include "light_system.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEVICE_ID					224
#define READING_BUFFER_SIZE			150
/*CAN messages IDs*/
#define GyroscopeReadingsID       0x003
#define AccelerometerReadingsID   0x004
#define MagnetometerReadingsID    0x005
#define TemperatureReadingsID     0x021
#define ShockSensorCrashID        0x020
#define Gyro_Accelero_ErrID		  0x012
#define	Magento_ErrID			  0x013
#define Turn_Right_MessageID	  0x022
#define Turn_Left_MessageID		  0x023
#define Front_Light_H_MessageID   0x024
#define Front_Light_L_MessageID   0x025
#define Back_Light_H_MessageID    0x026
#define Back_Light_L_MessageID    0x027
#define Awaiting_MessageID        0x028
#define	Inter_Light1_MessageID	  0x029
#define	Inter_Light2_MessageID	  0x030
#define	Inter_Light3_MessageID	  0x031
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

osThreadId defaultTaskHandle;
/* USER CODE BEGIN PV */
SemaphoreHandle_t xBuzzerSemaphore;
SemaphoreHandle_t xCANSemaphore;
/*Flags*/
uint8_t IMU_send_flag = 0;
uint8_t Shock_flag = 0;
uint8_t gy521_err_flag = 0;
uint8_t gy271_err_flag = 0;
/*Global Variables*/
//float magnetometer_val[3] = {0.0f, 0.0f, 0.0f};
float gyroscope_val[3] = {0.0f, 0.0f, 0.0f};
float accelerometer_val[3] = {0.0f, 0.0f, 0.0f};
float temperature_val = {0.0f};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_CAN_Init(void);
static void MX_I2C2_Init(void);
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
/************************************************************************************************************************
* Exception Name: HAL_GPIO_EXTI_Callback
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Description: Callback function of External Interrupt due to shock sensor rising edge
************************************************************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_1)
	{
		Shock_flag = 1;
	}
}
/************************************************************************************************************************
* Exception Name: HAL_CAN_RxFifo0MsgPendingCallback
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Description: Callback function to receive and assemble CAN messages
************************************************************************************************************************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RecievePacket_Callback();
	CAN_TP_Recieve_Callback();
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/************************************************************************************************************************
* Task Name: vIMUReadTask
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Description: Task to get IMU readings periodically and trigger CAN task to send them
************************************************************************************************************************/
void vIMUReadTask(void *pvParameters)
{
	TickType_t  xTaskLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		if(Shock_flag == 1)
		{
			xSemaphoreGive(xCANSemaphore);
		}
		// Read from GY-271 magnetometer
//		if(GY_271_Magnetometer_Read(magnetometer_val) != HAL_OK)
//		{
//			gy271_err_flag = 1;
//		}
		// Read from GY-521 gyroscope
		if(GY_521_Gyroscope_Read(gyroscope_val) != HAL_OK)
		{
			gy521_err_flag = 1;
		}
		// Read from GY-521 accelerometer
		if(GY_521_Accelerometer_Read(accelerometer_val) != HAL_OK)
		{
			gy521_err_flag = 1;
		}
		// Read from GY-521 temperature sensor
		if(GY_521_Temperature_Read(&temperature_val)!= HAL_OK)
		{
			gy521_err_flag = 1;
		}
		/*If there's no errors*/
		if((gy521_err_flag == 0) && (gy271_err_flag == 0))
		{
			IMU_send_flag = 1;
			GY_521_FUNCTION_INDICATE();
			GY_271_FUNCTION_INDICATE();
		}
		xSemaphoreGive(xCANSemaphore);
		// Delay to run the task periodically
		vTaskDelayUntil(&xTaskLastWakeTime,pdMS_TO_TICKS(30) );
	}
}
/************************************************************************************************************************
* Task Name: vLightSystemTask
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Description: Task to Update continuously Lighting System
************************************************************************************************************************/
void vLightSystemTask(void *pvParameters)
{
	uint8_t Received_CAN_Message[10] = {0};
	uint32_t Received_CAN_Message_length = 0;
	SecurityLevel Message_Security = NO_SECURITY;
	uint32_t Device_ID = 0;
	TickType_t  xTaskLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		if(Shock_flag == 1)
		{
			xSemaphoreGive(xCANSemaphore);
		}
		if(CAN_SM_Check_Inbox(Turn_Right_MessageID) == CAN_SM_OK)
		{
			if(CAN_SM_Read(Received_CAN_Message,Turn_Right_MessageID,\
				&Received_CAN_Message_length,&Message_Security,&Device_ID) == CAN_SM_OK)
			{
				Light_System_Toggle_State(Right_Turning);
			}
		}
		if(CAN_SM_Check_Inbox(Turn_Left_MessageID) == CAN_SM_OK)
		{
			if(CAN_SM_Read(Received_CAN_Message,Turn_Left_MessageID,\
				&Received_CAN_Message_length,&Message_Security,&Device_ID) == CAN_SM_OK)
			{
				Light_System_Toggle_State(Left_Turning);
			}
		}
		if(CAN_SM_Check_Inbox(Front_Light_H_MessageID) == CAN_SM_OK)
		{
			if(CAN_SM_Read(Received_CAN_Message,Front_Light_H_MessageID,\
				&Received_CAN_Message_length,&Message_Security,&Device_ID) == CAN_SM_OK)
			{
				Light_System_Toggle_State(Front_Light_H);
			}
		}
		if(CAN_SM_Check_Inbox(Front_Light_L_MessageID) == CAN_SM_OK)
		{
			if(CAN_SM_Read(Received_CAN_Message,Front_Light_L_MessageID,\
				&Received_CAN_Message_length,&Message_Security,&Device_ID) == CAN_SM_OK)
			{
				Light_System_Toggle_State(Front_Light_L);
			}
		}
		if(CAN_SM_Check_Inbox(Back_Light_H_MessageID) == CAN_SM_OK)
		{
			if(CAN_SM_Read(Received_CAN_Message,Back_Light_H_MessageID,\
				&Received_CAN_Message_length,&Message_Security,&Device_ID) == CAN_SM_OK)
			{
				Light_System_Toggle_State(Back_Light_H);
			}
		}
		if(CAN_SM_Check_Inbox(Back_Light_L_MessageID) == CAN_SM_OK)
		{
			if(CAN_SM_Read(Received_CAN_Message,Back_Light_L_MessageID,\
				&Received_CAN_Message_length,&Message_Security,&Device_ID) == CAN_SM_OK)
			{
				Light_System_Toggle_State(Back_Light_L);
			}
		}
		if(CAN_SM_Check_Inbox(Awaiting_MessageID) == CAN_SM_OK)
		{
			if(CAN_SM_Read(Received_CAN_Message,Awaiting_MessageID,\
				&Received_CAN_Message_length,&Message_Security,&Device_ID) == CAN_SM_OK)
			{
				Light_System_Toggle_State(Awaiting);
			}
		}
		if(CAN_SM_Check_Inbox(Inter_Light1_MessageID) == CAN_SM_OK)
		{
			if(CAN_SM_Read(Received_CAN_Message,Inter_Light1_MessageID,\
				&Received_CAN_Message_length,&Message_Security,&Device_ID) == CAN_SM_OK)
			{
				Light_System_Toggle_State(Interior_Light1);
			}
		}
		if(CAN_SM_Check_Inbox(Inter_Light2_MessageID) == CAN_SM_OK)
		{
			if(CAN_SM_Read(Received_CAN_Message,Inter_Light2_MessageID,\
				&Received_CAN_Message_length,&Message_Security,&Device_ID) == CAN_SM_OK)
			{
				Light_System_Toggle_State(Interior_Light2);
			}
		}
		if(CAN_SM_Check_Inbox(Inter_Light3_MessageID) == CAN_SM_OK)
		{
			if(CAN_SM_Read(Received_CAN_Message,Inter_Light3_MessageID,\
				&Received_CAN_Message_length,&Message_Security,&Device_ID) == CAN_SM_OK)
			{
				Light_System_Toggle_State(Interior_Light3);
			}
		}
		Light_System_Update();
		/*Delay every 500 ms*/
		vTaskDelayUntil(&xTaskLastWakeTime,pdMS_TO_TICKS(500));
	}
}
/************************************************************************************************************************
* Task Name: vCANTask
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Description: Task to send CAN messages of specific sensor or device to the receiving ECU
************************************************************************************************************************/
void vCANTask(void *pvParameters)
{
	while(1)
	{
		if(xSemaphoreTake(xCANSemaphore, portMAX_DELAY) == pdTRUE)
		{
			if(Shock_flag == 1)
			{
				Shock_flag = 0;
				CAN_SM_Send("SHOCK",ShockSensorCrashID,5,AES_CIPHER_HMAC,DEVICE_ID);
			}
			if(IMU_send_flag == 1)
			{
				/*CAN SM Send for all IMU data
				 * 15 seconds without security
				 * 35 seconds using Hash
				 * */
				IMU_send_flag = 0;
				uint8_t reading_str[READING_BUFFER_SIZE];
				// Convert float array to byte array
				sprintf(reading_str,"%f,%f,%f",gyroscope_val[0],gyroscope_val[1],gyroscope_val[2]);
				//Convert array of float of each reading to 3x4 bytes array of uint8_t
				CAN_SM_Send(reading_str,GyroscopeReadingsID,strlen(reading_str),HASH,DEVICE_ID);
//				memset(reading_str,0,READING_BUFFER_SIZE);
//				// Convert float array to byte array
//				sprintf(reading_str,"%f,%f,%f",magnetometer_val[0],magnetometer_val[1],magnetometer_val[2]);
//				//Convert array of float of each reading to 3x4 bytes array of uint8_t
//				CAN_SM_Send(reading_str,MagnetometerReadingsID,strlen(reading_str),HASH,DEVICE_ID);
				memset(reading_str,0,READING_BUFFER_SIZE);
				// Convert float array to byte array
				sprintf(reading_str,"%f,%f,%f",accelerometer_val[0],accelerometer_val[1],accelerometer_val[2]);
				//floatArrayToBytes(magnetometer_val, accelerometer_str, num_elements);
				CAN_SM_Send(reading_str,AccelerometerReadingsID,strlen(reading_str),HASH,DEVICE_ID);
				memset(reading_str,0,READING_BUFFER_SIZE);
				// Convert float array to byte array
				sprintf(reading_str,"%f",temperature_val);
				//Convert array of float of each reading to 3x4 bytes array of uint8_t
				CAN_SM_Send(reading_str,TemperatureReadingsID,strlen(reading_str),NO_SECURITY,DEVICE_ID);
				memset(reading_str,0,READING_BUFFER_SIZE);
			}
			if(gy521_err_flag == 1)
			{
				gy521_err_flag = 0;
				CAN_SM_Send((uint8_t*)("Err"),Gyro_Accelero_ErrID,3,HASH,DEVICE_ID);

			}
			if(gy271_err_flag == 1)
			{
				gy271_err_flag = 0;
				CAN_SM_Send((uint8_t*)("Err"),Magento_ErrID,3,HASH,DEVICE_ID);

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
  MX_I2C1_Init();
  MX_CAN_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
  //	if(GY_271_Init(&hi2c1) != HAL_OK)
  //	{
  //		Err_Handler();
  //	}
  if(GY_521_Init(&hi2c2)!= HAL_OK)
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
  xBuzzerSemaphore = xSemaphoreCreateBinary();
  if (xBuzzerSemaphore == NULL)
  {
	  Err_Handler();
  }

  xCANSemaphore = xSemaphoreCreateCounting(2,0);
  if (xCANSemaphore == NULL)
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

  // Create the sensor read task
  result = xTaskCreate(
		  vIMUReadTask,
		  "IMUReadTask",
		  128,
		  NULL,
		  tskIDLE_PRIORITY + 2,
		  NULL
  );

  if ((result != pdPASS)||(result == -1)) {
	  Err_Handler();
  }


  // Create the CAN task
  result = xTaskCreate(
		  vCANTask,
		  "CANTask",
		  512,
		  NULL,
		  tskIDLE_PRIORITY+4,
		  NULL
  );

  if ((result != pdPASS)||(result == -1)) {
	  Err_Handler();
  }

  // Create the Light System task
  result = xTaskCreate(
		  vLightSystemTask,
		  "LightSystemTask",
		  512,
		  NULL,
		  tskIDLE_PRIORITY+3,
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

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
  HAL_GPIO_WritePin(GPIOA, IMU_LED_Pin|Right_Light_Pin|Left_Light_Pin|Awaiting_Light_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, CAN_Fail_LED_Pin|Front_Light_H_Pin|Front_Light_L_Pin|Back_Light_H_Pin
                          |Back_Light_L_Pin|Inter_Light_3_Pin|Inter_Light_2_Pin|Inter_Light_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : System_Reset_LED_Pin CAN_Success_LED_Pin */
  GPIO_InitStruct.Pin = System_Reset_LED_Pin|CAN_Success_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : Shock_Sensor_Pin */
  GPIO_InitStruct.Pin = Shock_Sensor_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Shock_Sensor_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : IMU_LED_Pin Right_Light_Pin Left_Light_Pin Awaiting_Light_Pin */
  GPIO_InitStruct.Pin = IMU_LED_Pin|Right_Light_Pin|Left_Light_Pin|Awaiting_Light_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : CAN_Fail_LED_Pin Front_Light_H_Pin Front_Light_L_Pin Back_Light_H_Pin
                           Back_Light_L_Pin Inter_Light_3_Pin Inter_Light_2_Pin Inter_Light_1_Pin */
  GPIO_InitStruct.Pin = CAN_Fail_LED_Pin|Front_Light_H_Pin|Front_Light_L_Pin|Back_Light_H_Pin
                          |Back_Light_L_Pin|Inter_Light_3_Pin|Inter_Light_2_Pin|Inter_Light_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

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
