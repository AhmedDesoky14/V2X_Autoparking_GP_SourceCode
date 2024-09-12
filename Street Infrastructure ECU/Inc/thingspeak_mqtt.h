/************************************************************************************************************************
 * 	Module: Thingspeak MQTT
 * 	File Name: thingspeak_mqtt.h
 *  Authors: Ahmed Desoky
 *	Date: 13/3/2024
 *	*********************************************************************************************************************
 *	Description: Thingspeak MQTT Module, is a specified module for Thingspeak Mathworks Platform using MQTT Protocol.
 *				 It's a free platform for IoT devices database.
 *				 This module deals with data only in JSON Format.
 *				 This module is fully dependent on MQTT Client Module and cJSON Library.
 *				 Any change in Transport driver, like ESP8266 or Ethernet affects MQTT Client module, but doesn't
 *				 affect this one.
 *				 This module supports only 5 topics subscriptions
 *				 Unfortunately, although Thingspeak is free, it doesn't support retained published messages, so to get
 *				 the latest update from the channel once start listening and subscribing to the channel
 *				 HTTP Get is used as a work around to get the latest updates, for the first time ONLY, as HTTP is also
 *				 introduced in Thingspeak.
 *				 This work around violates abstraction of this module from the transport driver, as the work around
 *				 piece of code uses transport driver function directly.
 *				 Please Refer to the first line of CONSTANT DEFINITIONS section to trigger the work around.
 ***********************************************************************************************************************/
#ifndef THINGSPEAK_MQTT_H
#define THINGSPEAK_MQTT_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "mqtt_client.h"
#include "ESP_Mode_Switch.h"
#include "cJSON.h"
#include <string.h>
#include <stdlib.h>
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.1.2*/
#define THINGSPEAK_MQTT_SW_MAJOR_VERSION           (1U)
#define THINGSPEAK_MQTT_SW_MINOR_VERSION           (1U)
#define THINGSPEAK_MQTT_SW_PATCH_VERSION           (2U)
/*Software Version checking of Dependent Module - MQTT Client Module - Version 1.2.4*/
#if ((MQTT_CLIENT_SW_MAJOR_VERSION != (1U))\
 ||  (MQTT_CLIENT_SW_MINOR_VERSION != (2U))\
 ||  (MQTT_CLIENT_SW_PATCH_VERSION != (4U)))
  #error "The Software version of MQTT Client Module does not match the expected version"
#endif
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
/*Configuration lines for HTTP work around trigger*/
#define Work_around_Trigger				1	/*The only configuration in this module - A Trigger*/
#if(Work_around_Trigger == 1)
#define HTTP_Tx_Buffer_Size				100U
#define HTTP_Rx_Buffer_Size				700U
#define HTTP_Channel_Feeds_Size			400U
#define THINGSPEAK_HTTP_SERVER_ADDRESS	((uint8_t*)("34.198.44.32"))	/*IP for: api.thingspeak.com*/
#define THINGSPEAK_HTTP_SERVER_PORT		80
#endif
#define THINGSPEAK_MQTT_BROKER_ADDRESS	((uint8_t*)("54.81.146.55"))/*IP for: mqtt3.thingspeak.com*/
#define THINGSPEAK_MQTT_BROKER_PORT		1883
#define PUBLISH_TOPIC_LENGTH			350U
#define SUBSCRIBE_TOPIC_LENGTH			350U
#define PAYLOAD_LENGTH					150U
#define MAX_CHANNELS_LISTEN				5U
#define FIELD_SIZE						40U
#define CHANNEL_STRING_SIZE				15U
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
/*typedef structure used to store Channels Feeds*/
typedef struct
{
	uint32_t Channel_ID;
	uint8_t field1[FIELD_SIZE];
	uint8_t field2[FIELD_SIZE];
	uint8_t field3[FIELD_SIZE];
	uint8_t field4[FIELD_SIZE];
	uint8_t field5[FIELD_SIZE];
	uint8_t field6[FIELD_SIZE];
	uint8_t field7[FIELD_SIZE];
	uint8_t field8[FIELD_SIZE];
#if(Work_around_Trigger == 1)
	uint8_t Read_API_Key[FIELD_SIZE];
#endif
}Thingspeak_Channel;
/*Typdef enumeration to clarify different responses of Thingspeak module*/
typedef enum
{
	THINGSPEAK_OK = 1, THINGSPEAK_NOT_OK = 2, THINGSPEAK_MQTT_ERROR = 3, THINGSPEAK_TRANSPORT_ERROR = 4
}THINGSPEAK_STATE;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
THINGSPEAK_STATE Thingspeak_Update_Channel(Thingspeak_Channel* Channel,MQTT_CLIENT_CREDENTIALS* Client);
THINGSPEAK_STATE Thingspeak_Start_Listen_Channels(Thingspeak_Channel* Channels,uint8_t Channels_Num,MQTT_CLIENT_CREDENTIALS* Client);
THINGSPEAK_STATE Thingspeak_Stop_Listen_Channels(void);
THINGSPEAK_STATE Thingspeak_Read_Message(Thingspeak_Channel* Channels);
void Thingspeak_Receive_Callback(void);
THINGSPEAK_STATE Thingspeak_Check_Inbox(uint32_t Channel_ID);
THINGSPEAK_STATE Thingspeak_Check_Connection(void);
#endif /*THINGSPEAK_MQTT_H*/
