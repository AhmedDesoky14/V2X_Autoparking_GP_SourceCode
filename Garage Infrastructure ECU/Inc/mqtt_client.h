/************************************************************************************************************************
 * 	Module: MQTT Client
 * 	File Name: mqtt_client.h
 *  Authors: Ahmed Desoky
 *	Date: 11/3/2024
 *	*********************************************************************************************************************
 *	Description: Module implemented based on Paho MQTT Packet C Library, which is used to serialize MQTT Packets
 *				 This module is based on STM HAL Drivers, ESP8266 Driver and Paho MQTT Packet C Library
 *				 which is a low level library used to serialize and deserialize MQTT Packets
 *				 It deals with MQTT Brokers and topics to publish and subscribe messages
 *				 This module by default supports only QoS=0 Transactions and Non-Retained messages
 *				 This module only support receiving messages in JSON Format
 *				 This module set the keep alive parameter to 6 minutes and every 5 minutes ping to the MQTT broker
 *				 This module is only working when TCP server of ESP8266 is off
 ***********************************************************************************************************************/
#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "stm32f4xx_hal.h"
#include "esp8266.h"
#include "mqtt_client_cfg.h"
#include "MQTTPacket.h"
#include <string.h>
#include <stdlib.h>
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.2.4*/
#define MQTT_CLIENT_SW_MAJOR_VERSION           (1U)
#define MQTT_CLIENT_SW_MINOR_VERSION           (2U)
#define MQTT_CLIENT_SW_PATCH_VERSION           (4U)
/*Software Version checking between Module Configuration file and Header file*/
#if ((MQTT_CLIENT_SW_MAJOR_VERSION != MQTT_CLIENT_CFG_SW_MAJOR_VERSION)\
 ||  (MQTT_CLIENT_SW_MINOR_VERSION != MQTT_CLIENT_CFG_SW_MINOR_VERSION)\
 ||  (MQTT_CLIENT_SW_PATCH_VERSION != MQTT_CLIENT_CFG_SW_PATCH_VERSION))
  #error "The Software version of MQTT Client does not match the configurations expected version"
#endif
/*Software Version checking of Dependent Module - ESP8266 Driver - Version 2.4.4*/
#if ((ESP8266_SW_MAJOR_VERSION != (2U))\
 ||  (ESP8266_SW_MINOR_VERSION != (4U))\
 ||  (ESP8266_SW_PATCH_VERSION != (4U)))
  #error "The Software version of ESP8266 Driver does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define MAX_SUBSCRIBE_TOPICS				5U		/*Max allowed subscription*/
#define MQTT_3_1_1							4U	/*MQTT Version used is 3.1.1*/
#define DEFAULT_QoS							0U	/*Quality of Service = 0*/
#define DEFAULT_RETAINED_FLAG				0U	/*Retained Flag = 0*/
#define NO_DUPLICATION						0U	/*Duplication Flag = 0*/
#define PING_PACKET_BUFFER					10U		/*Ping Packet Buffer Size*/
#define DISCONNECT_PACKET_BUFFER			10U		/*Disconnect Packet Buffer Size*/
#define KEEP_ALIVE_VALUE					360U	/*Keep Alive interval value in seconds*/
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
/*typdef enum used to express states of executing the module functions*/
typedef enum
{
	MQTT_OK = 1, MQTT_NOT_OK = 2, TRANSPORT_ERROR = 3
}MQTT_Transaction_State;
typedef enum
{
	CLIENT_DISCONNECTED = 0, CLIENT_CONNECTED = 1
}CLIENT_CONNECTION;
/*typedef struct to store Device/Client ID, User name and Password*/
typedef struct
{
	uint8_t* Client_ID;
	uint8_t* Client_Username;
	uint8_t* Client_Password;
}MQTT_CLIENT_CREDENTIALS;
/*typedef struct to store Broker IP address and Port*/
typedef struct
{
	uint8_t* Address;
	uint32_t Socket;
}MQTT_BROKER_ADDRESS;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
MQTT_Transaction_State MQTT_Publish(MQTT_BROKER_ADDRESS* Broker,MQTT_CLIENT_CREDENTIALS* Client,uint8_t* Topic,uint8_t* Payload,int32_t Payload_len);
MQTT_Transaction_State MQTT_Subscribe(MQTT_BROKER_ADDRESS* Broker,MQTT_CLIENT_CREDENTIALS* Client,uint8_t* Topics_string);
MQTT_Transaction_State MQTT_UnSubscribe(void);
MQTT_Transaction_State MQTT_Read(uint8_t* MQTT_Rx_Read);
void MQTT_Subscribe_Ping_Callback(void);
void MQTT_Subscribe_Receive_Callback(void);
MQTT_Transaction_State MQTT_Check_Inbox(void);
MQTT_Transaction_State MQTT_Check_Client_Connection(void);
/************************************************************************************************************************
 *                    							   External Variables
 ***********************************************************************************************************************/
/*extern timer4 to be used in MQTT Ping request*/
extern TIM_HandleTypeDef htim4;
#endif /*MQTT_CLIENT_H*/
