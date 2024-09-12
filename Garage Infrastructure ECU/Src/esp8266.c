/************************************************************************************************************************
 * 	Module: esp8266
 * 	File Name: esp8266.c
 *  Authors: Ahmed Desoky
 *	Date: 14/2/2024
 *	*********************************************************************************************************************
 *	Description: ESP8266 Driver source file
 *				 ESP8266 Driver on STM32F103C8T6 Micro-controller.
 *				 Micro-controller Drivers are STM32 HAL Drivers.
 *				 This Driver is designed to support generally any application it's needed to be used in
 *				 You can skip looking in source file functions if you intend you use it only
 *				 This Driver uses the dedicated UART peripheral resources, and no other device or instance can share it
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "esp8266.h"
/***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
/*Private Global variables used within */
static UART_HandleTypeDef* esp8266_huart; 							 /*ESP8266 connected UART global variable*/
static ESP8266_INDICATOR_FLAG ESP8266_Initialized_Flag = LOW;	     /*Flag to indicate initialization*/
static ESP8266_INDICATOR_FLAG ESP8266_Access_Point_Flag = LOW;		 /*Flag to indicate if Access Point is ON or OFF*/
static ESP8266_INDICATOR_FLAG ESP8266_WIFI_Connected_Flag = LOW;	 /*Flag to indicate WiFi connection*/
static ESP8266_INDICATOR_FLAG ESP8266_Internet_Connection_Flag = LOW;/*Flag to indicate Internet connection*/
static ESP8266_INDICATOR_FLAG ESP8266_TCP_SERVER_FLAG = LOW;/*Flag to indicate establishment of TCP Server (no LED used)*/
static ESP8266_INDICATOR_FLAG ESP8266_TCP_CONNECTION_FLAG = LOW;/*Flag to indicate existence of TCP connection (no LED used)*/
static ESP8266_INDICATOR_FLAG TEMP_STOP_RECEIVE = LOW;/*flag to indicate in temporary stop reception while sending commands*/
static ESP8266_INDICATOR_FLAG READY_TO_RECEIVE = LOW;	/*Flag to indicate that receiving data is ongoing and any current command execution must be suspended*/
static ECHO_STATE Echo = ECHO_ON;	/*Flag to indicate Echo state of ESP8266 - for debugging mainly*/
/*These global arrays are mainly used for the reception call back function when receiving packets*/
/*The arrays sizes are always 4 indicating to number of max possible connections*/
static uint8_t RxBuffer_Temp[ESP8266_Rx_Buffers_Sizes]={0};	/*Temporary Buffer to store received data*/
static uint8_t RxBuffer_Connections[ESP8266_MAX_TCP_CONNECTIONS][ESP8266_Rx_Buffers_Sizes]={0};/*Array of buffers to store data from different connections*/
static uint16_t Connections_Packets_Counter[ESP8266_MAX_TCP_CONNECTIONS]={0};/*Array to store how many packets are received*/
static uint32_t RxBuffer_Connections_Data_Counter[ESP8266_MAX_TCP_CONNECTIONS] = {0};/*Array to store size of data used in bytes*/
/***********************************************************************************************************************
 *                     					      PRIVATE FUNCTIONS PROTOTYPES
 ***********************************************************************************************************************/
/*Private Function only used by other function implemented in this driver*/
static ESP8266_RESPONSE ESP8266_SEND_COMMAND(uint8_t* command,uint32_t command_size,uint8_t* response_buffer,\
uint32_t response_buffer_size,uint32_t timeout,SPECIAL_COMMAND TCP_connection_command,SEND_AND_RECEIVE send_receive,uint8_t* Receive_Buffer);
static ESP8266_RESPONSE PARSE_RESPONSE(uint8_t* buffer,uint8_t* expected_response);
static void ESP8266_LED_INDICATOR(ESP8266_LED_STATE LED,ESP8266_INDICATOR_FLAG Flag);
static ECHO_STATE ESP8266_Disable_Echo(void);
static void ESP8266_Delay_ms(uint32_t Delay_ms);
/***********************************************************************************************************************
 *                     					        FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: ESP8266_Init
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Configurations Object
* 				   UART peripheral instance used
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of execution
* Description: Function to initialize ESP8266 module according to the initial configurations
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Init(UART_HandleTypeDef* huartx,const ESP8266_INIT_CONFIG* esp8266_config)
{
	/*if already initialized*/
	if(ESP8266_Initialized_Flag == HIGH)
	{
		return RESPONSE_OK;
	}
	esp8266_huart = huartx;					/*ESP8266 connected UART global variable*/
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	uint8_t Response_Buffer[BUFFER_4] = {0};		/*Buffer to always check AT responses*/
	uint8_t long_command[BUFFER_3] = {0};			/*Buffer used to send long commands that has different parameters*/
	/*-------------------------------------------Reset ESP8266---------------------------------------------------------*/
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND("AT+RST\r\n",15,Response_Buffer,BUFFER_4,TIMEOUT_3,NO,SEND_ONLY,NULL);
	/*--------------------------------------------Check Attention------------------------------------------------------*/
	memset(Response_Buffer,0,BUFFER_4);			/*empty the buffer*/
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND("AT\r\n",5,Response_Buffer,BUFFER_4,TIMEOUT_0,NO,SEND_ONLY,NULL);
	if(Response_Status != RESPONSE_OK) /*Always checking Response*/
	{
		return RESPONSE_NOT_OK;
	}
	/*------------------------------------------Set Operation Mode----------------------------------------------------*/
	memset(Response_Buffer,0,BUFFER_4);			/*empty the buffer*/
	Echo = ESP8266_Disable_Echo();
	if(esp8266_config->Operation_Mode == STATION_MODE)
	{
		Response_Status = ESP8266_SEND_COMMAND("AT+CWMODE_CUR=1\r\n",18,Response_Buffer,BUFFER_4,TIMEOUT_0,NO,SEND_ONLY,NULL);
	}
	else if(esp8266_config->Operation_Mode == SOFT_AP_MODE)
	{
		Response_Status = ESP8266_SEND_COMMAND("AT+CWMODE_CUR=2\r\n",18,Response_Buffer,BUFFER_4,TIMEOUT_0,NO,SEND_ONLY,NULL);
	}
	else if(esp8266_config->Operation_Mode == AP_STATION_MODE)
	{
		Response_Status = ESP8266_SEND_COMMAND("AT+CWMODE_CUR=3\r\n",18,Response_Buffer,BUFFER_4,TIMEOUT_0,NO,SEND_ONLY,NULL);
	}
	if(Response_Status != RESPONSE_OK) /*Always checking Response*/
	{
		return RESPONSE_NOT_OK;
	}
	/*----------------------------------------Disconnect from any AP-------------------------------------------------------*/
	if((esp8266_config->Operation_Mode == STATION_MODE) || (esp8266_config->Operation_Mode == AP_STATION_MODE))
	{
		memset(Response_Buffer,0,BUFFER_4);			/*empty the buffer*/
		Echo = ESP8266_Disable_Echo();
		Response_Status = ESP8266_SEND_COMMAND("AT+CWQAP\r\n",12,Response_Buffer,BUFFER_4,TIMEOUT_0,NO,SEND_ONLY,NULL);
		if(Response_Status != RESPONSE_OK) /*Always checking Response*/
		{
			return RESPONSE_NOT_OK;
		}
	/*----------------------------------------Enable Auto Connection------------------------------------------------------*/
		memset(Response_Buffer,0,BUFFER_4);			/*empty the buffer*/
		Echo = ESP8266_Disable_Echo();
		Response_Status = ESP8266_SEND_COMMAND("AT+CWAUTOCONN=1\r\n",18,Response_Buffer,BUFFER_4,TIMEOUT_0,NO,SEND_ONLY,NULL);
		if(Response_Status != RESPONSE_OK) /*Always checking Response*/
		{
			return RESPONSE_NOT_OK;
		}
	}
	/*------------------------------------Set Station MAC Address----------------------------------------------------------*/
	if((esp8266_config->Operation_Mode == STATION_MODE) || (esp8266_config->Operation_Mode == AP_STATION_MODE))
	{
		memset(Response_Buffer,0,BUFFER_4);			/*empty the buffer*/
		memset(long_command,0,BUFFER_3);			/*empty the buffer*/
		sprintf(long_command,"AT+CIPSTAMAC_DEF=\"%s\"\r\n",esp8266_config->Station_MAC_ADD); /*Store command into the buffer*/
		Echo = ESP8266_Disable_Echo();
		Response_Status = ESP8266_SEND_COMMAND(long_command,BUFFER_3,Response_Buffer,BUFFER_4,TIMEOUT_0,NO,SEND_ONLY,NULL);
		if(Response_Status != RESPONSE_OK) /*Always checking Response*/
		{
			return RESPONSE_NOT_OK;
		}
	}
	/*---------------------------------------Set AP MAC Address------------------------------------------------------------*/
	if((esp8266_config->Operation_Mode == SOFT_AP_MODE) || (esp8266_config->Operation_Mode == AP_STATION_MODE))
	{
		memset(Response_Buffer,0,BUFFER_4);			/*empty the buffer*/
		memset(long_command,0,BUFFER_3);			/*empty the buffer*/
		sprintf(long_command,"AT+CIPAPMAC_DEF=\"%s\"\r\n",esp8266_config->AP_MAC_ADD);/*Store command into the buffer*/
		Echo = ESP8266_Disable_Echo();
		Response_Status = ESP8266_SEND_COMMAND(long_command,BUFFER_3,Response_Buffer,BUFFER_4,TIMEOUT_0,NO,SEND_ONLY,NULL);
		if(Response_Status != RESPONSE_OK) /*Always checking Response*/
		{
			return RESPONSE_NOT_OK;
		}
	/*----------------------------------------Set AP Static IP Address-----------------------------------------------------*/
		memset(Response_Buffer,0,BUFFER_4);			/*empty the buffer*/
		memset(long_command,0,BUFFER_3);			/*empty the buffer*/
		sprintf(long_command,"AT+CIPAP_DEF=\"%s\",\"%s\",\"255.255.255.0\"\r\n",\
								esp8266_config->AP_Static_IP,esp8266_config->AP_Static_IP); /*Store command into the buffer*/
		Echo = ESP8266_Disable_Echo();
		Response_Status = ESP8266_SEND_COMMAND(long_command,BUFFER_3,Response_Buffer,BUFFER_4,TIMEOUT_0,NO,SEND_ONLY,NULL);
		if(Response_Status != RESPONSE_OK) /*Always checking Response*/
		{
			return RESPONSE_NOT_OK;
		}
	}
	/*-------------------------------------------Auto Connect or NOT-------------------------------------------------------*/
	if((esp8266_config->Operation_Mode == STATION_MODE) || (esp8266_config->Operation_Mode == AP_STATION_MODE))
	{
		if(esp8266_config->AP_Auto_Connect == ESP8266_CONNECT)
		{
			/*----------------------------Join Access Point (Connect to WiFi)----------------------------------------------*/
			memset(Response_Buffer,0,BUFFER_4);			/*empty the buffer*/
			memset(long_command,0,BUFFER_3);			/*empty the buffer*/
			/*Store command into the buffer*/
			sprintf(long_command,"AT+CWJAP=\"%s\",\"%s\"\r\n",esp8266_config->Default_SSID,esp8266_config->Default_PASSWD);
			Echo = ESP8266_Disable_Echo();
			Response_Status = ESP8266_SEND_COMMAND(long_command,BUFFER_3,Response_Buffer,BUFFER_4,TIMEOUT_7,NO,SEND_ONLY,NULL);
			Response_Status = PARSE_RESPONSE(Response_Buffer,"WIFI GOT IP");
			if(Response_Status != RESPONSE_OK) /*Always checking Response*/
			{
				ESP8266_WIFI_Connected_Flag = LOW;
			}
			else
			{
				ESP8266_WIFI_Connected_Flag = HIGH;
			}
		}
	}
	/*-----------------------------------Configure AP if it's in AP Mode----------------------------------------------*/
	if((esp8266_config->Operation_Mode == SOFT_AP_MODE) || (esp8266_config->Operation_Mode == AP_STATION_MODE))
	{
		memset(Response_Buffer,0,BUFFER_4);			/*empty the buffer*/
		memset(long_command,0,BUFFER_3);			/*empty the buffer*/
		sprintf(long_command,"AT+CWSAP_DEF=\"%s\",\"%s\",%d,4,%d,0\r\n",esp8266_config->AP_SSID,\
				esp8266_config->AP_PASSWD,esp8266_config->Channel_ID,esp8266_config->Max_AP_Connections); /*Store command into the buffer*/
		Echo = ESP8266_Disable_Echo();
		Response_Status = ESP8266_SEND_COMMAND(long_command,BUFFER_3,Response_Buffer,BUFFER_4,TIMEOUT_2,NO,SEND_ONLY,NULL);
		if(Response_Status != RESPONSE_OK) /*Always checking Response*/
		{
			ESP8266_Access_Point_Flag = LOW;
		}
		else
		{
			ESP8266_Access_Point_Flag = HIGH;
		}
	}
	/*-------------------------------------Check Internet Connection----------------------------------------------*/
	if((esp8266_config->Operation_Mode == STATION_MODE) || (esp8266_config->Operation_Mode == AP_STATION_MODE))
	{
		if(ESP8266_WIFI_Connected_Flag == HIGH)
		{
			memset(Response_Buffer,0,BUFFER_4);			/*empty the buffer*/
			/*PING to Google servers 5 times to check Internet connection*/
			for(uint8_t k=0 ; k<=PING_ITERATIONS ; k++)
			{
				Echo = ESP8266_Disable_Echo();
				Response_Status = ESP8266_SEND_COMMAND("AT+PING=\"8.8.8.8\"\r\n",18,Response_Buffer,BUFFER_4,TIMEOUT_3,NO,SEND_ONLY,NULL);
				/*at least one packet returned*/
				if(Response_Status == RESPONSE_OK)
				{
					break;
				}
			}
			if(Response_Status == RESPONSE_OK)
			{
				ESP8266_Internet_Connection_Flag = HIGH;	/*Connected to Internet*/
			}
			else
			{
				ESP8266_Internet_Connection_Flag = LOW;	/*Not Connected to Internet*/
			}
		}
	}
	/*----------------------------------------Disable TCP Server-------------------------------------------------*/
	memset(Response_Buffer,0,BUFFER_4);			/*empty the buffer*/
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND("AT+CIPMUX?\r\n",18,Response_Buffer,BUFFER_4,TIMEOUT_0,NO,SEND_ONLY,NULL);
	Response_Status = PARSE_RESPONSE(Response_Buffer,"0");
	if(Response_Status != RESPONSE_OK)
	{
		/*If CIPMUX=1, Close all connections Disable TCP Server*/
		Echo = ESP8266_Disable_Echo();
		Response_Status = ESP8266_SEND_COMMAND("AT+CIPCLOSE=5\r\n",16,Response_Buffer,BUFFER_4,TIMEOUT_0,NO,SEND_ONLY,NULL);
		Echo = ESP8266_Disable_Echo();
		Response_Status = ESP8266_SEND_COMMAND("AT+CIPSERVER=0\r\n",18,Response_Buffer,BUFFER_4,TIMEOUT_0,NO,SEND_ONLY,NULL);
		Echo = ESP8266_Disable_Echo();
		Response_Status = ESP8266_SEND_COMMAND("AT+CIPMUX=0\r\n",15,Response_Buffer,BUFFER_0,TIMEOUT_0,NO,SEND_ONLY,NULL);
	}
	ESP8266_Initialized_Flag = HIGH;	/*ESP8266 is initialized as configured*/
	/*Enable LEDs Indicators*/
	ESP8266_LED_INDICATOR(INITIALIZATION,ESP8266_Initialized_Flag);
	ESP8266_LED_INDICATOR(ACCESS_POINT,ESP8266_Access_Point_Flag);
	ESP8266_LED_INDICATOR(WIFI_CONNECT,ESP8266_WIFI_Connected_Flag);
	ESP8266_LED_INDICATOR(INTERNET_CONNECT,ESP8266_Internet_Connection_Flag);
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Get_AP_IP_Address
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Access Point IP Address buffer to store the address
* Parameters (inout): NONE
* Parameters (out):	Access Point IP Address buffer to store the address
* Return value: Response of execution
* Description: Function to get ESP8266 Access Point IP address
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Get_AP_IP_Address(uint8_t* AP_IP_Address)
{
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	uint8_t response_buffer[BUFFER_3] = {0};
	uint8_t Address[BUFFER_0] = {0};
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND("AT+CIFSR\r\n",12,response_buffer,BUFFER_3,TIMEOUT_2,NO,SEND_ONLY,NULL);
	if(Response_Status != RESPONSE_OK) /*Always checking Response*/
	{
		return RESPONSE_NOT_OK;
	}
	/*Parse and get IP from the response*/
	uint32_t i = 0;	/*counter to iterate mainly on the response buffer*/
	while(response_buffer[i])
	{
		if((response_buffer[i] == 'A') && ((response_buffer[i+1]) == 'P') && ((response_buffer[i+2]) == 'I') && ((response_buffer[i+3]) == 'P'))
		{
			i+=6;	/*to make the pointer reach start of the IP Address*/
			uint8_t counter = 0;
			while(response_buffer[i] != '"')
			{
				Address[counter] = response_buffer[i];	/*Store IP Address in the array*/
				i++;
				counter++;
			}
			Address[counter+1] = '\0';
			break;
		}
		i++;
	}
	strcpy(AP_IP_Address,Address);	/*Copy the IP Address to the function parameter*/
	/*check IP is not 0.0.0.0*/
	if(strstr(AP_IP_Address,"0.0.0.0") != NULL)	/*function from string.h library to find a substring in a string*/
	{
		return RESPONSE_NOT_OK;
	}
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Get_Station_IP_Address
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Station IP Address buffer to store the address
* Parameters (inout): NONE
* Parameters (out):	Station IP Address buffer to store the address
* Return value: Response of execution
* Description: Function to get ESP8266 IP address in station mode for it connected to another Access point
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Get_Station_IP_Address(uint8_t* Station_IP_Address)
{
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	uint8_t response_buffer[BUFFER_3] = {0};
	uint8_t Address[BUFFER_0] = {0};
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND("AT+CIFSR\r\n",12,response_buffer,BUFFER_3,TIMEOUT_2,NO,SEND_ONLY,NULL);
	Response_Status = PARSE_RESPONSE(response_buffer,"OK");
	if(Response_Status != RESPONSE_OK) /*Always checking Response*/
	{
		return RESPONSE_NOT_OK;
	}
	/*Parse and get IP from the response*/
	uint32_t i = 0; /*counter to mainly iterate on response buffer*/
	while(response_buffer[i])
	{
		if((response_buffer[i] == 'S') && ((response_buffer[i+1]) == 'T') && ((response_buffer[i+2]) == 'A')\
							&& ((response_buffer[i+3]) == 'I') && ((response_buffer[i+4]) == 'P'))
		{
			i+=7;	/*to make the pointer reach start of the IP Address*/
			uint8_t counter = 0;
			while(response_buffer[i] != '"')
			{
				Address[counter] = response_buffer[i];	/*Store IP Address in the array*/
				i++;
				counter++;
			}
			Address[counter+1] = '\0';
			break;
		}
		i++;
	}
	strcpy(Station_IP_Address,Address);	/*Copy the IP Address to the function parameter*/
	/*check IP is not 0.0.0.0*/
	if(strstr(Station_IP_Address,"0.0.0.0") != NULL)/*function from string.h library to find a substring in a string*/
	{
		ESP8266_WIFI_Connected_Flag = LOW;
		ESP8266_LED_INDICATOR(WIFI_CONNECT,ESP8266_WIFI_Connected_Flag);
		return RESPONSE_NOT_OK;
	}
	ESP8266_WIFI_Connected_Flag = HIGH;
	ESP8266_LED_INDICATOR(WIFI_CONNECT,ESP8266_WIFI_Connected_Flag);
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Get_Connected_Device_Addresses
* Function ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Concerned device MAC and IP Addresses buffers to store the address
* 				   Device ID
* Parameters (inout): NONE
* Parameters (out):	concerned device MAC and IP Addresses buffers to store the address, Device ID
* Return value: Response of execution
* Description: Function to get MAC and IP addresses of a concerned devices connected to ESP8266
* 			   Max number of devices that can be connected to ESP8266 is 4
* 			   also in Post-Build configurations file, pre-determined number of devices to connect is configured
* 			   if it returns RESPONSE_NOT_OK, it means it failed to get the addresses, it may be due to 2 reasons
* 			   1- there's no connection device with such ID and it's most likely
* 			   2- It failed to fetch the addresses
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Get_Connected_Device_Addresses(uint8_t* IP_Address,uint8_t* MAC_Address,uint8_t Device_ID)
{
	uint8_t Devices_Number = 0;				/*Local variable to store number of connected devices*/
	uint8_t counter = 0;					/*counter used to Parse and Get Addresses*/
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	uint8_t response_buffer[BUFFER_4] = {0};
	uint8_t IP_Address_local[BUFFER_0] = {0};
	uint8_t MAC_Address_local[BUFFER_0] = {0};
	if(ESP8266_Access_Point_Flag == LOW)
	{
		return RESPONSE_NOT_OK;
	}
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND("AT+CWLIF\r\n",12,response_buffer,BUFFER_4,TIMEOUT_4,NO,SEND_ONLY,NULL);
	if(Response_Status != RESPONSE_OK) /*Always checking Response*/
	{
		return RESPONSE_NOT_OK;
	}
	/*Count number of connected devices first*/
	uint8_t buffer_counter = 0;
	while(response_buffer[buffer_counter])
	{
		if(response_buffer[buffer_counter] == ',')
		{
			Devices_Number++;
		}
		buffer_counter++;
	}
	/*If no devices connected*/
	if(Devices_Number < 1)
	{
		return RESPONSE_NOT_OK;
	}
	/*Parse and get chosen device addresses*/
	uint32_t i = 0; /*counter to mainly iterate on response buffer*/
	switch(Device_ID)
	{
		case 0:
			if(Devices_Number > 0)
			{
				/*Start Parsing*/
				counter = 0;
				while(response_buffer[i] != ',')
				{
					IP_Address_local[counter] = response_buffer[i];
					i++;
					counter++;
				}
				IP_Address_local[counter+1] = '\0';
				/*Got IP Address*/
				i++;
				counter = 0;
				while(response_buffer[i] != '\r')
				{
					MAC_Address_local[counter] = response_buffer[i];
					i++;
					counter++;
				}
				MAC_Address_local[counter+1] = '\0';
			}
			else
			{
				/*Wrong chosen Device ID*/
				return RESPONSE_NOT_OK;
			}
			break;
		case 1:
			if(Devices_Number > 1)
			{
				/*Move until the second device*/
				while(response_buffer[i] != '\n')
				{
					i++;
				}
				i++;	/*to skip the '\n' character*/
				/*Start Parsing*/
				counter = 0;
				while(response_buffer[i] != ',')
				{
					IP_Address_local[counter] = response_buffer[i];
					i++;
					counter++;
				}
				IP_Address_local[counter+1] = '\0';
				/*Got IP Address*/
				i++;
				counter = 0;
				while(response_buffer[i] != '\r')
				{
					MAC_Address_local[counter] = response_buffer[i];
					i++;
					counter++;
				}
				MAC_Address_local[counter+1] = '\0';
			}
			else
			{
				/*Wrong chosen Device ID*/
				return RESPONSE_NOT_OK;
			}
			break;
		case 2:
			if(Devices_Number > 2)
			{
				/*Move until the second device*/
				while(response_buffer[i] != '\n')
				{
					i++;
				}
				i++;	/*to skip the '\n' character*/
				/*Move until the third device*/
				while(response_buffer[i] != '\n')
				{
					i++;
				}
				i++;	/*to skip the '\n' character*/
				/*Start Parsing*/
				counter = 0;
				while(response_buffer[i] != ',')
				{
					IP_Address_local[counter] = response_buffer[i];
					i++;
					counter++;
				}
				IP_Address_local[counter+1] = '\0';
				/*Got IP Address*/
				i++;
				counter = 0;
				while(response_buffer[i] != '\r')
				{
					MAC_Address_local[counter] = response_buffer[i];
					i++;
					counter++;
				}
				MAC_Address_local[counter+1] = '\0';
			}
			else
			{
				/*Wrong chosen Device ID*/
				return RESPONSE_NOT_OK;
			}
			break;
		case 3:
			if(Devices_Number == 4)	/*Max allowed devices to connect to ESP8266 = 4 Devices*/
			{
				/*Move until the second device*/
				while(response_buffer[i] != '\n')
				{
					i++;
				}
				i++;	/*to skip the '\n' character*/
				/*Move until the third device*/
				while(response_buffer[i] != '\n')
				{
					i++;
				}
				i++;	/*to skip the '\n' character*/
				/*Move until the fourth device*/
				while(response_buffer[i] != '\n')
				{
					i++;
				}
				i++;	/*to skip the '\n' character*/
				/*Start Parsing*/
				counter = 0;
				while(response_buffer[i] != ',')
				{
					IP_Address_local[counter] = response_buffer[i];
					i++;
					counter++;
				}
				IP_Address_local[counter+1] = '\0';
				/*Got IP Address*/
				i++;
				counter = 0;
				while(response_buffer[i] != '\r')
				{
					MAC_Address_local[counter] = response_buffer[i];
					i++;
					counter++;
				}
				MAC_Address_local[counter+1] = '\0';
			}
			else
			{
				/*Wrong chosen Device ID*/
				return RESPONSE_NOT_OK;
			}
			break;
		/*In case of wrong Device ID*/
		default:
			return RESPONSE_NOT_OK;
	}
	strcpy(IP_Address,IP_Address_local);	/*Copy the IP Address to the function parameter*/
	strcpy(MAC_Address,MAC_Address_local);	/*Copy the MAC Address to the function parameter*/
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Get_Connected_Devices_Number
* Function ID[hex]: 0x04
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): variable passed by address to store number of the connected devices to ESP8266 Access Point
* Parameters (inout): NONE
* Parameters (out):	variable passed by address to store number of the connected devices to ESP8266 Access Point
* Return value: Response of execution
* Description: Function to get number of connected devices to ESP8266 Access Point
* 			   Max number of devices that can be connected to ESP8266 is 4
* 			   also in Post-Build configurations file, pre-determined number of devices to connect is configured
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Get_Connected_Devices_Number(uint8_t* Number)
{
	uint8_t counter = 0;					/*counter to count number of devices*/
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	uint8_t response_buffer[BUFFER_4] = {0};
	if(ESP8266_Access_Point_Flag == LOW)
	{
		*Number = 0;
		return RESPONSE_NOT_OK;
	}
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND("AT+CWLIF\r\n",12,response_buffer,BUFFER_4,TIMEOUT_4,NO,SEND_ONLY,NULL);
	if(Response_Status != RESPONSE_OK) /*Always checking Response*/
	{
		*Number = 0;
		return RESPONSE_NOT_OK;
	}
	/*Count number of connected devices*/
	uint32_t i = 0; /*counter to mainly iterate on response buffer*/
	while(response_buffer[i])
	{
		if(response_buffer[i] == ',')
		{
			counter++;
		}
		i++;
	}
	*Number = counter;
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Check_Internet_Connection
* Function ID[hex]: 0x05
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of Internet connection
* Description: Function to return Internet status
* 			   RESPONSE_OK means connected to Internet
* 			   RESPONSE_NOT_OK means not connected to Internet
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Check_Internet_Connection(void)
{
	if(ESP8266_WIFI_Connected_Flag == LOW)
	{
		ESP8266_Internet_Connection_Flag = LOW;
		ESP8266_LED_INDICATOR(INTERNET_CONNECT,ESP8266_Internet_Connection_Flag);
		return RESPONSE_NOT_OK;
	}
	uint8_t response_buffer[BUFFER_1] = {0};
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	/*PING to Google servers 5 times to check Internet connection*/
	for(uint8_t k=0 ; k<=PING_ITERATIONS ; k++)
	{
		Echo = ESP8266_Disable_Echo();
		Response_Status = ESP8266_SEND_COMMAND("AT+PING=\"8.8.8.8\"\r\n",18,response_buffer,BUFFER_1,TIMEOUT_3,NO,SEND_ONLY,NULL);
		/*at least one packet returned*/
		if(Response_Status == RESPONSE_OK)
		{
			break;
		}
	}
	if(Response_Status == RESPONSE_OK)
	{
		ESP8266_Internet_Connection_Flag = HIGH;	/*Connected to Internet*/
		ESP8266_LED_INDICATOR(INTERNET_CONNECT,ESP8266_Internet_Connection_Flag);
	}
	else
	{
		ESP8266_Internet_Connection_Flag = LOW;	/*Not Connected to Internet*/
		ESP8266_LED_INDICATOR(INTERNET_CONNECT,ESP8266_Internet_Connection_Flag);
	}
	return Response_Status;
}
/************************************************************************************************************************
* Function Name: ESP8266_WiFi_Disconnect
* Function ID[hex]: 0x06
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of execution
* Description: Function to disconnect from the WiFi Access Point that ESP8266 is connected to
* 			   This function is only used when the esp8266 is configured in Station or AP/Station Modes
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_WiFi_Disconnect(void)
{
	uint8_t response_buffer[BUFFER_1] = {0};
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND("AT+CWQAP\r\n",12,response_buffer,BUFFER_1,TIMEOUT_0,NO,SEND_ONLY,NULL);
	if(Response_Status != RESPONSE_OK) /*Always checking Response*/
	{

		return RESPONSE_NOT_OK;
	}
	ESP8266_WIFI_Connected_Flag = LOW;
	ESP8266_Internet_Connection_Flag = LOW;
	ESP8266_LED_INDICATOR(WIFI_CONNECT,ESP8266_WIFI_Connected_Flag);
	ESP8266_LED_INDICATOR(INTERNET_CONNECT,ESP8266_Internet_Connection_Flag);
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_WiFi_Connect
* Function ID[hex]: 0x07
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Access Point SSID
* 				   Access Point Password
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of execution
* Description: Function to make ESP8266 connect to WiFi Access Point
* 			   This function is only used when the esp8266 is configured in Station or AP/Station Modes
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_WiFi_Connect(uint8_t* SSID,uint8_t* Password)
{
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	uint8_t response_buffer[BUFFER_2] = {0};
	uint8_t long_command[BUFFER_1] = {0};
	sprintf(long_command,"AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,Password); /*Store command into the buffer*/
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND(long_command,BUFFER_1,response_buffer,BUFFER_2,TIMEOUT_7,NO,SEND_ONLY,NULL);
	Response_Status = PARSE_RESPONSE(response_buffer,"WIFI GOT IP");
	if(Response_Status != RESPONSE_OK) /*Always checking Response*/
	{
		return RESPONSE_NOT_OK;
	}
	ESP8266_WIFI_Connected_Flag = HIGH;	/*Connected to WiFi Successfully*/
	ESP8266_LED_INDICATOR(WIFI_CONNECT,ESP8266_WIFI_Connected_Flag);
	/*-------------------------------Try to check if connected to Internet or not-------------------------------------*/
	memset(response_buffer,0,BUFFER_2);			/*empty the buffer*/
	for(uint8_t k=0 ; k<=PING_ITERATIONS ; k++)
	{
		Echo = ESP8266_Disable_Echo();
		Response_Status = ESP8266_SEND_COMMAND("AT+PING=\"8.8.8.8\"\r\n",18,response_buffer,BUFFER_2,TIMEOUT_3,NO,SEND_ONLY,NULL);
		/*at least one packet returned*/
		if(Response_Status == RESPONSE_OK)
		{
			break;
		}
	}
	if(Response_Status == RESPONSE_OK)
	{
		ESP8266_Internet_Connection_Flag = HIGH;	/*Connected to Internet*/
		ESP8266_LED_INDICATOR(INTERNET_CONNECT,ESP8266_Internet_Connection_Flag);
	}
	else
	{
		ESP8266_Internet_Connection_Flag = LOW;	/*Not Connected to Internet*/
		ESP8266_LED_INDICATOR(INTERNET_CONNECT,ESP8266_Internet_Connection_Flag);
	}
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Configure_AccessPoint
* Function ID[hex]: 0x08
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): New Access Point SSID
* 				   New Access Point Password
* 			       Channel ID
* 				   Max allowed stations to connect
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of execution
* Description: Function to make ESP8266 re-configure ESP8266 Access Point
* 			   This function is only used when the esp8266 is configured in Access Point or Station/Access Point Modes
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Configure_AccessPoint(uint8_t* SSID,uint8_t* Password,uint8_t Channel_ID,uint8_t Max_AP_Connections)
{
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	uint8_t response_buffer[BUFFER_2] = {0};
	uint8_t long_command[BUFFER_2] = {0};
	/*Store command into the buffer*/
	sprintf(long_command,"AT+CWSAP_CUR=\"%s\",\"%s\",%d,4,%d,0\r\n",SSID,Password,Channel_ID,Max_AP_Connections);
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND(long_command,BUFFER_2,response_buffer,BUFFER_2,TIMEOUT_2,NO,SEND_ONLY,NULL);
	if(Response_Status != RESPONSE_OK) /*Always checking Response*/
	{
		ESP8266_Access_Point_Flag = LOW;
		return RESPONSE_NOT_OK;
	}
	else
	{
		ESP8266_Access_Point_Flag = HIGH;
		return RESPONSE_OK;
	}
}
/************************************************************************************************************************
* Function Name: ESP8266_Search_AccessPoint
* Function ID[hex]: 0x09
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Access Point SSID
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of found or not
* Description: Function to make search for certain Access Point
* 			   RESPONSE_OK means found
* 			   RESPONSE_NOT_OK means not found
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Search_AccessPoint(uint8_t* SSID)
{
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	uint8_t response_buffer[BUFFER_2] = {0};
	uint8_t long_command[BUFFER_2] = {0};
	sprintf(long_command,"AT+CWLAP=\"%s\"\r\n",SSID); /*Store command into the buffer*/
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND(long_command,BUFFER_2,response_buffer,BUFFER_2,TIMEOUT_3,NO,SEND_ONLY,NULL);
	Response_Status = PARSE_RESPONSE(response_buffer,SSID);
	if(Response_Status != RESPONSE_OK) /*Always checking Response*/
	{
		return RESPONSE_NOT_OK;
	}
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Establish_TCP_Server
* Function ID[hex]: 0x010
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of execution
* Description: This Function establish TCP Server on ESP8266
* 			   If TCP Server is ON, ESP8266 can't establish TCP connection
* 			   so don't call ESP8266_Establish_TCP_Connection function if TCP server is ON
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Establish_TCP_Server(void)
{
	if(ESP8266_TCP_SERVER_FLAG == HIGH)
	{
		return RESPONSE_OK; /*Means its already Established, not need to send the commands again*/
	}
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	uint8_t response_buffer[BUFFER_0] = {0};
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND("AT+CIPMUX=1\r\n",15,response_buffer,BUFFER_0,TIMEOUT_0,NO,SEND_ONLY,NULL);
	if(Response_Status != RESPONSE_OK)
	{
		return RESPONSE_NOT_OK;
	}
	memset(response_buffer,0,BUFFER_0);			/*empty the buffer*/
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND("AT+CIPSERVER=1\r\n",18,response_buffer,BUFFER_0,TIMEOUT_0,NO,SEND_ONLY,NULL);
	if(Response_Status != RESPONSE_OK)
	{
		return RESPONSE_NOT_OK;
	}
	/*Set TCP Server connection Timeout as default = 180 seconds*/
	memset(response_buffer,0,BUFFER_0);			/*empty the buffer*/
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND("AT+CIPSTO=180\r\n",18,response_buffer,BUFFER_0,TIMEOUT_0,NO,SEND_ONLY,NULL);
	if(Response_Status != RESPONSE_OK)
	{
		return RESPONSE_NOT_OK;
	}
	ESP8266_TCP_SERVER_FLAG = HIGH;
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Disband_TCP_Server
* Function ID[hex]: 0x11
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of execution
* Description: Function to disband the TCP server
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Disband_TCP_Server(void)
{
	if(ESP8266_TCP_SERVER_FLAG == LOW)
	{
		return RESPONSE_OK;	/*Means its already Disbanded, not need to send the commands again*/
	}
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	uint8_t response_buffer[BUFFER_0] = {0};
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND("AT+CIPSERVER=0\r\n",18,response_buffer,BUFFER_0,TIMEOUT_0,NO,SEND_ONLY,NULL);
	if(Response_Status != RESPONSE_OK)
	{
		return RESPONSE_NOT_OK;
	}
	memset(response_buffer,0,BUFFER_0);			/*empty the buffer*/
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND("AT+CIPMUX=0\r\n",15,response_buffer,BUFFER_0,TIMEOUT_0,NO,SEND_ONLY,NULL);
	if(Response_Status != RESPONSE_OK)
	{
		return RESPONSE_NOT_OK;
	}
	ESP8266_TCP_SERVER_FLAG = LOW;
	ESP8266_TCP_CONNECTION_FLAG = LOW;
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Establish_TCP_Connection
* Function ID[hex]: 0x12
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): IP address of the TCP server to connect to
* 				   Connection socket
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of execution
* Description:	Function to establish a TCP connection with a server
* 				Don't use ESP8266_Establish_TCP_Server function when you are going to establish a TCP connection
* 				Don't call this function more than one time or it will be ineffective, unless you called
* 				ESP8266_Close_TCP_Connection and the previous connection is already closed
* 				If TCP server mode is off, only 1 connection can be established and closed
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Establish_TCP_Connection(uint8_t* IP_Address,uint16_t Socket_Port)
{
//	if(ESP8266_TCP_CONNECTION_FLAG == HIGH)
//	{
//		return RESPONSE_BUSY;
//	}
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	uint8_t response_buffer[BUFFER_4] = {0};
	uint8_t long_command[BUFFER_2] = {0};
	sprintf(long_command,"AT+CIPSTART=\"TCP\",\"%s\",%u\r\n",IP_Address,Socket_Port); /*Store command into the buffer*/
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND(long_command,BUFFER_2,response_buffer,BUFFER_4,TIMEOUT_1,NO,SEND_ONLY,NULL);
	Response_Status = PARSE_RESPONSE(response_buffer,"CONNECT");
	if(Response_Status != RESPONSE_OK)
	{
		return RESPONSE_NOT_OK;
	}
	ESP8266_TCP_CONNECTION_FLAG = HIGH;
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Close_TCP_Connection
* Function ID[hex]: 0x13
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Connection ID, that is concerned to be closed
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of execution
* Description: Function to close a TCP connection
*			   Number of max connection in case of TCP Server mode is 5, 0-4, if you chose ID=5 all Links will be closed
*			   If TCP server mode is off, only 1 connection can be established and closed
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Close_TCP_Connection(uint8_t Connection_ID)
{
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	uint8_t response_buffer[BUFFER_6] = {0};
	uint8_t long_command[BUFFER_0] = {0};
	if(ESP8266_TCP_SERVER_FLAG == LOW)
	{
		if(ESP8266_TCP_CONNECTION_FLAG == LOW)	/*To return OK if connection in this case is already closed*/
		{
			return RESPONSE_OK;
		}
		memset(response_buffer,0,BUFFER_6);			/*empty the buffer*/
		Echo = ESP8266_Disable_Echo();
		Response_Status = ESP8266_SEND_COMMAND("AT+CIPCLOSE\r\n",16,response_buffer,BUFFER_6,TIMEOUT_0,NO,SEND_ONLY,NULL);
		Response_Status = PARSE_RESPONSE(response_buffer,"CLOSED");	/*No need to check for the CLOSED*/
		ESP8266_TCP_CONNECTION_FLAG = LOW;
	}
	else if(ESP8266_TCP_SERVER_FLAG == HIGH)
	{
		if(ESP8266_TCP_CONNECTION_FLAG == LOW) /*To return OK if all connections in this case are already closed*/
		{
			return RESPONSE_OK;
		}
		memset(response_buffer,0,BUFFER_6);			/*empty the buffer*/
		memset(long_command,0,BUFFER_0);			/*empty the buffer*/
		sprintf(long_command,"AT+CIPCLOSE=%u\r\n",Connection_ID); /*Store command into the buffer*/
		Echo = ESP8266_Disable_Echo();
		Response_Status = ESP8266_SEND_COMMAND(long_command,BUFFER_0,response_buffer,BUFFER_6,TIMEOUT_0,NO,SEND_ONLY,NULL);
		Response_Status = PARSE_RESPONSE(response_buffer,"CLOSED"); /*No need to check for the CLOSED*/
		memset(response_buffer,0,BUFFER_6);			/*empty the buffer*/
		Echo = ESP8266_Disable_Echo();
		Response_Status = ESP8266_SEND_COMMAND("AT+CIPSTATUS\r\n",16,response_buffer,BUFFER_6,TIMEOUT_1,NO,SEND_ONLY,NULL);
		if(Response_Status != RESPONSE_OK)
		{
			return RESPONSE_NOT_OK;
		}
		/*Checking if there's a connection or not*/
		uint8_t count_tcp_connections = 0;
		uint32_t i = 0;	/*counter to iterate mainly on response buffer*/
		i+=7;	/*move after 7 characters to skip "STATUS" word*/
		while(PARSE_RESPONSE(response_buffer+i,"CIPSTATUS") == RESPONSE_OK)
		{
			count_tcp_connections++;
			i+=45;	/*to move to the next connection data - an adding for fixing*/
		}
		if(count_tcp_connections > 0)
		{
			ESP8266_TCP_CONNECTION_FLAG = HIGH;
		}
		else
		{
			ESP8266_TCP_CONNECTION_FLAG = LOW;
		}
	}
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Check_TCP_Connections_Number
* Function ID[hex]: 0x14
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): variable by address to store number of ongoing TCP connections
* Parameters (inout): NONE
* Parameters (out): variable by address to store number of ongoing TCP connections
* Return value: Response of execution
* Description: Function to get number of ongoing TCP connections
* 			   If TCP server is ON, it can be up to 5 connections
* 			   If TCP server is OFF, it is up to 1 connection only*
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Check_TCP_Connections_Number(uint8_t* Connections_Number)
{
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	uint8_t response_buffer[BUFFER_5] = {0};
	Echo = ESP8266_Disable_Echo();
	Response_Status = ESP8266_SEND_COMMAND("AT+CIPSTATUS\r\n",16,response_buffer,BUFFER_5,TIMEOUT_1,NO,SEND_ONLY,NULL);
	if(Response_Status != RESPONSE_OK)
	{
		return RESPONSE_NOT_OK;
	}
	if(ESP8266_TCP_SERVER_FLAG == LOW)
	{
		Response_Status = PARSE_RESPONSE(response_buffer,"CIPSTATUS");
		if(Response_Status != RESPONSE_OK)
		{
			*Connections_Number = 0;
			ESP8266_TCP_CONNECTION_FLAG = LOW;
		}
		else if(Response_Status == RESPONSE_OK)
		{
			*Connections_Number = 1;
			ESP8266_TCP_CONNECTION_FLAG = HIGH;
		}
	}
	else if(ESP8266_TCP_SERVER_FLAG == HIGH)
	{
		uint8_t count_tcp_connections = 0;
		uint32_t i = 0;	/*counter to iterate mainly on response buffer*/
		i+=7;	/*move after 7 characters to skip "STATUS" word*/
		while(PARSE_RESPONSE(response_buffer+i,"CIPSTATUS") == RESPONSE_OK)
		{
			count_tcp_connections++;
			i+=45;	/*to move to the next connection data - an adding for fixing*/
		}
		if(count_tcp_connections > 0)
		{
			*Connections_Number = count_tcp_connections;
			ESP8266_TCP_CONNECTION_FLAG = HIGH;
		}
		else
		{
			*Connections_Number = 0;
			ESP8266_TCP_CONNECTION_FLAG = LOW;
		}
	}
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Send_TCP_Packets
* Function ID[hex]: 0x15
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Data to be sent,
* 				   Data size
* 			       TCP connection ID to send the data to
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of execution
* Description: Function to Send TCP Packets to the concerned established connection
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Send_TCP_Packets(uint8_t* Data,uint32_t Data_size,uint8_t Connection_ID,SEND_AND_RECEIVE send_receive,uint8_t* Receive_Buffer)
{
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	uint8_t response_buffer[BUFFER_6] = {0};
	uint8_t long_command[BUFFER_1] = {0};
	uint8_t send_trials = 0;
	if(ESP8266_TCP_SERVER_FLAG == LOW)
	{
		sprintf(long_command,"AT+CIPSEND=%u\r\n",Data_size);
	}
	else if(ESP8266_TCP_SERVER_FLAG == HIGH)
	{
		sprintf(long_command,"AT+CIPSEND=%u,%u\r\n",Connection_ID,Data_size);
	}
	while((Response_Status != RESPONSE_OK) && (send_trials < COMMAND_TRIALS_NUMBER))
	{
		send_trials++;
		Echo = ESP8266_Disable_Echo();
		Response_Status = ESP8266_SEND_COMMAND(long_command,BUFFER_1,response_buffer,BUFFER_6,TIMEOUT_0,YES,SEND_ONLY,NULL);
		Response_Status = PARSE_RESPONSE(response_buffer," ");
		if(Response_Status != RESPONSE_OK)
		{
			continue;
		}
		memset(response_buffer,0,BUFFER_6);			/*empty the buffer*/
		Response_Status = ESP8266_SEND_COMMAND(Data,Data_size,response_buffer,BUFFER_6,(TIMEOUT_0*(Data_size)),NO,send_receive,Receive_Buffer);
		Response_Status = PARSE_RESPONSE(response_buffer,"SEND OK");
		if(Response_Status != RESPONSE_OK)
		{
			continue;
		}
		memset(response_buffer,0,BUFFER_6);			/*empty the buffer*/
		if(Response_Status == RESPONSE_OK)
		{
			break;
		}
	}
	/*check what happened in the previous while loop*/
	if(Response_Status != RESPONSE_OK)
	{
		return RESPONSE_NOT_OK;
	}
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Start_Receive_TCP_Packets
* Function ID[hex]: 0x16
* Sync/Async: Asynchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of execution
* Description: Function to give permission to start receiving TCP packets
* 			   Make sure that TCP serves is ON or if it's OFF make sure that there's ongoing TCP connection
* 			   Be careful in timing while using this function
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Start_Receive_TCP_Packets(void)
{
	if((READY_TO_RECEIVE == HIGH) && (TEMP_STOP_RECEIVE == LOW))
	{
		return RESPONSE_OK;
	}
	HAL_StatusTypeDef HAL_Status = HAL_UARTEx_ReceiveToIdle_DMA(esp8266_huart,RxBuffer_Temp,ESP8266_Rx_Buffers_Sizes);
	if(HAL_Status != HAL_OK)
	{
		return RESPONSE_NOT_OK;
	}
	READY_TO_RECEIVE = HIGH;
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Receive_TCP_Packets_Callback
* Function ID[hex]: 0x17
* Sync/Async: Asynchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Callback function, don't call it in your application, just call it in the specific HAL Callback function
* 			   Please, check STM HAL configurations section
* 			   This function privately store any received data in a private global buffer related to the connection ID
* 			   every time it receives data and increment their number of packets counter and data sizes counter
************************************************************************************************************************/
void ESP8266_Receive_TCP_Packets_Callback(uint8_t* Device_Connected)
{
	ESP8266_RESPONSE Response = 0;
	uint16_t Received_Data_Size = 0;
	uint8_t Received_Data_Size_char[5] = {0};
	uint8_t header_size = 0;
	/*buffer is empty, false interrupt*/
	if(strlen(RxBuffer_Temp) == 0)
	{
		memset(RxBuffer_Temp,0,ESP8266_Rx_Buffers_Sizes);			/*empty the buffer*/
		HAL_UARTEx_ReceiveToIdle_DMA(esp8266_huart,RxBuffer_Temp,ESP8266_Rx_Buffers_Sizes);	/*Receive Again*/
		return;
	}
	if((PARSE_RESPONSE(RxBuffer_Temp,"STA_CONNECT") == RESPONSE_OK)
			|| (PARSE_RESPONSE(RxBuffer_Temp,"STA_IP") == RESPONSE_OK))
	{
		*Device_Connected = 1;
		memset(RxBuffer_Temp,0,ESP8266_Rx_Buffers_Sizes);			/*empty the buffer*/
		HAL_UARTEx_ReceiveToIdle_DMA(esp8266_huart,RxBuffer_Temp,ESP8266_Rx_Buffers_Sizes);	/*Receive Again*/
		return;
	}
	if(PARSE_RESPONSE(RxBuffer_Temp,"CLOSED\r") == RESPONSE_OK)
	{
		ESP8266_TCP_CONNECTION_FLAG = LOW;
	}
	if(PARSE_RESPONSE(RxBuffer_Temp,"CONNECT\r") == RESPONSE_OK)
	{
		ESP8266_TCP_CONNECTION_FLAG = HIGH;
	}
	Response = PARSE_RESPONSE(RxBuffer_Temp,"+IPD");
	if(Response != RESPONSE_OK)
	{
		memset(RxBuffer_Temp,0,ESP8266_Rx_Buffers_Sizes);			/*empty the buffer*/
		/*Receive Again*/
		HAL_UARTEx_ReceiveToIdle_DMA(esp8266_huart,RxBuffer_Temp,ESP8266_Rx_Buffers_Sizes);
		return;										/*False Receive, not packets*/
	}

	/*Section to handle extra IPD messages that contains garbage*/
	uint8_t count_IPD = 10;	/*count up to 10 times*/
	uint32_t count_to_skip_extra_IPD = 0;
	uint32_t garbage_data_size = 0;
	uint8_t garbage_data_Size_char[5] = {0};
	for(uint8_t g = 0;g<count_IPD;g++)
	{
		while((RxBuffer_Temp[count_to_skip_extra_IPD] != 'I') || (RxBuffer_Temp[count_to_skip_extra_IPD+1] != 'P') || (RxBuffer_Temp[count_to_skip_extra_IPD+2] != 'D'))
		{
			count_to_skip_extra_IPD++;
		}
		/*Iterate to ',' to get received size*/
		while(RxBuffer_Temp[count_to_skip_extra_IPD] != ',')
		{
			count_to_skip_extra_IPD++;
		}
		count_to_skip_extra_IPD++;	/*Skip ',' character*/
		uint16_t j = 0; /*counter to iterate on the Received_Data_Size_char*/
		while(RxBuffer_Temp[count_to_skip_extra_IPD] != ':')
		{
			garbage_data_Size_char[j] = RxBuffer_Temp[count_to_skip_extra_IPD];
			j++;
			count_to_skip_extra_IPD++;
		}
		count_to_skip_extra_IPD++;
		/*Convert Characters to integers*/
		garbage_data_size = atoi(garbage_data_Size_char);
		if(PARSE_RESPONSE(RxBuffer_Temp+garbage_data_size+count_to_skip_extra_IPD,"+IPD") != RESPONSE_OK)
		{
			header_size = garbage_data_size+count_to_skip_extra_IPD*g;
			break;
		}
	}
	/*------------------------------------------------------------------------------------------------*/
	while((RxBuffer_Temp[header_size] != 'I') || (RxBuffer_Temp[header_size+1] != 'P') || (RxBuffer_Temp[header_size+2] != 'D'))
	{
		header_size++;
	}
	uint32_t i = header_size;	/*counter to iterate on the RxBuffer_Temp*/
	if(ESP8266_TCP_SERVER_FLAG == LOW)
	{
		/*Iterate to ',' to get received size*/
		while(RxBuffer_Temp[i] != ',')
		{
			i++;
		}
		i++;	/*Skip ',' character*/
		uint16_t j = 0; /*counter to iterate on the Received_Data_Size_char*/
		while(RxBuffer_Temp[i] != ':')
		{
			Received_Data_Size_char[j] = RxBuffer_Temp[i];
			j++;
			i++;
		}
		i++;
		/*Convert Characters to integers*/
		Received_Data_Size = atoi(Received_Data_Size_char);
		/*wrong packet received*/
		if(Received_Data_Size < 7)
		{
			memset(RxBuffer_Temp,0,ESP8266_Rx_Buffers_Sizes);			/*empty the buffer*/
			/*Receive Again*/
			HAL_UARTEx_ReceiveToIdle_DMA(esp8266_huart,RxBuffer_Temp,ESP8266_Rx_Buffers_Sizes);
			return;
		}
		if(Connections_Packets_Counter[0] == 0)
		{
			/*Wait until full reception of RxBuffer_Temp*/
			while(RxBuffer_Temp[Received_Data_Size+i-1] == '\0');
			memcpy(RxBuffer_Connections[0],RxBuffer_Temp+i,Received_Data_Size);
			/*Remove CLOSED sub string from the data*/
			if(PARSE_RESPONSE(RxBuffer_Connections[0],"CLOSED\r") == RESPONSE_OK)
			{
				while((RxBuffer_Connections[0][i] != 'C') || (RxBuffer_Connections[0][i+1] != 'L') || (RxBuffer_Connections[0][i+2] != 'O')\
				|| (RxBuffer_Connections[0][i+3] != 'S') || (RxBuffer_Connections[0][i+4] != 'E') || (RxBuffer_Connections[0][i+5] != 'D'))
				{
					i++;
				}
				/*iterate until the end and remove every character from CLOSED*/
				while(RxBuffer_Connections[0][i])
				{
					RxBuffer_Connections[0][i] = '\0';
					i++;
				}
				//Received_Data_Size -=9; /*Line to be checked*/ /*remove the CLOSED characters size*/
			}
			RxBuffer_Connections_Data_Counter[0] = Received_Data_Size;
		}
		else
		{
			uint16_t z = RxBuffer_Connections_Data_Counter[0];	/*counter for the output string*/
			/****************test priority here in case of accumulated received data***********/
			/*Wait until full reception of RxBuffer_Temp*/
			while(RxBuffer_Temp[Received_Data_Size+i-1] == '\0');
			memcpy(RxBuffer_Connections[0]+z,RxBuffer_Temp+i,Received_Data_Size);
			/*Remove CLOSED sub string from the data*/
			if(PARSE_RESPONSE(RxBuffer_Connections[0],"CLOSED\r") == RESPONSE_OK)
			{
				while((RxBuffer_Connections[0][i] != 'C') || (RxBuffer_Connections[0][i+1] != 'L') || (RxBuffer_Connections[0][i+2] != 'O')\
				|| (RxBuffer_Connections[0][i+3] != 'S') || (RxBuffer_Connections[0][i+4] != 'E') || (RxBuffer_Connections[0][i+5] != 'D'))
				{
					i++;
				}
				/*iterate until the end and remove every character from CLOSED*/
				while(RxBuffer_Connections[0][i])
				{
					RxBuffer_Connections[0][i] = '\0';
					i++;
				}
				//Received_Data_Size -=9; /*Line to be checked*/ /*remove the CLOSED characters size*/
			}
			RxBuffer_Connections_Data_Counter[0] += Received_Data_Size;
		}
		Connections_Packets_Counter[0]++;
	}
	else if(ESP8266_TCP_SERVER_FLAG == HIGH)
	{
		uint8_t Connection_ID = 0;
		uint8_t Connection_ID_Char[2] = {0};
		/*Iterate to ',' to get connection ID*/
		while(RxBuffer_Temp[i] != ',')
		{
			i++;
		}
		i++;
		Connection_ID_Char[0] = RxBuffer_Temp[i];
		Connection_ID = atoi(Connection_ID_Char);
		/*Iterate to ',' to get received size*/
		while(RxBuffer_Temp[i] != ',')
		{
			i++;
		}
		i++;
		uint16_t j = 0; /*counter to iterate on the Received_Data_Size_char*/
		while(RxBuffer_Temp[i] != ':')
		{
			Received_Data_Size_char[j] = RxBuffer_Temp[i];
			j++;
			i++;
		}
		i++;
		Received_Data_Size = atoi(Received_Data_Size_char);
		/*wrong packet received*/
		if(Received_Data_Size < 7)
		{
			memset(RxBuffer_Temp,0,ESP8266_Rx_Buffers_Sizes);			/*empty the buffer*/
			/*Receive Again*/
			HAL_UARTEx_ReceiveToIdle_DMA(esp8266_huart,RxBuffer_Temp,ESP8266_Rx_Buffers_Sizes);
			return;
		}
		if(Connections_Packets_Counter[Connection_ID] == 0)
		{
			uint16_t z = 0;	/*counter for the output string*/
			/*Wait until full reception of RxBuffer_Temp*/
			while(RxBuffer_Temp[Received_Data_Size+i-1] == '\0');
			memcpy(RxBuffer_Connections[Connection_ID],RxBuffer_Temp+i,Received_Data_Size);
			/*Remove CLOSED sub string from the data*/
			if(PARSE_RESPONSE(RxBuffer_Connections[Connection_ID],"CLOSED\r") == RESPONSE_OK)
			{

				while((RxBuffer_Connections[0][i] != 'C') || (RxBuffer_Connections[0][i+1] != 'L') || (RxBuffer_Connections[0][i+2] != 'O')\
				|| (RxBuffer_Connections[0][i+3] != 'S') || (RxBuffer_Connections[0][i+4] != 'E') || (RxBuffer_Connections[0][i+5] != 'D'))
				{
					i++;
				}
				/*iterate until the end and remove every character from CLOSED*/
				while(RxBuffer_Connections[0][i])
				{
					RxBuffer_Connections[0][i] = '\0';
					i++;
				}
				//Received_Data_Size -=9; /*Line to be checked*/ /*remove the CLOSED characters size*/
			}
			RxBuffer_Connections_Data_Counter[Connection_ID] = Received_Data_Size;
		}
		else
		{
			uint16_t z = RxBuffer_Connections_Data_Counter[Connection_ID];	/*counter for the output string*/
			/****************test priority here in case of accumulated received data***********/
			/*Wait until full reception of RxBuffer_Temp*/
			while(RxBuffer_Temp[Received_Data_Size+i-1] == '\0');
			memcpy(RxBuffer_Connections[Connection_ID]+z,RxBuffer_Temp+i,Received_Data_Size);
			/*Remove CLOSED sub string from the data*/
			if(PARSE_RESPONSE(RxBuffer_Connections[Connection_ID],"CLOSED") == RESPONSE_OK)
			{
				while((RxBuffer_Connections[0][i] != 'C') || (RxBuffer_Connections[0][i+1] != 'L') || (RxBuffer_Connections[0][i+2] != 'O')\
				|| (RxBuffer_Connections[0][i+3] != 'S') || (RxBuffer_Connections[0][i+4] != 'E') || (RxBuffer_Connections[0][i+5] != 'D'))
				{
					i++;
				}
				/*iterate until the end and remove every character from CLOSED*/
				while(RxBuffer_Connections[0][i])
				{
					RxBuffer_Connections[0][i] = '\0';
					i++;
				}
				//Received_Data_Size -=9; /*Line to be checked*/ /*remove the CLOSED characters size*/
			}
			RxBuffer_Connections_Data_Counter[Connection_ID] += Received_Data_Size;
		}
		Connections_Packets_Counter[Connection_ID]++;
	}
	memset(RxBuffer_Temp,0,ESP8266_Rx_Buffers_Sizes);			/*empty the buffer*/
	HAL_UARTEx_ReceiveToIdle_DMA(esp8266_huart,RxBuffer_Temp,ESP8266_Rx_Buffers_Sizes);
	return;
}
/************************************************************************************************************************
* Function Name: ESP8266_Read_TCP_Packets
* Function ID[hex]: 0x18
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Data buffer to get the data,
* 				   Variable by address to get Data size,
* 				   Concerned TCP Connection ID
* Parameters (inout): NONE
* Parameters (out): Data buffer to get the data
* 					Variable by address to get Data size
* Return value: Response of execution
* Description: Function to read the recently received data related
* 			   to the given TCP connection ID
* 			   Be careful in timing while using this function
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Read_TCP_Packets(uint8_t* Data,uint32_t* Data_Size,uint8_t Connection_ID)
{
	/*Copy Data*/
	*Data_Size = RxBuffer_Connections_Data_Counter[Connection_ID];	/*Copy data size*/
	memcpy(Data,RxBuffer_Connections[Connection_ID],ESP8266_Rx_Buffers_Sizes);	/*Copy data from Rx buffer*/
	memset(RxBuffer_Connections[Connection_ID],0,ESP8266_Rx_Buffers_Sizes);	/*Clear the Rx Buffer of the concerned connection ID*/
	Connections_Packets_Counter[Connection_ID] = 0;	/*clearing number of packets received*/
	RxBuffer_Connections_Data_Counter[Connection_ID] = 0;	/*Clearing number of bytes received*/
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Stop_Receiving_TCP_Packets
* Function ID[hex]: 0x19
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of execution
* Description: Function to stop receiving any TCP packets and clear all Receiving buffers of all connections
* 			   Be careful in timing while using this function, it should not be used before ESP8266_Read_TCP_Packets function
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Stop_Receiving_TCP_Packets(void)
{
	if(READY_TO_RECEIVE == LOW)
	{
		return RESPONSE_OK;
	}
	/*Re-setting the Concerned Connection Parameters*/
	for(uint8_t i = 0 ; i<ESP8266_MAX_TCP_CONNECTIONS ; i++)
	{
		memset(RxBuffer_Connections[i],0,ESP8266_Rx_Buffers_Sizes);
		Connections_Packets_Counter[i] = 0;
		RxBuffer_Connections_Data_Counter[i] = 0;
	}
	HAL_StatusTypeDef HAL_Status = HAL_UART_Abort_IT(esp8266_huart);
	if(HAL_Status != HAL_OK)
	{
		return RESPONSE_NOT_OK;
	}
	READY_TO_RECEIVE = LOW;
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_SEND_COMMAND
* Function ID[hex]: 0x20
* Sync/Async: Synchronous/Asynchronous - depending on FreeRTOS usage
* Reentrancy: Non-Reentrant
* Parameters (in): command buffer
* 				   command size
* 				   response buffer
* 				   response size
* 				   command timeout duration in ms
* 				   command specialty whether it's establishing TCP connection
* 				   option whether to send only or send and wait to receive
* 				   Buffer to store the received data when using Sending and receiving option, if not using it, pass NULL
* Parameters (inout): NONE
* Parameters (out): Command Execution Response in Response Buffer
* 					Receive Buffer if send and receive option is used
* Return value: Response of execution
* Description: Private function used by other functions in this driver to send commands to ESP8266 module
* 			   Please, check Pre-build configurations file to decide some parameters regarding this essential function
************************************************************************************************************************/
static ESP8266_RESPONSE ESP8266_SEND_COMMAND(uint8_t* command,uint32_t command_size,uint8_t* response_buffer,\
		uint32_t response_buffer_size,uint32_t timeout,SPECIAL_COMMAND TCP_connection_command,SEND_AND_RECEIVE send_receive,uint8_t* Receive_Buffer)
{
	HAL_StatusTypeDef HAL_Status = 0;
	ESP8266_RESPONSE Response_Status = 0;
	uint8_t local_response_buffer[response_buffer_size];	/*local buffer to hold the response*/
	uint16_t stuck_in_loop = 0;	/*Variable to get out of while 1 if the ESP keeps resetting or not working*/
	uint8_t command_trials = 1;	/*Trials to retry executing the command*/
	uint8_t command_busy = 1;	/*used to slow down commands execution when busy p message is received*/
	memset(response_buffer,0,response_buffer_size);			/*empty the buffer  //&& (Trials_Number > 0)*/
	/*Before starting to send command and receive response, stop receiving through Interrupt*/
	if(READY_TO_RECEIVE == HIGH && TEMP_STOP_RECEIVE == LOW)
	{
		Response_Status = ESP8266_Stop_Receiving_TCP_Packets();
		if(Response_Status == RESPONSE_OK)
		{
			READY_TO_RECEIVE = HIGH;
			TEMP_STOP_RECEIVE = HIGH;
			Response_Status = 0;	/*reset response*/
		}
	}
	while((Response_Status != RESPONSE_OK) && (command_trials<COMMAND_TRIALS_NUMBER))
	{
		command_trials++;
		memset(local_response_buffer,0,response_buffer_size); 	/*empty the buffer*/
		HAL_Status = HAL_UART_Receive_DMA(esp8266_huart,local_response_buffer,response_buffer_size-1);
		if(HAL_Status != HAL_OK)
		{
			continue;
		}
		ESP8266_Delay_ms((ESP_TAKE_BREATH_ms*command_busy*COMMANDS_TIMEOUT_SAFETY_FACTOR)/(command_trials));
		HAL_Status = HAL_UART_Transmit(esp8266_huart,command,command_size+1,100);
		uint16_t inside_loop_breaker = 0;
		while(1)
		{
			ESP8266_Delay_ms((timeout*command_busy*COMMANDS_TIMEOUT_SAFETY_FACTOR)/(command_trials));
			/*corner case for sending TCP packets situation*/
			if(TCP_connection_command == YES)
			{
				if(PARSE_RESPONSE(local_response_buffer," ") != RESPONSE_OK)
				{
					inside_loop_breaker++;
					if(inside_loop_breaker<COMMAND_TRIALS_NUMBER)
					{
						break;
					}
					continue;
				}
			}
			/*ESP8266 restarts unintentionally - it may stuck in while loop here*/
			if((PARSE_RESPONSE(local_response_buffer,"restart") == RESPONSE_OK) ||\
				((PARSE_RESPONSE(local_response_buffer,"ets") == RESPONSE_OK) && (PARSE_RESPONSE(command,"RST") != RESPONSE_OK)))
			{
				Response_Status = RESPONSE_NOT_OK;
				memset(local_response_buffer,0,response_buffer_size); 	/*empty the buffer*/
				HAL_UART_AbortReceive(esp8266_huart);
				HAL_Status = HAL_UART_Receive_DMA(esp8266_huart,local_response_buffer,response_buffer_size-1);
				if(HAL_Status != HAL_OK)
				{
					inside_loop_breaker++;
					if(inside_loop_breaker<COMMAND_TRIALS_NUMBER)
					{
						break;
					}
					continue;
				}
				ESP8266_Delay_ms((ESP_TAKE_BREATH_ms*command_busy*COMMANDS_TIMEOUT_SAFETY_FACTOR)/(command_trials));
				HAL_Status = HAL_UART_Transmit(esp8266_huart,command,command_size+1,100);
			}
			/*Check OK Response*/
			else if(PARSE_RESPONSE(local_response_buffer,"OK") == RESPONSE_OK)
			{
				Response_Status = RESPONSE_OK;
				break;
			}
			/*Check ERROR Response*/
			else if(PARSE_RESPONSE(local_response_buffer,"ERROR") == RESPONSE_OK)
			{
				Response_Status = RESPONSE_NOT_OK;
				break;
			}
			/*ESP is busy doing the command, so wait*/
			else if((PARSE_RESPONSE(local_response_buffer,"busy p") == RESPONSE_OK)\
					|| (strlen(local_response_buffer) == 0))
			{
				command_busy++;
				Response_Status = RESPONSE_NOT_OK;
				if(command_busy > COMMAND_TRIALS_NUMBER*2)
				{
					command_busy = 1;
					break;
				}
				continue;
			}
			else
			{
				/*a stray character , so try again*/
				if(strlen(local_response_buffer)>0 || strlen(command)>0)
				{
					break;
				}
				/*Dealing with random reset issue*/
				memset(local_response_buffer,0,response_buffer_size); 	/*empty the buffer*/
				HAL_UART_AbortReceive(esp8266_huart);
				HAL_Status = HAL_UART_Receive_DMA(esp8266_huart,local_response_buffer,response_buffer_size-1);
				if(HAL_Status != HAL_OK)
				{
					continue;
				}
				ESP8266_Delay_ms((ESP_TAKE_BREATH_ms*command_busy*COMMANDS_TIMEOUT_SAFETY_FACTOR)/(command_trials));
				HAL_Status = HAL_UART_Transmit(esp8266_huart,command,command_size+1,100);
			}
			/*This section for dealing with while loop and can be changed further*/
			stuck_in_loop++;
			/*went through that loop 50 times, system is off*/
			/*LEDs off after it was on means error in ESP8266 and this function*/
			if(stuck_in_loop>50)
			{
				ESP8266_Initialized_Flag = LOW;
				ESP8266_Access_Point_Flag = LOW;
				ESP8266_WIFI_Connected_Flag = LOW;
				ESP8266_Internet_Connection_Flag = LOW;
				ESP8266_LED_INDICATOR(INITIALIZATION,ESP8266_Initialized_Flag);
				ESP8266_LED_INDICATOR(ACCESS_POINT,ESP8266_Access_Point_Flag);
				ESP8266_LED_INDICATOR(WIFI_CONNECT,ESP8266_WIFI_Connected_Flag);
				ESP8266_LED_INDICATOR(INTERNET_CONNECT,ESP8266_Internet_Connection_Flag);
				break;
			}
		}
		HAL_UART_AbortReceive(esp8266_huart);
		/*command waiting for something to receive*/
		if(send_receive == SEND_RECEIVE)
		{
			strcpy(Receive_Buffer,local_response_buffer);
		}
	}
	if(READY_TO_RECEIVE == HIGH && TEMP_STOP_RECEIVE == HIGH)
	{
		Response_Status = ESP8266_Start_Receive_TCP_Packets();
		if(Response_Status == RESPONSE_OK)
		{
			TEMP_STOP_RECEIVE = LOW;
		}
		else
		{
			Response_Status = RESPONSE_NOT_OK;
		}
	}
	strcpy(response_buffer,local_response_buffer);
	return Response_Status;
}
/************************************************************************************************************************
* Function Name: PARSE_RESPONSE
* Function ID[hex]: 0x21
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Response buffer,
* 				   Expected response (string that i am looking for)
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Result of parsing
* Description: Private function used by other functions in this driver to parse a certain string in the response
*			   RESPONSE_OK means string is found
*			   RESPONSE_NOT_OK means string is not found
************************************************************************************************************************/
static ESP8266_RESPONSE PARSE_RESPONSE(uint8_t* buffer,uint8_t* expected_response)
{
	ESP8266_RESPONSE Response_Status = 0;
	/*function from string.h library to find a substring in a string*/
	if(strstr(buffer,expected_response) != NULL)
	{
		Response_Status = RESPONSE_OK;
	}
	else
	{
		Response_Status = RESPONSE_NOT_OK;
	}
	return Response_Status;
}
/************************************************************************************************************************
* Function Name: ESP8266_LED_INDICATOR
* Function ID[hex]: 0x22
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): LED type to change its state to either ON of OFF
* 				   Its related flag, either ON or OFF
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Private function used by other functions in this driver to turn on or off the LEDs
* 			   to indicate a certain functionality, LEDs are connected in positive logic
************************************************************************************************************************/
static void ESP8266_LED_INDICATOR(ESP8266_LED_STATE LED_Type,ESP8266_INDICATOR_FLAG Flag)
{
	switch(LED_Type)
	{
		case INITIALIZATION:
			if(Flag == HIGH)
			{
				HAL_GPIO_WritePin(INITIALIZED_CONNECTED_LED_PORT,INITIALIZED_CONNECTED_LED_PIN,GPIO_PIN_SET);
			}
			else if(Flag == LOW)
			{
				HAL_GPIO_WritePin(INITIALIZED_CONNECTED_LED_PORT,INITIALIZED_CONNECTED_LED_PIN,GPIO_PIN_RESET);
			}
			break;
		case ACCESS_POINT:
			if(Flag == HIGH)
			{
				HAL_GPIO_WritePin(ACCESS_POINT_LED_PORT,ACCESS_POINT_LED_PIN,GPIO_PIN_SET);
			}
			else if(Flag == LOW)
			{
				HAL_GPIO_WritePin(ACCESS_POINT_LED_PORT,ACCESS_POINT_LED_PIN,GPIO_PIN_RESET);
			}
			break;
		case WIFI_CONNECT:
			if(Flag == HIGH)
			{
				HAL_GPIO_WritePin(CONNECTED_TO_WIFI_LED_PORT,CONNECTED_TO_WIFI_LED_PIN,GPIO_PIN_SET);
			}
			else if(Flag == LOW)
			{
				HAL_GPIO_WritePin(CONNECTED_TO_WIFI_LED_PORT,CONNECTED_TO_WIFI_LED_PIN,GPIO_PIN_RESET);
			}
			break;
		case INTERNET_CONNECT:
			if(Flag == HIGH)
			{
				HAL_GPIO_WritePin(INTERNET_CONNECTED_LED_PORT,INTERNET_CONNECTED_LED_PIN,GPIO_PIN_SET);
			}
			else if(Flag == LOW)
			{
				HAL_GPIO_WritePin(INTERNET_CONNECTED_LED_PORT,INTERNET_CONNECTED_LED_PIN,GPIO_PIN_RESET);
			}
			break;
		default:
			/*Do Nothing*/
			break;
	}
}
/************************************************************************************************************************
* Function Name: ESP8266_Disable_Echo
* Function ID[hex]: 0x23
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: ESP8266 Echo Status
* Description: Private function used by other functions in this driver to always disable echo before every command sent
* 			   It's notices that ESP8266 sometimes glitches and enables echo by itself which affects the device
* 			   performance and expected response
************************************************************************************************************************/
static ECHO_STATE ESP8266_Disable_Echo(void)
{
	ESP8266_RESPONSE Response_Status = 0;	/*Variable to check Response Status: 0x01 OK, 0x02 NOT OK*/
	uint8_t Command_Response_Buffer[BUFFER_4];		/*Buffer to always check AT responses*/
	uint8_t send_trials = 0;
	memset(Command_Response_Buffer,0,BUFFER_4);			/*empty the buffer*/
	Response_Status = ESP8266_SEND_COMMAND("ATE0\r\n",7,Command_Response_Buffer,BUFFER_4,TIMEOUT_0,NO,SEND_ONLY,NULL);
	if(Response_Status != RESPONSE_OK)
	{
		return ECHO_ON;
	}
	return ECHO_OFF;
}
/************************************************************************************************************************
* Function Name: ESP8266_Check_Inbox
* Function ID[hex]: 0x24
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): Connection ID to check if its inbox is empty or not
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of state
* Description: Function to check inbox of certain connection
* 			   RESPONSE_NOT_OK means inbox is empty
* 			   RESPONSE_OK means inbox is not empty
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Check_Inbox(uint8_t Connection_ID)
{
	if(Connections_Packets_Counter[Connection_ID] == 0)
	{
		return RESPONSE_NOT_OK;
	}
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Check_Inbox
* Function ID[hex]: 0x25
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of state
* Description: Function to check ESP8266 TCP server status
* 			   RESPONSE_NOT_OK means ESP8266 TCP server is off
* 			   RESPONSE_OK means ESP8266 TCP server is on
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Check_TCP_Server_Status(void)
{
	if(ESP8266_TCP_SERVER_FLAG == LOW)
	{
		return RESPONSE_NOT_OK;
	}
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_TCP_Reception_Status
* Function ID[hex]: 0x26
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of state
* Description: Function to check ESP8266 TCP Reception status
* 			   RESPONSE_NOT_OK means ESP8266 Not receiving TCP packets
* 			   RESPONSE_OK means ESP8266 is receiving TCP packets
************************************************************************************************************************/
ESP8266_RESPONSE ESP8266_Check_TCP_Reception_Status(void)
{
	if(READY_TO_RECEIVE == LOW)
	{
		return RESPONSE_NOT_OK;
	}
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: ESP8266_Delay_ms
* Function ID[hex]: 0x27
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of state
* Description: Function to do delay until command is executed and response is received
************************************************************************************************************************/
static void ESP8266_Delay_ms(uint32_t Delay_ms)
{
	uint32_t SystemClockFreq = HAL_RCC_GetHCLKFreq();
	/*17000 not 1000 because even using accurate calculations delay is performed more than expected*/
	uint32_t cycles_per_ms = SystemClockFreq / 16000; // cycles per millisecond
	uint32_t total_cycles = cycles_per_ms * Delay_ms;
	// Perform the delay using a for loop
	for (volatile uint32_t i = 0; i < total_cycles; ++i);
	return;
}
