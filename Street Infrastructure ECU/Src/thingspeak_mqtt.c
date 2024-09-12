/************************************************************************************************************************
 * 	Module: Thingspeak MQTT
 * 	File Name: thingspeak_mqtt.c
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
/***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "thingspeak_mqtt.h"
/***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
#if(Work_around_Trigger == 1)
static uint16_t New_Subscriber = 1;	/*Flag to indicate that I'm new subscriber or not*/
#endif
static uint8_t Thingspeak_Rx_Index = 0;	/*used for iteration for ESP8266 Read Packets*/
/*Array of buffers to store data from different connections, same sizes as MQTT Client*/
static uint8_t Thingspeak_Rx[MQTT_Rx_Last_Messages_Number][MQTT_Rx_Buffer_Size]={0};
/***********************************************************************************************************************
 *                     					        FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: Thingspeak_Update_Channel
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Channel Structure from which the fields will be published to the broker
* 				   Client Credentials to connect to the broker
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value:	Response of execution
* Description: Function to Update a specified channel, mainly the device's channel.
* 			   It updates all the fields of the channel.
************************************************************************************************************************/
THINGSPEAK_STATE Thingspeak_Update_Channel(Thingspeak_Channel* Channel,MQTT_CLIENT_CREDENTIALS* Client)
{
	MQTT_Transaction_State MQTT_Response = 0;
	uint8_t Channel_ID_string[10] = {0};
	uint8_t Topic[PUBLISH_TOPIC_LENGTH] = "channels/";
	uint8_t Payload[PAYLOAD_LENGTH] = {0};
	MQTT_BROKER_ADDRESS Thingspeak_Broker = {THINGSPEAK_MQTT_BROKER_ADDRESS,THINGSPEAK_MQTT_BROKER_PORT};
	utoa(Channel->Channel_ID,Channel_ID_string,10);
	strcat(Topic,Channel_ID_string);
	strcat(Topic,"/publish");
	sprintf(Payload,"field1=%s&field2=%s&field3=%s"
			"&field4=%s&field5=%s&field6=%s"
			"&field7=%s&field8=%s",\
			Channel->field1,Channel->field2,Channel->field3,\
			Channel->field4,Channel->field5,Channel->field6,\
			Channel->field7,Channel->field8); //,Channel->status);
	MQTT_Response = MQTT_Publish(&Thingspeak_Broker,Client,Topic,Payload,strlen(Payload));
	if(MQTT_Response == MQTT_NOT_OK)
	{
		return THINGSPEAK_MQTT_ERROR;
	}
	else if(MQTT_Response == TRANSPORT_ERROR)
	{
		return THINGSPEAK_TRANSPORT_ERROR;
	}
	return THINGSPEAK_OK;
}
/************************************************************************************************************************
* Function Name: Thingspeak_Start_Listen_Channels
* Function ID[hex]: 0x01
* Sync/Async: Asynchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Channels array of Structures in which the fields will be filled from the subscribed channels
* 				   Number of channels to subscribe to and start listening
* 				   Client Credentials to connect to the broker
* Parameters (inout): Channels array of Structures in which the fields will be filled from the subscribed channels
* Parameters (out): NONE
* Return value: Response of execution
* Description: Function to start listening to specific channels and subscribe to them
* 			   This function allows using HTTP Get for one time ONLY to get the latest updates of the channels passed
* 			   and store their updates in the Channels array of structures.
************************************************************************************************************************/
THINGSPEAK_STATE Thingspeak_Start_Listen_Channels(Thingspeak_Channel* Channels,uint8_t Channels_Num,MQTT_CLIENT_CREDENTIALS* Client)
{
	MQTT_Transaction_State MQTT_Response = 0;
	THINGSPEAK_STATE Thingspeak_Response = 0;
	uint8_t Channels_ID_string[MAX_CHANNELS_LISTEN][CHANNEL_STRING_SIZE] = {0};
	MQTT_BROKER_ADDRESS Thingspeak_Broker = {THINGSPEAK_MQTT_BROKER_ADDRESS,THINGSPEAK_MQTT_BROKER_PORT};
	uint8_t Topics[MAX_CHANNELS_LISTEN][PUBLISH_TOPIC_LENGTH] = {"channels/","channels/"
																,"channels/","channels/","channels/"};
	uint8_t Topics_string[MAX_CHANNELS_LISTEN*PUBLISH_TOPIC_LENGTH] = {0};
	/*Check MQTT Client Status*/
#if(Work_around_Trigger == 1)
	if(Thingspeak_Check_Connection() == THINGSPEAK_NOT_OK)
	{
		New_Subscriber = 1;
	}
	else if(Thingspeak_Check_Connection() == THINGSPEAK_OK)
	{
		New_Subscriber = 0;
	}
	/*Start to subscribe, use HTTP API only one time to get the latest update, as thingspeak doesn't support retained published messages*/
	if(New_Subscriber == 1)
	{
		uint8_t Connections_Num = 0;
		uint8_t HTTP_Get[HTTP_Tx_Buffer_Size] = {0};
		uint8_t HTTP_Channels_ID_string[MAX_CHANNELS_LISTEN][CHANNEL_STRING_SIZE] = {0};
		uint8_t HTTP_Rx_Buffer[HTTP_Rx_Buffer_Size] = {0};
		uint8_t Channel_Feeds[HTTP_Channel_Feeds_Size] = {0};
		cJSON* Message_JSON = cJSON_CreateObject();
		cJSON* Temp_JSON = cJSON_CreateObject();
		for(uint8_t z=0 ; z<Channels_Num ; z++)
		{
			/*Clear the buffers*/
			memset(HTTP_Get,0,HTTP_Tx_Buffer_Size);/*empty the buffer*/
			memset(HTTP_Channels_ID_string[z],0,CHANNEL_STRING_SIZE);/*empty the buffer*/
			memset(HTTP_Rx_Buffer,0,HTTP_Rx_Buffer_Size);/*empty the buffer*/
			memset(Channel_Feeds,0,HTTP_Channel_Feeds_Size);/*empty the buffer*/
			memset(Channels[z].field1,0,FIELD_SIZE);
			memset(Channels[z].field2,0,FIELD_SIZE);
			memset(Channels[z].field3,0,FIELD_SIZE);
			memset(Channels[z].field4,0,FIELD_SIZE);
			memset(Channels[z].field5,0,FIELD_SIZE);
			memset(Channels[z].field6,0,FIELD_SIZE);
			memset(Channels[z].field7,0,FIELD_SIZE);
			memset(Channels[z].field8,0,FIELD_SIZE);
			utoa(Channels[z].Channel_ID,HTTP_Channels_ID_string[z],10);
			sprintf(HTTP_Get,"GET https://api.thingspeak.com/channels/%s/feeds.json?api_key=%s&results=1\r\n"
					,HTTP_Channels_ID_string[z],Channels[z].Read_API_Key);
			if(ESP8266_Establish_TCP_Connection(THINGSPEAK_HTTP_SERVER_ADDRESS,THINGSPEAK_HTTP_SERVER_PORT) != RESPONSE_OK)
			{
				return THINGSPEAK_TRANSPORT_ERROR;
			}
			if(ESP8266_Send_TCP_Packets(HTTP_Get,strlen(HTTP_Get),0,SEND_RECEIVE,HTTP_Rx_Buffer) != RESPONSE_OK)
			{
				return THINGSPEAK_TRANSPORT_ERROR;
			}
//			if(ESP8266_Check_TCP_Connections_Number(&Connections_Num) != RESPONSE_OK)
//			{
//				return THINGSPEAK_TRANSPORT_ERROR;
//			}
			ESP8266_Close_TCP_Connection(0);	/*Close the connection*/
			/*Removing unnecessary characters and obtain JSON format */
			uint16_t j,h=0,HTTP_Rx_size;
			if(strstr(HTTP_Rx_Buffer,"feeds") != NULL)
			{
				for(j=0;!((HTTP_Rx_Buffer[j]=='f')&&(HTTP_Rx_Buffer[j+1]=='e')&&(HTTP_Rx_Buffer[j+2]=='e')\
						&&(HTTP_Rx_Buffer[j+3]=='d')&&(HTTP_Rx_Buffer[j+4]=='s'));j++);
				sprintf(HTTP_Rx_Buffer,"%s",HTTP_Rx_Buffer+j+8);	/*to skip all characters until the '{' character*/
				HTTP_Rx_size = strlen(HTTP_Rx_Buffer);
				while(HTTP_Rx_Buffer[h] != '}' && h<HTTP_Rx_size)
				{
					Channel_Feeds[h] = HTTP_Rx_Buffer[h];
					h++;
				}
				/*add the last character '}'*/
				Channel_Feeds[h] = HTTP_Rx_Buffer[h];
				/*check if the format is correct or not*/
				if(Channel_Feeds[strlen(Channel_Feeds)-1] != '}')
				{
					/*to make sure the format is obtained correct*/
					z--;
					continue;
				}
			}
			else
			{
				z--;
				continue;
			}
			/*Start to parse and extract from JSON*/
			Message_JSON = cJSON_Parse(Channel_Feeds);
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field1");
			strcpy(Channels[z].field1,Temp_JSON->valuestring);
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field2");
			strcpy(Channels[z].field2,Temp_JSON->valuestring);
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field3");
			strcpy(Channels[z].field3,Temp_JSON->valuestring);
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field4");
			strcpy(Channels[z].field4,Temp_JSON->valuestring);
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field5");
			strcpy(Channels[z].field5,Temp_JSON->valuestring);
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field6");
			strcpy(Channels[z].field6,Temp_JSON->valuestring);
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field7");
			strcpy(Channels[z].field7,Temp_JSON->valuestring);
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field8");
			strcpy(Channels[z].field8,Temp_JSON->valuestring);
		}
	    cJSON_Delete(Temp_JSON);
	    cJSON_Delete(Message_JSON);
	}
#endif
	for(uint8_t i=0 ; i<Channels_Num ; i++)
	{
		utoa(Channels[i].Channel_ID,Channels_ID_string[i],10);
		strcat(Topics[i],Channels_ID_string[i]);
		strcat(Topics[i],"/subscribe");
		strcat(Topics_string,Topics[i]);
		if(i<Channels_Num-1)
		{
			strcat(Topics_string,",");
		}
	}
	MQTT_Response = MQTT_Subscribe(&Thingspeak_Broker,Client,Topics_string);
	/*Check return*/
	switch(MQTT_Response)
	{
		case TRANSPORT_ERROR:
			Thingspeak_Response = THINGSPEAK_TRANSPORT_ERROR;
			break;
		case MQTT_NOT_OK:
			Thingspeak_Response = THINGSPEAK_MQTT_ERROR;
			break;
		case MQTT_OK:
			Thingspeak_Response = THINGSPEAK_OK;
#if(Work_around_Trigger == 1)
			New_Subscriber = 0;	/*not a new subscriber anymore*/
#endif
			break;
	}
	return Thingspeak_Response;
}
/************************************************************************************************************************
* Function Name: Thingspeak_Stop_Listen_Channels
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of execution
* Description: Function to stop listening and unsubscribe from all channels
************************************************************************************************************************/
THINGSPEAK_STATE Thingspeak_Stop_Listen_Channels(void)
{
	MQTT_Transaction_State MQTT_Response = 0;
	THINGSPEAK_STATE Thingspeak_Response = 0;
	MQTT_Response = MQTT_UnSubscribe();
	switch(MQTT_Response)
	{
		case TRANSPORT_ERROR:
			Thingspeak_Response = THINGSPEAK_TRANSPORT_ERROR;
			break;
		case MQTT_NOT_OK:
			Thingspeak_Response = THINGSPEAK_MQTT_ERROR;
			break;
		case MQTT_OK:
			Thingspeak_Response = THINGSPEAK_OK;
			break;
	}
	for(uint8_t i=0;i<MQTT_Rx_Last_Messages_Number;i++)
	{
		memset(Thingspeak_Rx[i],0,MQTT_Rx_Buffer_Size);	/*Clear all buffers*/
	}
	Thingspeak_Rx_Index = 0;	/*Reset Index*/
#if(Work_around_Trigger == 1)
	New_Subscriber = 1;	/*next time it will become a new subscriber*/
#endif
	return Thingspeak_Response;
}
/************************************************************************************************************************
* Function Name: Thingspeak_Read_Message
* Function ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Channel Structure to store fields into it
* Parameters (inout): Channel Structure to store fields into it
* Parameters (out): NONE
* Return value: Response of execution
* Description: Function to read from a specific channel by passing a specific channel
* 			   to it and search if its feeds are received or not
* 			   if received it stores its feeds into the structure.
************************************************************************************************************************/
THINGSPEAK_STATE Thingspeak_Read_Message(Thingspeak_Channel* Channels)
{
	uint8_t Channel_found_flag = 0;
	cJSON* Message_JSON = cJSON_CreateObject();
	cJSON* Temp_JSON = cJSON_CreateObject();
	for(uint8_t j=0 ; j<MQTT_Rx_Last_Messages_Number ; j++)
	{
		Message_JSON = cJSON_Parse(Thingspeak_Rx[j]);
		Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"channel_id");
		if(Channels->Channel_ID == Temp_JSON->valueint)
		{
			/*given channel found, copy data from the channel*/
			Channel_found_flag = 1;
			memset(Channels->field1,0,FIELD_SIZE);/*empty the buffer*/
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field1");
			if(strlen(Temp_JSON->valuestring)!=0)
			{
				strcpy(Channels->field1,Temp_JSON->valuestring);
			}
			memset(Channels->field2,0,FIELD_SIZE);/*empty the buffer*/
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field2");
			if(strlen(Temp_JSON->valuestring)!=0)
			{
				strcpy(Channels->field2,Temp_JSON->valuestring);
			}
			memset(Channels->field3,0,FIELD_SIZE);/*empty the buffer*/
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field3");
			if(strlen(Temp_JSON->valuestring)!=0)
			{
				strcpy(Channels->field3,Temp_JSON->valuestring);
			}
			memset(Channels->field4,0,FIELD_SIZE);/*empty the buffer*/
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field4");
			if(strlen(Temp_JSON->valuestring)!=0)
			{
				strcpy(Channels->field4,Temp_JSON->valuestring);
			}
			memset(Channels->field5,0,FIELD_SIZE);/*empty the buffer*/
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field5");
			uint16_t len = strlen(Temp_JSON->valuestring);
			if(strlen(Temp_JSON->valuestring)!=0)
			{
				strcpy(Channels->field5,Temp_JSON->valuestring);
			}
			memset(Channels->field6,0,FIELD_SIZE);/*empty the buffer*/
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field6");
			if(strlen(Temp_JSON->valuestring)!=0)
			{
				strcpy(Channels->field6,Temp_JSON->valuestring);
			}
			memset(Channels->field7,0,FIELD_SIZE);/*empty the buffer*/
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field7");
			if(strlen(Temp_JSON->valuestring)!=0)
			{
				strcpy(Channels->field7,Temp_JSON->valuestring);
			}
			memset(Channels->field8,0,FIELD_SIZE);/*empty the buffer*/
			Temp_JSON = cJSON_GetObjectItemCaseSensitive(Message_JSON,"field8");
			if(strlen(Temp_JSON->valuestring)!=0)
			{
				strcpy(Channels->field8,Temp_JSON->valuestring);
			}
			memset(Thingspeak_Rx[j],0,MQTT_Rx_Buffer_Size);/*empty the buffer after reading*/
			break;
		}
	}
	/*Channel not found*/
	if(Channel_found_flag != 1)
	{
		return THINGSPEAK_NOT_OK;
	}
    cJSON_Delete(Temp_JSON);
    cJSON_Delete(Message_JSON);
	return THINGSPEAK_OK;
}
/************************************************************************************************************************
* Function Name: Thingspeak_Receive_Callback
* Function ID[hex]: 0x04
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Callback function to store the received JSON objects into the Rx Buffers to read and get from them
* 			   channels feeds later.
* 			   This callback function is added inside HAL_UARTEx_RxEventCallback function as following
*					if (huart->Instance == USART1,2,3,4,5)
*					{
*						Thingspeak_Receive_Callback();
*					}
************************************************************************************************************************/
void Thingspeak_Receive_Callback(void)
{
	if(Get_ESP_Operation_Mode() != Network_Database_Communications)	/*If not in Database mode*/
	{
		return;
	}
	memset(Thingspeak_Rx[Thingspeak_Rx_Index],0,MQTT_Rx_Buffer_Size);/*empty the buffer*/
	if(MQTT_Check_Inbox() != MQTT_OK)
	{
		return;
	}
	MQTT_Read(Thingspeak_Rx[Thingspeak_Rx_Index]);
	if(strlen(Thingspeak_Rx[Thingspeak_Rx_Index]) > 0)
	{
		Thingspeak_Rx_Index++;
	}
	if(Thingspeak_Rx_Index == MQTT_Rx_Last_Messages_Number)
	{
		Thingspeak_Rx_Index = 0;	/*reset the index*/
	}
	return;
}
/************************************************************************************************************************
* Function Name: Thingspeak_Check_Inbox
* Function ID[hex]: 0x05
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Channel ID to check its reception in the inbox
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of execution
* Description: Function to check Inbox for a specific channel using the channel ID
* 			   THINGSPEAK_OK means it's found, THINGSPEAK_NOT_OK means it's not found
************************************************************************************************************************/
THINGSPEAK_STATE Thingspeak_Check_Inbox(uint32_t Channel_ID)
{
	uint8_t Channels_ID_string[CHANNEL_STRING_SIZE];
	for(uint8_t k=0 ; k<MQTT_Rx_Last_Messages_Number ; k++)
	{
		if(strlen(Thingspeak_Rx[k]) > 0)
		{
			memset(Channels_ID_string,0,CHANNEL_STRING_SIZE);
			utoa(Channel_ID,Channels_ID_string,10);
			if(strstr(Thingspeak_Rx[k],Channels_ID_string) != NULL)
			{
				return THINGSPEAK_OK;
			}
		}
	}
	return THINGSPEAK_NOT_OK;
}
/************************************************************************************************************************
* Function Name: Thingspeak_Check_Connection
* Function ID[hex]: 0x06
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Response of execution
* Description: Function to get the MQTT Client-Broker Connection Status
* 			   THINGSPEAK_OK means it's connected, THINGSPEAK_NOT_OK means it's not connected
************************************************************************************************************************/
THINGSPEAK_STATE Thingspeak_Check_Connection(void)
{
	if(MQTT_Check_Client_Connection() == MQTT_NOT_OK)
	{
		return THINGSPEAK_NOT_OK;
	}
	return THINGSPEAK_OK;
}
