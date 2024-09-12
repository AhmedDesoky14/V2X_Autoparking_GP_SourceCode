/* USER CODE BEGIN Header */
/************************************************************************************************************************
 * 	main: Street Infrastructure STM32 ECU
 *  Authors: Ahmed Desoky
 *           Dina Hamed
 *	Date: 28/6/2024
 *	*********************************************************************************************************************
 *	Application and OS Layer for Sensors Hub ECU
 *  Applications:
 *              - Reset the microcontroller whenever a hang occurs
 *              - Continuously check and fetch latest updates of other infrastructures on the network from the database
 *				  via MQTT MathWorks Thingspeak Broker 
 *				- Always Ready to accept incoming vehicles connections and exchange neccessary information (Event Driven Application)
 *				- Continuously check if there's any emergency occured, and if occured connected buzzer become ON
 *				  and report to the database via MQTT MathWorks Thingspeak Broker (Event Driven Application)
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
#include "button.h"
#include"thingspeak_mqtt.h"
#include "v2i_server_service_manager.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define Network_Infrastructures	1/*Number of infrastructures connected excluding this infrastructure*/
#define Retry_Iterations		3	/*Iterations to retry if anything was not done OK*/
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart6_rx;

osThreadId defaultTaskHandle;
/* USER CODE BEGIN PV */
/*FreeRTOS Global Variables*/
SemaphoreHandle_t xMutex_ESP;	/*Mutex for ESP8266 Exclusive Resource sharing*/
SemaphoreHandle_t xSemaphore_Inf_Update;	/*Semaphore to trigger Inf. Update Task*/
SemaphoreHandle_t xSemaphore_V2I_Comm;	/*Semaphore to trigger V2ICommunicate Task*/
SemaphoreHandle_t xSemaphore_Emergency_Alarm;	/*Semaphore to trigger V2ICommunicate Task*/

/*Applications Global Variables*/
uint8_t ping_timer_counter = 0;	/*counter for ping timer*/
uint8_t connected_vehicle_location[2] = {0};	/*[0]:Latitude,[1]:Longitude*/
uint8_t V2I_Comm_flag = 0;
uint8_t Accident_flag = 0;
uint8_t Emergency_Vehicle_flag = 0;

/*Credentials of this infrastructure to be able to connect to the MQTT Broker*/
MQTT_CLIENT_CREDENTIALS Inf1_Client_Cred = {"HjEaMQwrKhsDLRsnJg8iORQ"		/*Client ID*/
		,"HjEaMQwrKhsDLRsnJg8iORQ"		/*Username*/
		,"1FQg1LEN47R5XaH2l7vq+4vH"};	/*Password*/

/* Subscribed Channels
 * Channel ID = 2435472
 * Field1 = Service Type = 1 (Garage) - Constant
 * Field2 = Garage/Infrastructure Name - Constant
 * Field3 = Infrastructure address - Constant
 * Field4 = Infrastructure Location (Latitude,Longitude) - Constant
 * Field5 = Available Slots - dynamic
 * Field6 = Cost Per Hour - dynamic
 */
Thingspeak_Channel Subscribed_Channels[Network_Infrastructures] =
{{2435472,"","",""
		,"","","","","","Z90II6H1KHYL7XOV"}};

/* My Channel
 * Channel ID = 2439697
 * Field1 = Service Type - Constant
 * Field2 = Garage/Infrastructure Name - Constant
 * Field3 = Infrastructure address - Constant
 * Field4 = Infrastructure Location (Latitude,Longitude) - Constant
 * Field5 = Collision Alarm - dynamic
 * Field6 = Emergency Vehicle Approaching Alarm - dynamic
 * Field7 = Traffic condition , Low-Medium-High
 */
Thingspeak_Channel My_Channel = {2439697,"2","Renaissance Statue","Oula Rd,El Omraniya,Giza,Egypt"
		,"30.027996,31.2153481","0","0","Low","","D0HTE4SJ6S9A6F2E"};

const uint16_t Speed_Limit = 60;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART6_UART_Init(void);
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
	Delay_ms(1000);
	HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
	NVIC_SystemReset();
}
/************************************************************************************************************************
* Function Name: Delay_ms
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Description: Delay Function
************************************************************************************************************************/
void Delay_ms(uint32_t delay_ms)
{
	uint32_t SystemClockFreq = HAL_RCC_GetHCLKFreq();
	uint32_t cycles_per_ms = SystemClockFreq / 16000;
	uint32_t total_cycles = cycles_per_ms * delay_ms;
	for (volatile uint32_t i = 0; i < total_cycles; ++i);
	return;
}
/************************************************************************************************************************
* Exception Name: HAL_UARTEx_RxEventCallback
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Description: Callback function of UART interrupt handler to receive any incoming data through esp8266 for any task
************************************************************************************************************************/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == USART6)
	{
		BaseType_t V2ICommunicateTaskWoken = pdFALSE;
		uint8_t Check_Device_Connected = 0;
		ESP8266_Receive_TCP_Packets_Callback(&Check_Device_Connected);
		if(Check_Device_Connected == 1)
		{
			/*Connected Device to the Access Point*/
			if(V2I_Comm_flag == 0)
			{
				xSemaphoreGiveFromISR(xSemaphore_V2I_Comm,&V2ICommunicateTaskWoken);
				portYIELD_FROM_ISR(V2ICommunicateTaskWoken);
				return;
			}
		}
		MQTT_Subscribe_Receive_Callback();
		Thingspeak_Receive_Callback();
		V2I_Receive_Message_Callback();
	}
}
/************************************************************************************************************************
* Exception Name: HAL_GPIO_EXTI_Callback
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Description: Callback function of the external interrupt of the
* 			   the push button to turn off the alarm whenever it's pushed
************************************************************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_5)
	{
		BaseType_t InfUpdateTaskWoken = pdFALSE;
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
		itoa(0,My_Channel.field5,10);
		Accident_flag = 0;
		Emergency_Vehicle_flag = 0;
		xSemaphoreGiveFromISR(xSemaphore_Inf_Update,&InfUpdateTaskWoken);
		portYIELD_FROM_ISR(InfUpdateTaskWoken);
	}
}
/************************************************************************************************************************
* Task Name: vAccidentTask
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Description: Task triggered whenever an accident message is sent to the infrastructure
* 			   from any vehicle nearby
************************************************************************************************************************/
void vEmergencyTask(void *pvParameters)
{
	while(1)
	{
		if(xSemaphoreTake(xSemaphore_Emergency_Alarm,portMAX_DELAY) == pdTRUE)
		{
			/*Set buzzer alarm and the led high*/
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
			itoa(1,My_Channel.field5,10);
			xSemaphoreGive(xSemaphore_Inf_Update);
		}
	}
}
/************************************************************************************************************************
* Task Name: vInfUpdateTask
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Description: Task to update the infrastructure status to the MQTT broker whenever there's an update
* 			   check "My_Channel" global object
************************************************************************************************************************/
void vInfUpdateTask(void *pvParameters)
{
	uint8_t Publish_Iterations_retry = 0;
	while(1)
	{
		/*Try to take the semaphore, if taken start the Task*/
		if(xSemaphoreTake(xSemaphore_Inf_Update,portMAX_DELAY) == pdTRUE)
		{
			// Wait for the mutex to ensure exclusive access
			if (xSemaphoreTake(xMutex_ESP,portMAX_DELAY) == pdTRUE) // BEFORE STOP LISTEN
			{
				while(Publish_Iterations_retry < Retry_Iterations)
				{
					// Stop listening to channels before updating if listening is on and subscribing to the broker
					if(Thingspeak_Check_Connection() == THINGSPEAK_OK)
					{
						if(Thingspeak_Stop_Listen_Channels() != THINGSPEAK_OK)
						{
							Publish_Iterations_retry++;
							continue;	/*If failed release the mutex and continue*/
						}
					}
					/*Update this infrastructure info at the cloud (Broker)*/
					if(Thingspeak_Update_Channel(&My_Channel,&Inf1_Client_Cred) != THINGSPEAK_OK)
					{
						Publish_Iterations_retry++;
						continue;	/*If failed release the mutex and continue*/
					}
					/*Updated, Start listening back to other infrastructures*/
					Thingspeak_Start_Listen_Channels(Subscribed_Channels,Network_Infrastructures,&Inf1_Client_Cred);
					break;
				}
				Publish_Iterations_retry = 0;
				// Release the Mutex
				xSemaphoreGive(xMutex_ESP);
			}
		}
	}
}
/************************************************************************************************************************
* Task Name: vGetNetworkDataTask
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Description: Task to periodically check whether there's a new update on the infrastructures network
* 			   and get that update if available
* 			   check "Subscribed_Channels" global variable
************************************************************************************************************************/
void vGetNetworkDataTask(void *pvParameters)
{
	while(1)
	{
		// Attempt to take the Mutex, wait indefinitely if not available
		if(xSemaphoreTake(xMutex_ESP,portMAX_DELAY) == pdTRUE)
		{
			/*If not subscribed to the MQTT Broker*/
			if (Thingspeak_Check_Connection() == THINGSPEAK_NOT_OK)
			{
				uint8_t start_listen_retry = 0;
				while(start_listen_retry < Retry_Iterations)
				{
					/*Retry to connect if not done successfully*/
					if(Thingspeak_Start_Listen_Channels(Subscribed_Channels,Network_Infrastructures,&Inf1_Client_Cred) == THINGSPEAK_OK)
					{
						break;
					}
					start_listen_retry++;
				}
			}
			/*If I'm connected and listening*/
			if(Thingspeak_Check_Connection() == THINGSPEAK_OK)
			{
				//Check inbox for messages for the specific channel
				if (Thingspeak_Check_Inbox(Subscribed_Channels[0].Channel_ID) == THINGSPEAK_OK)
				{
					// Read the message
					Thingspeak_Read_Message(&Subscribed_Channels[0]);
				}
			}
		}
		//After finishing release the Mutex
		xSemaphoreGive(xMutex_ESP);
		// Delay for a period of time before checking for updates again
		vTaskDelay(pdMS_TO_TICKS(7000)); // Delay for 7 second
	}
}
/************************************************************************************************************************
* Task Name: vV2ICommunicateTask
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Description: Task is triggered whenever a new vehicle connects to the infrastructure
*			   and hand its requests, send and receive information if needed
************************************************************************************************************************/
void vV2ICommunicateTask(void *pvParameters)
{
	uint8_t message_recieved[V2I_MAX_MESSAGE] = {0};
	uint32_t message_recieved_size = 0;
	uint8_t inbox_flag = 0;
	while(1)
	{
		uint32_t Current_Vehicle_ID = 0;
		V2I_Comm_flag = 0;
		if(xSemaphoreTake(xSemaphore_V2I_Comm,portMAX_DELAY) == pdTRUE)
		{
			V2I_Comm_flag = 1;
			if(xSemaphoreTake(xMutex_ESP,portMAX_DELAY) == pdTRUE)
			{
				Thingspeak_Stop_Listen_Channels();
				Set_ESP_Operation_Mode(V2I_Communications);
				V2I_Start_Reception();
				uint8_t start_client_connection_retry = 0;
				while(start_client_connection_retry < Retry_Iterations)
				{
					if(V2I_Check_Clients(&Current_Vehicle_ID) == V2I_OK)
					{
						break;
					}
					Delay_ms(100);	//////
					start_client_connection_retry++;
				}
				if(Current_Vehicle_ID == 0)
				{
					V2I_Stop_Reception();
					Set_ESP_Operation_Mode(Network_Database_Communications);
					/*Release Mutex after completion*/
					xSemaphoreGive(xMutex_ESP);
					continue;
				}
				/*Send to vehicle to inform it to send to serve it*/
				uint8_t Ready_Message[25] = {0};
				sprintf(Ready_Message,"%s%d",READY_CONNECTION,My_Channel.Channel_ID);	/*Channel ID = Infrastructure ID*/
				V2I_Send_Message(Ready_Message,strlen(Ready_Message),Current_Vehicle_ID,NO_SECURITY);
				uint8_t check_inbox_retry = 0;
				while(check_inbox_retry < Retry_Iterations)
				{
					if(V2I_Check_Inbox(Current_Vehicle_ID) == V2I_OK)
					{
						inbox_flag = 1;
						break;
					}
					check_inbox_retry++;
					Delay_ms(100);	//////
				}
				if(inbox_flag == 1)
				{
					V2I_Get_Received_Message(message_recieved,&message_recieved_size,Current_Vehicle_ID);
					/*After each read message, send acknowledge to confirm*/
					if(strstr(message_recieved,REQUEST_MESSAGE) != NULL)
					{
						V2I_Send_Message(ACK_REQUEST,strlen(ACK_REQUEST),Current_Vehicle_ID,NO_SECURITY);
					}
					else if(strstr(message_recieved,SEND_MESSAGE) != NULL)
					{
						V2I_Send_Message(ACK_SEND,strlen(ACK_SEND),Current_Vehicle_ID,NO_SECURITY);
					}
					while(1)
					{
						/*Request message format -> "REQUEST:x/" */
						if(strstr(message_recieved,REQUEST_MESSAGE) != NULL)
						{
							uint32_t Request_Number = 0;
							uint8_t Request_Number_char[5] = {0};
							uint8_t Get_Request_Counter = strlen(REQUEST_MESSAGE);
							uint8_t i = 0;
							while(message_recieved[Get_Request_Counter] != '/')
							{
								Request_Number_char[i] = message_recieved[Get_Request_Counter];
								i++;
								Get_Request_Counter++;
							}
							Request_Number = atoi(Request_Number_char);
							switch(Request_Number)
							{
/*============================================================================================================================================*/
								case REQUEST_WEATHER:
									cJSON* Weather = cJSON_CreateArray();
									cJSON_AddNumberToObject(Weather,"Temperature",35);
									cJSON_AddNumberToObject(Weather,"Humidity %",95);
									cJSON_AddNumberToObject(Weather,"UV Index",8);
									uint8_t* Weather_Str = cJSON_Print(Weather);
									V2I_Send_Message(Weather_Str,strlen(Weather_Str),Current_Vehicle_ID,NO_SECURITY);
									break;
								case REQUEST_GARAGE:
									cJSON* Garage_Message = cJSON_CreateObject();
									for(uint16_t i=0;i<Network_Infrastructures;i++)
									{

									}
									break;
								case REQUEST_SPEED_LIMIT:
									uint8_t Speed_Limit_Str[10] = {0};
									sprintf(Speed_Limit_Str,"%d Km/h",Speed_Limit);
									V2I_Send_Message(Speed_Limit_Str,strlen(Speed_Limit_Str),Current_Vehicle_ID,NO_SECURITY);
									break;
								case REQUEST_TRAFFIC_CONDITION:
									V2I_Send_Message(My_Channel.field7,strlen(My_Channel.field7),Current_Vehicle_ID,NO_SECURITY);
									break;
								case REQUEST_EMERGENCY_VEHICLE:
									if(Emergency_Vehicle_flag == 1)
									{
										V2I_Send_Message("1",1,Current_Vehicle_ID,NO_SECURITY);
									}
									else
									{
										V2I_Send_Message("0",1,Current_Vehicle_ID,NO_SECURITY);
									}
									break;
								case REQUEST_ACCIDENT_ISSUE:
									if(Accident_flag == 1)
									{
										V2I_Send_Message("1",1,Current_Vehicle_ID,NO_SECURITY);
									}
									else
									{
										V2I_Send_Message("0",1,Current_Vehicle_ID,NO_SECURITY);
									}
									break;

							/*Section End*/
/*============================================================================================================================================*/
							}
						}
						/*Send message format -> "SEND:x/" */
						else if(strstr(message_recieved,SEND_MESSAGE) != NULL)
						{
							uint32_t Send_Number = 0;
							uint8_t Send_Number_char[5] = {0};
							uint8_t Get_Send_Counter = strlen(SEND_MESSAGE);
							uint8_t i = 0;
							while(message_recieved[Get_Send_Counter] != '/')
							{
								Send_Number_char[i] = message_recieved[Get_Send_Counter];
								i++;
								Get_Send_Counter++;
							}
							Send_Number = atoi(Send_Number_char);
							switch(Send_Number)
							{
/*============================================================================================================================================*/
							/*Send Messages section, to add as message as you want*/
								case SEND_ACCIDENT_ISSUE:
									Accident_flag = 1;
									xSemaphoreGive(xSemaphore_Emergency_Alarm);
									break;
								case SEND_EMERGENCY_VEHICLE:
									Emergency_Vehicle_flag = 1;
									xSemaphoreGive(xSemaphore_Emergency_Alarm);
									break;
								case SEND_CURRENT_LOCATION:
									uint8_t Latitude[25] = {0};
									uint8_t Longitude[25] = {0};
									uint8_t Rx_Vehicle_Current_Location_Str[50] = {0};
									uint32_t Rx_Size = 0;
									V2I_Get_Received_Message(Rx_Vehicle_Current_Location_Str,&Rx_Size,Current_Vehicle_ID);
									uint16_t i = 0;
									while(Rx_Vehicle_Current_Location_Str[i] != ',')
									{
										Latitude[i] = Rx_Vehicle_Current_Location_Str[i];
										i++;
									}
									uint16_t j = 0;
									while(Rx_Vehicle_Current_Location_Str[i] != '\0')
									{
										Longitude[j] = Rx_Vehicle_Current_Location_Str[i];
										i++; j++;
									}
									/*Store current vehicle location*/
									connected_vehicle_location[0] = atof(Latitude);
									connected_vehicle_location[1] = atof(Longitude);
									break;
							/*Section End*/
/*============================================================================================================================================*/
							}
						}
						else
						{
							memset(message_recieved,0,V2I_MAX_MESSAGE);
							message_recieved_size = 0;
							break;
						}
						/*for more option format -> "REQUEST:x/MORE" or "SEND:x/MORE" */
						if(strstr(message_recieved,MORE_SERVICES) == NULL)
						{
							break;
						}
						memset(message_recieved,0,V2I_MAX_MESSAGE);
						message_recieved_size = 0;
						check_inbox_retry = 0;
						inbox_flag = 0;
						V2I_Send_Message(CONTINUE_MESSAGE,strlen(CONTINUE_MESSAGE),Current_Vehicle_ID,NO_SECURITY);
						while(check_inbox_retry < Retry_Iterations)
						{
							if(V2I_Check_Inbox(Current_Vehicle_ID) == V2I_OK)
							{
								inbox_flag = 1;
								break;
							}
							check_inbox_retry++;
							Delay_ms(100);	//////
						}
						if(inbox_flag == 1)
						{
							V2I_Get_Received_Message(message_recieved,&message_recieved_size,Current_Vehicle_ID);
							/*After each read message, send acknowledge to confirm*/
							if(strstr(message_recieved,REQUEST_MESSAGE) != NULL)
							{
								V2I_Send_Message(ACK_REQUEST,strlen(ACK_REQUEST),Current_Vehicle_ID,NO_SECURITY);
							}
							else if(strstr(message_recieved,SEND_MESSAGE) != NULL)
							{
								V2I_Send_Message(ACK_SEND,strlen(ACK_SEND),Current_Vehicle_ID,NO_SECURITY);
							}
						}
						else if(inbox_flag == 0)	/*No thing received or maybe connection lost*/
						{
							break;
						}
					}
					/*Reset Buffers*/
					memset(message_recieved,0,V2I_MAX_MESSAGE);
					message_recieved_size = 0;
					inbox_flag = 0;
					check_inbox_retry = 0;
				}
				V2I_Stop_Reception();
				Set_ESP_Operation_Mode(Network_Database_Communications);
				/*Release Mutex after completion*/
				xSemaphoreGive(xMutex_ESP);
			}
		}
	}
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_DMA_Init();
  MX_USART6_UART_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

  	if(Crypto_Init() != CSM_OK)
  	{
  		Err_Handler();
  	}

	if(ESP8266_Init(&huart6,&ESP8266_Configuration) != RESPONSE_OK)
	{
		Err_Handler();
	}

	if(ESP8266_Start_Receive_TCP_Packets() != RESPONSE_OK)
	{
		Err_Handler();
	}

	if(Set_ESP_Operation_Mode(Network_Database_Communications) != RESPONSE_OK)
	{
		Err_Handler();
	}

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
	xMutex_ESP = xSemaphoreCreateMutex();
	if (xMutex_ESP == NULL)
	{
		// Handle Mutex creation failure
		Err_Handler();
	}
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	xSemaphore_Inf_Update = xSemaphoreCreateBinary();
	if(xSemaphore_Inf_Update == NULL)
	{
		// Handle Semaphore creation failure
		Err_Handler();
	}
	xSemaphore_V2I_Comm = xSemaphoreCreateBinary();
	if(xSemaphore_Inf_Update == NULL)
	{
		// Handle Semaphore creation failure
		Err_Handler();
	}

	xSemaphore_Emergency_Alarm = xSemaphoreCreateBinary();
	if(xSemaphore_Emergency_Alarm == NULL)
	{
		// Handle Semaphore creation failure
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
  BaseType_t xReturn;

	if (xReturn == pdFALSE)
	{
		Err_Handler();
	}

  xReturn = xTaskCreate(
			vInfUpdateTask,     // Task function
			"InfUpdateTask",    // Name of the task
			4096,                 // Stack size in words
			NULL,                // Task parameter
			tskIDLE_PRIORITY+2 ,    // Task priority
			NULL                 // Task handle
	);

	if (xReturn == pdFALSE)
	{
		Err_Handler();
	}

 xReturn = xTaskCreate(
			vGetNetworkDataTask,           // Task function
			"GetNetworkDataTask",          // Task name
			4096,    					   // Stack size
			NULL,                        // Task parameter
			tskIDLE_PRIORITY+1,        // Task priority
			NULL
	);

	if (xReturn == pdFALSE)
	{
		Err_Handler();
	}

	xReturn = xTaskCreate(
			vV2ICommunicateTask,          // Function that implements the task.
			"V2ICommunicateTask",        // Text name for the task.
			8192,             // Stack size in words, not bytes.
			NULL,             // Parameter passed into the task.
			tskIDLE_PRIORITY+4, // Priority at which the task is created.
			NULL              // Used to pass out the created task's handle.
	);
	if (xReturn == pdFALSE)
	{
		Err_Handler();
	}

	  xReturn = xTaskCreate(
			  vEmergencyTask,      // Task function.
				"EmergencyTask",     // Name of the task.
				1024,                // Stack size in words.
				NULL,               // Task parameter
				tskIDLE_PRIORITY+5,   // Task priority
				NULL                // Task handle.
		);

		if (xReturn == pdFALSE)
		{
			Err_Handler();
		}

		/*Give semaphore to make infrastructure update once at start*/
		xSemaphoreGive(xSemaphore_Inf_Update);
	/* add threads, ... */

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 49999;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 59999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, System_Reset_LED_Pin|ESP8266_Init_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Alarm_Buzzer_Pin|Alarm_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|ESP8266_AP_LED_Pin|GPIO_PIN_5|ESP8266_Wi_Fi_Connected_LED_Pin
                          |ESP8266_Internet_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : System_Reset_LED_Pin ESP8266_Init_LED_Pin */
  GPIO_InitStruct.Pin = System_Reset_LED_Pin|ESP8266_Init_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Alarm_Buzzer_Pin Alarm_LED_Pin */
  GPIO_InitStruct.Pin = Alarm_Buzzer_Pin|Alarm_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : Alarm_Reset_Push_Button_Pin */
  GPIO_InitStruct.Pin = Alarm_Reset_Push_Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(Alarm_Reset_Push_Button_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 ESP8266_AP_LED_Pin PB5 ESP8266_Wi_Fi_Connected_LED_Pin
                           ESP8266_Internet_LED_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_12|ESP8266_AP_LED_Pin|GPIO_PIN_5|ESP8266_Wi_Fi_Connected_LED_Pin
                          |ESP8266_Internet_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

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
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
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
