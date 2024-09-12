/* USER CODE BEGIN Header */
/************************************************************************************************************************
 * 	main: Garage Infrastructure STM32 ECU
 *  Authors: Ahmed Desoky
 *           Dina Hamed
 *	Date: 30/6/2024
 *	*********************************************************************************************************************
 *	Application and OS Layer for Sensors Hub ECU
 *  Applications:
 *              - Reset the microcontroller whenever a hang occurs
 *              - Continuously check and fetch latest updates of other infrastructures on the network from the database
 *				  via MQTT MathWorks Thingspeak Broker 
 *              - Continuously check garage lots updates and report to the database via MQTT
 *				  MathWorks Thingspeak Broker (Event Driven Application)
 *				- Always Ready to accept incoming vehicles connections and exchange neccessary information (Event Driven Application)
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
#include "Ir.h"
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
#define MAP_SIZE_PAIRS			272
#define Parking_Fees_Base		10
#define Total_Parking_Slots		4
#define K_Constant				1.5
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
/*Applications Global Variables*/
uint8_t ping_timer_counter = 0;	/*counter for ping timer*/
slot_state slots_state[IR_NUMBERS] = {0};
uint32_t previous_available_slot_count = 0;
uint32_t available_slot_count = 0;
uint32_t current_free_slot = 0;
uint32_t current_free_Slot_location[2] = {0};
uint32_t recently_free_slot = 0;
uint32_t recently_free_slot_location[2] = {0};
uint8_t connected_vehicle_id = 0;
uint8_t V2I_Comm_flag = 0;

uint32_t current_fees_rate = 0;



int32_t garage_map[MAP_SIZE_PAIRS][2] = {
	    {272, 188}, {272, 191}, {272, 194}, {272, 197}, {272, 200}, {272, 203}, {272, 206},
	    {272, 209}, {272, 212}, {272, 215}, {272, 218}, {272, 221}, {272, 224}, {272, 227},
	    {272, 230}, {272, 233}, {272, 236}, {273, 188}, {273, 191}, {273, 194}, {273, 197},
	    {273, 200}, {273, 203}, {273, 206}, {273, 209}, {273, 212}, {273, 215}, {273, 218},
	    {273, 221}, {273, 224}, {273, 227}, {273, 230}, {273, 233}, {273, 236}, {274, 188},
	    {274, 191}, {274, 194}, {274, 197}, {274, 200}, {274, 203}, {274, 206}, {274, 209},
	    {274, 212}, {274, 215}, {274, 218}, {274, 221}, {274, 224}, {274, 227}, {274, 230},
	    {274, 233}, {274, 236}, {275, 188}, {275, 191}, {275, 194}, {275, 197}, {275, 200},
	    {275, 203}, {275, 206}, {275, 209}, {275, 212}, {275, 215}, {275, 218}, {275, 221},
	    {275, 224}, {275, 227}, {275, 230}, {275, 233}, {275, 236}, {276, 188}, {276, 191},
	    {276, 194}, {276, 197}, {276, 200}, {276, 203}, {276, 206}, {276, 209}, {276, 212},
	    {276, 215}, {276, 218}, {276, 221}, {276, 224}, {276, 227}, {276, 230}, {276, 233},
	    {276, 236}, {277, 188}, {277, 191}, {277, 194}, {277, 197}, {277, 200}, {277, 203},
	    {277, 206}, {277, 209}, {277, 212}, {277, 215}, {277, 218}, {277, 221}, {277, 224},
	    {277, 227}, {277, 230}, {277, 233}, {277, 236}, {279, 188}, {279, 191}, {279, 194},
	    {279, 197}, {279, 200}, {279, 203}, {279, 206}, {279, 209}, {279, 212}, {279, 215},
	    {279, 218}, {279, 221}, {279, 224}, {279, 227}, {279, 230}, {279, 233}, {279, 236},
	    {280, 188}, {280, 191}, {280, 194}, {280, 197}, {280, 200}, {280, 203}, {280, 206},
	    {280, 209}, {280, 212}, {280, 215}, {280, 218}, {280, 221}, {280, 224}, {280, 227},
	    {280, 230}, {280, 233}, {280, 236}, {281, 188}, {281, 191}, {281, 194}, {281, 197},
	    {281, 200}, {281, 203}, {281, 206}, {281, 209}, {281, 212}, {281, 215}, {281, 218},
	    {281, 221}, {281, 224}, {281, 227}, {281, 230}, {281, 233}, {281, 236}, {282, 188},
	    {282, 191}, {282, 194}, {282, 197}, {282, 200}, {282, 203}, {282, 206}, {282, 209},
	    {282, 212}, {282, 215}, {282, 218}, {282, 221}, {282, 224}, {282, 227}, {282, 230},
	    {282, 233}, {282, 236}, {283, 188}, {283, 191}, {283, 194}, {283, 197}, {283, 200},
	    {283, 203}, {283, 206}, {283, 209}, {283, 212}, {283, 215}, {283, 218}, {283, 221},
	    {283, 224}, {283, 227}, {283, 230}, {283, 233}, {283, 236}, {284, 188}, {284, 191},
	    {284, 194}, {284, 197}, {284, 200}, {284, 203}, {284, 206}, {284, 209}, {284, 212},
	    {284, 215}, {284, 218}, {284, 221}, {284, 224}, {284, 227}, {284, 230}, {284, 233},
	    {284, 236}, {290, 188}, {290, 191}, {290, 194}, {290, 197}, {290, 200}, {290, 203},
	    {290, 206}, {290, 209}, {290, 212}, {290, 215}, {290, 218}, {290, 221}, {290, 224},
	    {290, 227}, {290, 230}, {290, 233}, {290, 236}, {291, 188}, {291, 191}, {291, 194},
	    {291, 197}, {291, 200}, {291, 203}, {291, 206}, {291, 209}, {291, 212}, {291, 215},
	    {291, 218}, {291, 221}, {291, 224}, {291, 227}, {291, 230}, {291, 233}, {291, 236},
	    {292, 188}, {292, 191}, {292, 194}, {292, 197}, {292, 200}, {292, 203}, {292, 206},
	    {292, 209}, {292, 212}, {292, 215}, {292, 218}, {292, 221}, {292, 224}, {292, 227},
	    {292, 230}, {292, 233}, {292, 236}, {293, 188}, {293, 191}, {293, 194}, {293, 197},
	    {293, 200}, {293, 203}, {293, 206}, {293, 209}, {293, 212}, {293, 215}, {293, 218},
	    {293, 221}, {293, 224}, {293, 227}, {293, 230}, {293, 233}, {293, 236}, {294, 188},
	    {294, 191}, {294, 194}, {294, 197}, {294, 200}, {294, 203}, {294, 206}, {294, 209},
	    {294, 212}, {294, 215}, {294, 218}, {294, 221}, {294, 224}, {294, 227}, {294, 230},
	    {294, 233}, {294, 236}, {295, 188}, {295, 191}, {295, 194}, {295, 197}, {295, 200},
	    {295, 203}, {295, 206}, {295, 209}, {295, 212}, {295, 215}, {295, 218}, {295, 221},
	    {295, 224}, {295, 227}, {295, 230}, {295, 233}, {295, 236}, {297, 188}, {297, 191},
	    {297, 194}, {297, 197}, {297, 200}, {297, 203}, {297, 206}, {297, 209}, {297, 212},
	    {297, 215}, {297, 218}, {297, 221}, {297, 224}, {297, 227}, {297, 230}, {297, 233},
	    {297, 236}, {298, 188}, {298, 191}, {298, 194}, {298, 197}, {298, 200}, {298, 203},
	    {298, 206}, {298, 209}, {298, 212}, {298, 215}, {298, 218}, {298, 221}, {298, 224},
	    {298, 227}, {298, 230}, {298, 233}, {298, 236}, {299, 188}, {299, 191}, {299, 194},
	    {299, 197}, {299, 200}, {299, 203}, {299, 206}, {299, 209}, {299, 212}, {299, 215},
	    {299, 218}, {299, 221}, {299, 224}, {299, 227}, {299, 230}, {299, 233}, {299, 236},
	    {300, 188}, {300, 191}, {300, 194}, {300, 197}, {300, 200}, {300, 203}, {300, 206},
	    {300, 209}, {300, 212}, {300, 215}, {300, 218}, {300, 221}, {300, 224}, {300, 227},
	    {300, 230}, {300, 233}, {300, 236}, {301, 188}, {301, 191}, {301, 194}, {301, 197},
	    {301, 200}, {301, 203}, {301, 206}, {301, 209}, {301, 212}, {301, 215}, {301, 218},
	    {301, 221}, {301, 224}, {301, 227}, {301, 230}, {301, 233}, {301, 236}, {302, 188},
	    {302, 191}, {302, 194}, {302, 197}, {302, 200}, {302, 203}, {302, 206}, {302, 209},
	    {302, 212}, {302, 215}, {302, 218}, {302, 221}, {302, 224}, {302, 227}, {302, 230},
	    {302, 233}, {302, 236}
	};

/*Credentials of this infrastructure to be able to connect to the MQTT Broker*/
MQTT_CLIENT_CREDENTIALS Inf0_Client_Cred = {"IQUqIjIPIQstCw4hACAXOz0"		/*Client ID*/
		,"IQUqIjIPIQstCw4hACAXOz0"		/*Username*/
		,"YXuek6xYQ41gPw6wLw44tMSR"};	/*Password*/
/* Subscribed Channels
 * Channel ID = 2439697
 * Field1 = Service Type - Constant
 * Field2 = Garage/Infrastructure Name - Constant
 * Field3 = Infrastructure address - Constant
 * Field4 = Infrastructure Location (Latitude,Longitude) - Constant
 * Field5 = Collision Alarm - dynamic
 */

Thingspeak_Channel Subscribed_Channels[Network_Infrastructures] =
{{2439697,"","",""
		,"","","","","","D0HTE4SJ6S9A6F2E"}};

/* My Channel
 * Channel ID = 2435472
 * Field1 = Service Type = 1 (Garage) - Constant
 * Field2 = Garage/Infrastructure Name - Constant
 * Field3 = Infrastructure address - Constant
 * Field4 = Infrastructure Location (Latitude,Longitude) - Constant
 * Field5 = Available Slots - dynamic
 * Field6 = Cost Per Hour - dynamic
 */
Thingspeak_Channel My_Channel = {2435472,"1","CUFE Garage","Cairo University,Oula Rd,Giza,Egypt"
		,"30.0261014,31.2110266","0","0","","","Z90II6H1KHYL7XOV"};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART6_UART_Init(void);
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
* Task Name: vIRSensorsTask
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Description: Task to read IR sensors periodically
* 			   represented as parking slots to check whether the slot is full or empty
************************************************************************************************************************/
void vIRSensorsTask(void *pvParameters)
{
	uint32_t free_slots = 0;
	while(1)
	{
		free_slots = 0;
		uint8_t current_free_flag = 0;
		for (int i = 0; i < IR_NUMBERS; i++)
		{
			slots_state[i] = IR_ReadData(i);
			isOccupied(i);
			if (slots_state[i] == AVAILABLE_SLOT )
			{
				if(current_free_flag == 0)
				{
					current_free_slot = i;
					current_free_flag = 1;
				}
				free_slots++;
			}
		}
		/*Update slots*/
		previous_available_slot_count = available_slot_count;
		available_slot_count = free_slots;
		current_fees_rate = Parking_Fees_Base*(1+K_Constant*((Total_Parking_Slots-available_slot_count)/(Total_Parking_Slots)));
		/*If there's a change in their number*/
		if(available_slot_count != previous_available_slot_count)
		{
			/*Trigger the update task to update this infrastructure data*/
			itoa(available_slot_count,My_Channel.field5,10);
			itoa(current_fees_rate,My_Channel.field6,10);
			xSemaphoreGive(xSemaphore_Inf_Update);
		}
		/*Delay for 3 seconds*/
		vTaskDelay(pdMS_TO_TICKS(3000));
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
					if(Thingspeak_Update_Channel(&My_Channel,&Inf0_Client_Cred) != THINGSPEAK_OK)
					{
						Publish_Iterations_retry++;
						continue;	/*If failed release the mutex and continue*/
					}
					/*Updated, Start listening back to other infrastructures*/
					Thingspeak_Start_Listen_Channels(Subscribed_Channels,Network_Infrastructures,&Inf0_Client_Cred);
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
					if(Thingspeak_Start_Listen_Channels(Subscribed_Channels,Network_Infrastructures,&Inf0_Client_Cred) == THINGSPEAK_OK)
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
							uint16_t Request_Number = 0;
							uint8_t Request_Number_char[5] = {0};
							uint8_t Get_Request_Counter = strlen(REQUEST_MESSAGE);
							uint8_t i = 0;
							/*Parsing Request Number*/
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
							/*Request Messages section, to add as message as you want*/
								case REQUEST_PARKING:
									/*Put Map, Slot Number, Slot Location, Parking Cost, Intermediate points if needed*/
									cJSON* Send_Message = cJSON_CreateObject();
									uint8_t free_slot_id_str[5] = {0};
									uint8_t parking_fees_str[5] = {0};
									recently_free_slot = current_free_slot;
									recently_free_slot_location[0] = current_free_Slot_location[0];
									recently_free_slot_location[1] = current_free_Slot_location[1];
									itoa(current_free_slot,free_slot_id_str,10); /*free slot to send*/
									itoa(current_fees_rate,parking_fees_str,10); /*free slot to send*/
									cJSON_AddNumberToObject(Send_Message,"Slot ID",current_free_slot);
									cJSON_AddStringToObject(Send_Message,"Slot Location","x,y");	/*Needs to be handled*/
									cJSON_AddNumberToObject(Send_Message,"Parking Cost",current_fees_rate);
//									cJSON_AddStringToObject(Send_Message,"Inter Points","[{x,y},{x,y},{x,y}]");
//									cJSON_AddStringToObject(Send_Message,"Garage Map","[{x,y},{x,y},{x,y},{x,y},{x,y}]");
									uint8_t* message_to_send = cJSON_Print(Send_Message);
									V2I_Send_Message(message_to_send,strlen(message_to_send),Current_Vehicle_ID,NO_SECURITY);
									break;
								case REQUEST_MAP:
									cJSON* Map_Array = cJSON_CreateArray();
									for(uint32_t i=0;i<MAP_SIZE_PAIRS;i++)
									{
										cJSON* point = cJSON_CreateArray();
										cJSON_AddItemToArray(point,cJSON_CreateNumber(garage_map[i][0]));
										cJSON_AddItemToArray(point,cJSON_CreateNumber(garage_map[i][1]));
										cJSON_AddItemToArray(Map_Array, point);
									}
									uint8_t* Map_Str = cJSON_PrintUnformatted(Map_Array);
									V2I_Send_Message(Map_Str,strlen(Map_Str),Current_Vehicle_ID,NO_SECURITY);
									break;
								case REQUEST_WEATHER:
									cJSON* Weather = cJSON_CreateArray();
									cJSON_AddNumberToObject(Weather,"Temperature",35);
									cJSON_AddNumberToObject(Weather,"Humidity %",95);
									cJSON_AddNumberToObject(Weather,"UV Index",8);
									uint8_t* Weather_Str = cJSON_Print(Weather);
									V2I_Send_Message(Weather_Str,strlen(Weather_Str),Current_Vehicle_ID,NO_SECURITY);
									break;
							/*Section End*/
/*============================================================================================================================================*/
							}
						}
						/*Send message format -> "SEND:x/" */
						else if(strstr(message_recieved,SEND_MESSAGE) != NULL)
						{
/*============================================================================================================================================*/
							/*Send Messages section, to add as message as you want*/
							/*Do Nothing*/
							/*Garage Infrastructure doesn't need to receive information*/

							/*Section End*/
/*============================================================================================================================================*/
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
  MX_TIM4_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */

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

  xReturn = xTaskCreate(
			vIRSensorsTask,      // Task function.
			"IRSensorsTask",     // Name of the task.
			2048,                // Stack size in words.
			NULL,               // Task parameter
			tskIDLE_PRIORITY+1,   // Task priority
			NULL                // Task handle.
	);

	if((xReturn == pdFALSE) || (xReturn == -1))
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

	if((xReturn == pdFALSE) || (xReturn == -1))
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

	if((xReturn == pdFALSE) || (xReturn == -1))
	{
		Err_Handler();
	}

	xReturn = xTaskCreate(
			vV2ICommunicateTask,          // Function that implements the task.
			"V2ICommunicateTask",        // Text name for the task.
			9830,             // Stack size in words, not bytes.
			NULL,             // Parameter passed into the task.
			tskIDLE_PRIORITY+4, // Priority at which the task is created.
			NULL              // Used to pass out the created task's handle.
	);
	if((xReturn == pdFALSE) || (xReturn == -1))
	{
		Err_Handler();
	}

	/*Give semaphore to make infrastructure update once at start*/
	xSemaphoreGive(xSemaphore_Inf_Update);
	/* add threads, ... */
	vTaskStartScheduler();

	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

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
  HAL_GPIO_WritePin(GPIOA, IR_LED_1_Pin|IR_LED_2_Pin|IR_LED_3_Pin|IR_LED_4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|ESP8266_AP_LED_Pin|GPIO_PIN_5|ESP8266_Wi_Fi_Connected_LED_Pin
                          |ESP8266_Internet_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : System_Reset_LED_Pin ESP8266_Init_LED_Pin */
  GPIO_InitStruct.Pin = System_Reset_LED_Pin|ESP8266_Init_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : IR_LED_1_Pin IR_LED_2_Pin IR_LED_3_Pin IR_LED_4_Pin */
  GPIO_InitStruct.Pin = IR_LED_1_Pin|IR_LED_2_Pin|IR_LED_3_Pin|IR_LED_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : IR_Sensor_1_Pin IR_Sensor_2_Pin IR_Sensor_3_Pin IR_Sensor_4_Pin */
  GPIO_InitStruct.Pin = IR_Sensor_1_Pin|IR_Sensor_2_Pin|IR_Sensor_3_Pin|IR_Sensor_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 ESP8266_AP_LED_Pin PB5 ESP8266_Wi_Fi_Connected_LED_Pin
                           ESP8266_Internet_LED_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_12|ESP8266_AP_LED_Pin|GPIO_PIN_5|ESP8266_Wi_Fi_Connected_LED_Pin
                          |ESP8266_Internet_LED_Pin;
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
