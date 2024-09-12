/************************************************************************************************************************
 * 	Module: esp8266
 * 	File Name: esp8266.h
 *  Authors: Ahmed Desoky
 *	Date: 14/2/2024
 *	*********************************************************************************************************************
 *	Description: ESP8266 Driver header file
 *				 ESP8266 Driver on STM32F103C8T6 Micro-controller.
 *				 Micro-controller Drivers are STM32 HAL Drivers.
 *				 This Driver is designed to support generally any application it's needed to be used in
 *				 You can skip looking in source file functions if you intend you use it only
 *				 This Driver uses the dedicated UART peripheral resources, and no other device or instance can share it
 ***********************************************************************************************************************/
#ifndef ESP8266_H
#define ESP8266_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "esp8266_cfg.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 2.4.4*/
#define ESP8266_SW_MAJOR_VERSION           (2U)
#define ESP8266_SW_MINOR_VERSION           (4U)
#define ESP8266_SW_PATCH_VERSION           (4U)
/*Software Version checking between Module Configuration file and Header file*/
#if ((ESP8266_SW_MAJOR_VERSION != ESP8266_CFG_SW_MAJOR_VERSION)\
 ||  (ESP8266_SW_MINOR_VERSION != ESP8266_CFG_SW_MINOR_VERSION)\
 ||  (ESP8266_SW_PATCH_VERSION != ESP8266_CFG_SW_PATCH_VERSION))
  #error "The Software version of ESP8266 does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define ESP8266_TCP_SERVER_DEFAULT_PORT			333	/*ESP8266 TCP Server default Port is 333 and it always must be used*/
#define ESP8266_TCP_SERVER_CONNECTION_TIMEOUT	180	/*default timeout for any TCP connection is 3 minutes, after that connection is closed*/
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
/*Data type used to indicate different responses of the function*/
typedef enum
{
	RESPONSE_OK = 0x01,
	RESPONSE_NOT_OK = 0x02,
	RESPONSE_BUSY = 0x03	/*When a resource is used*/
}ESP8266_RESPONSE;
/*Data type used to indicate state of different flags*/
typedef enum
{
	LOW = 0, HIGH = 1
}ESP8266_INDICATOR_FLAG;
/*Data type to indicate type of states to express when turning on and off each corresponding led
 *  Check ESP8266_LED_INDICATOR private function*/
typedef enum
{
	INITIALIZATION = 0,
	ACCESS_POINT = 1,
	WIFI_CONNECT = 2,
	INTERNET_CONNECT = 3
}ESP8266_LED_STATE;
/*Data type to express different types of sizes for different sizes of local buffers used in each function*/
typedef enum
{
	BUFFER_0=20,	BUFFER_1=40,
	BUFFER_2=100,	BUFFER_3=200,
	BUFFER_4=400,	BUFFER_5=500,
	BUFFER_6=1000,	BUFFER_7=2000,
	BUFFER_8=4000,	BUFFER_9=5000
}BUFFER_SIZE;
/*Data type to express different types of wait durations for different commands used in each function*/
typedef enum
{
	TIMEOUT_0 = 10,		TIMEOUT_1 = 50,
	TIMEOUT_2 = 100,	TIMEOUT_3 = 500,
	TIMEOUT_4 = 1000,	TIMEOUT_5 = 2000,
	TIMEOUT_6 = 5000,	TIMEOUT_7 = 10000
}TIMEOUT_PERIOD;
/*Data type used to choose the mode of ESP8266 initially*/
typedef enum
{
	STATION_MODE = 1,
	SOFT_AP_MODE = 2,
	AP_STATION_MODE = 3
}OPERATION_MODE;
/*Data type used to choose the connection of ESP8266 initially, either auto connected or not*/
typedef enum
{
	ESP8266_DISCONNECT = 0,ESP8266_CONNECT = 1
}WiFi_AUTO_CONNECT;
/*Data type used to express echo state of esp8266*/
typedef enum
{
	ECHO_OFF = 0,ECHO_ON = 1
}ECHO_STATE;
/*Data Type to differentiate between commands to apply HAL_Delay or not and
 * to differentiate between TCP connections commands and other*/
typedef enum
{
	NO = 0,YES = 1
}SPECIAL_COMMAND;
typedef enum
{
	SEND_ONLY = 0, SEND_RECEIVE = 1
}SEND_AND_RECEIVE;
/*Data type to hold ESP8266 initial configurations*/
typedef struct
{
	OPERATION_MODE Operation_Mode;
	WiFi_AUTO_CONNECT AP_Auto_Connect;
	uint8_t* Station_MAC_ADD;
	uint8_t* AP_MAC_ADD;
	uint8_t* Default_SSID;
	uint8_t* Default_PASSWD;
	uint8_t* AP_SSID;
	uint8_t* AP_PASSWD;
	uint8_t* AP_Static_IP;
	uint8_t Max_AP_Connections;
	uint8_t Channel_ID;
}ESP8266_INIT_CONFIG;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
ESP8266_RESPONSE ESP8266_Init(UART_HandleTypeDef* huartx,const ESP8266_INIT_CONFIG* esp8266_config);
ESP8266_RESPONSE ESP8266_Get_AP_IP_Address(uint8_t* AP_IP_Address);
ESP8266_RESPONSE ESP8266_Get_Station_IP_Address(uint8_t* Station_IP_Address);
ESP8266_RESPONSE ESP8266_Get_Connected_Device_Addresses(uint8_t* IP_Address,uint8_t* MAC_Address,uint8_t Device_ID);
ESP8266_RESPONSE ESP8266_Get_Connected_Devices_Number(uint8_t* count);
ESP8266_RESPONSE ESP8266_Check_Internet_Connection(void);
ESP8266_RESPONSE ESP8266_WiFi_Disconnect(void);
ESP8266_RESPONSE ESP8266_WiFi_Connect(uint8_t* SSID,uint8_t* Password);
ESP8266_RESPONSE ESP8266_Establish_TCP_Server(void);
ESP8266_RESPONSE ESP8266_Disband_TCP_Server(void);
ESP8266_RESPONSE ESP8266_Establish_TCP_Connection(uint8_t* IP_Address,uint16_t Socket_Port);
ESP8266_RESPONSE ESP8266_Close_TCP_Connection(uint8_t Connection_ID);
ESP8266_RESPONSE ESP8266_Check_TCP_Connections_Number(uint8_t* Connections_Number);
ESP8266_RESPONSE ESP8266_Send_TCP_Packets(uint8_t* Data,uint32_t Data_size,uint8_t Connection_ID,SEND_AND_RECEIVE send_receive,uint8_t* Receive_Buffer);
ESP8266_RESPONSE ESP8266_Start_Receive_TCP_Packets(void);
void ESP8266_Receive_TCP_Packets_Callback(uint8_t* Device_Connected);
ESP8266_RESPONSE ESP8266_Read_TCP_Packets(uint8_t* Data,uint32_t* Data_Size,uint8_t Connection_ID);
ESP8266_RESPONSE ESP8266_Stop_Receiving_TCP_Packets(void);
ESP8266_RESPONSE ESP8266_Check_Inbox(uint8_t Connection_ID);
ESP8266_RESPONSE ESP8266_Check_TCP_Server_Status(void);
ESP8266_RESPONSE ESP8266_Check_TCP_Reception_Status(void);
/************************************************************************************************************************
 *                    							   External Variables
 ***********************************************************************************************************************/
extern const ESP8266_INIT_CONFIG ESP8266_Configuration;	/*Initialization Object for ESP8266*/
#endif /*ESP8266_H*/
