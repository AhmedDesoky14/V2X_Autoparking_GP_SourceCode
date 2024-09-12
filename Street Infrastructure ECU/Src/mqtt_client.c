/************************************************************************************************************************
 * 	Module: MQTT Client
 * 	File Name: mqtt_client.c
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
/***********************************************************************************************************************
*                     							      INCLUDES
***********************************************************************************************************************/
#include "mqtt_client.h"
/***********************************************************************************************************************
*                     					           GLOBAL VARIABLES
***********************************************************************************************************************/
static CLIENT_CONNECTION Client_State = CLIENT_DISCONNECTED;	/*Client/Device Status regarding connection to a broker*/
static uint16_t Publish_Packet_ID = 0;	/*Subscribe Packet ID counter*/
static uint16_t Subscribe_Packet_ID = 0;	/*Subscribe Packet ID counter*/
static uint8_t MQTT_Get_Message_Index = 0;	/*User for iteration for read messages*/
static uint8_t MQTT_Rx_Index = 0;	/*used for iteration for ESP8266 Read Packets*/
static uint8_t MQTT_Rx[MQTT_Rx_Last_Messages_Number][MQTT_Rx_Buffer_Size]={0};/*Array of buffers to store data from different connections*/
/***********************************************************************************************************************
*                     					        FUNCTIONS DEFINTITIONS
***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: MQTT_Publish
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Broker structure object, contains IP Address and Port
* 				   Client structure object, contains ID, User name and Password
* 				   Topic to publish into
* 				   Pay load
* 				   Pay load length
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value:	Execution response
* Description: Function to publish a pay load into a specific topic, using QoS=0 (Fire & Forget) and messages are not retained
************************************************************************************************************************/
MQTT_Transaction_State MQTT_Publish(MQTT_BROKER_ADDRESS* Broker,MQTT_CLIENT_CREDENTIALS* Client,uint8_t* Topic,uint8_t* Payload,int32_t Payload_len)
{
	if(ESP8266_Check_TCP_Server_Status() == RESPONSE_OK)
	{
		return MQTT_NOT_OK;
	}
	ESP8266_RESPONSE ESP_Response = 0;
	/*Initially, If there's no Internet connection, exit*/
//	ESP_Response = ESP8266_Check_Internet_Connection();
//	if(ESP_Response != RESPONSE_OK)
//	{
//		return TRANSPORT_ERROR;
//	}
	MQTTPacket_connectData MQTT_Packet = MQTTPacket_connectData_initializer;/*Create MQTT Packet*/
	MQTTString Publish_Topic = MQTTString_initializer;
	uint8_t Duplication_Flag = NO_DUPLICATION;
	int32_t Packet_len = 0;
	int32_t QoS = DEFAULT_QoS;
	uint8_t Send_Buffer[SEND_BUFFER_SIZE] = {0};
	MQTT_Packet.MQTTVersion = MQTT_3_1_1;	/*MQTT Version 3.1.1*/
	MQTT_Packet.clientID.cstring = Client->Client_ID;
	if((Client->Client_Username != NULL) && (Client->Client_Password != NULL))
	{
		MQTT_Packet.username.cstring = Client->Client_Username;
		MQTT_Packet.password.cstring = Client->Client_Password;
	}
	MQTT_Packet.keepAliveInterval = KEEP_ALIVE_VALUE;
	MQTT_Packet.cleansession = 1;
	Publish_Topic.cstring = Topic;
	/*Serialize Connect Packet*/
	Packet_len = MQTTSerialize_connect((uint8_t*)(Send_Buffer),SEND_BUFFER_SIZE,&MQTT_Packet);
	/*Serialize Publish Packet and add to the buffer*/
	Packet_len += MQTTSerialize_publish((uint8_t*)(Send_Buffer+Packet_len),SEND_BUFFER_SIZE-Packet_len,\
			Duplication_Flag,QoS,DEFAULT_RETAINED_FLAG,Publish_Packet_ID,Publish_Topic,(uint8_t*)(Payload),Payload_len);
	/*Serialize Disconnect Packet and add to the buffer*/
	Packet_len += MQTTSerialize_disconnect((uint8_t*)(Send_Buffer+Packet_len),SEND_BUFFER_SIZE-Packet_len);
	if(Packet_len == 0)
	{
		return MQTT_NOT_OK;
	}
	/*adding carriage return to the end of the packet and increasing its size by 1*/
	Send_Buffer[Packet_len-1]='\x0d';
	Packet_len++;
	/*Establish TCP connection with the Broker*/
	ESP_Response = ESP8266_Establish_TCP_Connection(Broker->Address,Broker->Socket);
	if(ESP_Response != RESPONSE_OK)
	{
		return TRANSPORT_ERROR;
	}
	ESP_Response = ESP8266_Send_TCP_Packets(Send_Buffer,Packet_len,0,SEND_ONLY,NULL);
	if(ESP_Response != RESPONSE_OK)
	{
		return TRANSPORT_ERROR;
	}
	ESP8266_Close_TCP_Connection(0);
	Publish_Packet_ID++;
	return MQTT_OK;
}
/************************************************************************************************************************
* Function Name: MQTT_Subscribe
* Function ID[hex]: 0x01
* Sync/Async: Asynchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Broker structure object, contains IP Address and Port
* 				   Client structure object, contains ID, User name and Password
* 				   Topics string, String contains all topics, each are seperated by ',' characters
* 				   example: "channels/2435472/subscribe,channels/2439697/subscribe"
* Parameters (inout):NONE
* Parameters (out): NONE
* Return value: Execution response
* Description: Function to subscribe to specific number of topics, with maximum limit that can be configured
* 			   Then messages are obtained using MQTT_Read function
************************************************************************************************************************/
MQTT_Transaction_State MQTT_Subscribe(MQTT_BROKER_ADDRESS* Broker,MQTT_CLIENT_CREDENTIALS* Client,uint8_t* Topics_string)
{
	if(ESP8266_Check_TCP_Server_Status() == RESPONSE_OK)
	{
		return MQTT_NOT_OK;
	}
	ESP8266_RESPONSE ESP_Response = 0;
	/*Initially, If there's no Internet connection, exit*/
//	ESP_Response = ESP8266_Check_Internet_Connection();
//	if(ESP_Response != RESPONSE_OK)
//	{
//		Client_State = CLIENT_DISCONNECTED;
//		return TRANSPORT_ERROR;
//	}
	MQTTPacket_connectData MQTT_Packet = MQTTPacket_connectData_initializer;/*Create MQTT Packet*/
	uint8_t Topics_Holder[MAX_SUBSCRIBE_TOPICS][100] = {0};
	MQTTString Subscribe_Topics[MAX_SUBSCRIBE_TOPICS] = {MQTTString_initializer};
	uint8_t Topics_Number = 0;
	uint8_t Duplication_Flag = NO_DUPLICATION;
	int32_t QoS[MAX_SUBSCRIBE_TOPICS] = {DEFAULT_QoS};
	int32_t Packet_len = 0;
	HAL_StatusTypeDef HAL_Status = 0;
	uint8_t Send_Buffer[SEND_BUFFER_SIZE] = {0};
	MQTT_Packet.MQTTVersion = MQTT_3_1_1;	/*MQTT Version 3.1.1*/
	MQTT_Packet.clientID.cstring = Client->Client_ID;
	if((Client->Client_Username != NULL) && (Client->Client_Password != NULL))
	{
		MQTT_Packet.username.cstring = Client->Client_Username;
		MQTT_Packet.password.cstring = Client->Client_Password;
	}
	MQTT_Packet.keepAliveInterval = KEEP_ALIVE_VALUE;
	MQTT_Packet.cleansession = 1;
	uint8_t k = 0;	/*counter to count number of topics*/
	for(uint8_t i=0,j=0 ; Topics_string[i] ; i++)
	{
		if(Topics_string[i] == ',')
		{
			k++;
			j=0;
			continue;
		}
		Topics_Holder[k][j++] = Topics_string[i];
		/*Max allowed topics for subscription*/
		if(k == MAX_SUBSCRIBE_TOPICS-1)
		{
			break;
		}
	}
	Topics_Number = k+1;
	for(uint8_t i=0 ; i<Topics_Number ; i++)
	{
		Subscribe_Topics[i].cstring = Topics_Holder[i];
	}
	/*Serialize Connect Packet*/
	Packet_len = MQTTSerialize_connect((uint8_t*)(Send_Buffer),SEND_BUFFER_SIZE,&MQTT_Packet);
	/*Serialize Subscribe Packet*/
	Packet_len += MQTTSerialize_subscribe((uint8_t*)(Send_Buffer+Packet_len),SEND_BUFFER_SIZE-Packet_len\
			,Duplication_Flag,Subscribe_Packet_ID,Topics_Number,Subscribe_Topics,QoS);
	if(Packet_len == 0)
	{
		return MQTT_NOT_OK;
	}
	/*adding carriage return to the end of the packet and increasing its size by 1*/
	Send_Buffer[Packet_len-1]='\x0d';
	Packet_len++;
	/*Send Connection and Subscribe Packet*/
	ESP_Response = ESP8266_Establish_TCP_Connection(Broker->Address,Broker->Socket);
	if(ESP_Response != RESPONSE_OK)
	{
		return TRANSPORT_ERROR;
	}
	ESP_Response = ESP8266_Send_TCP_Packets(Send_Buffer,Packet_len,0,SEND_ONLY,NULL);
	if(ESP_Response != RESPONSE_OK)
	{
		return TRANSPORT_ERROR;
	}
	/*Start Timer and start listening*/
	Subscribe_Packet_ID++;
	ESP_Response = ESP8266_Start_Receive_TCP_Packets();
	__HAL_TIM_SET_COUNTER(&htim4, 0);
	HAL_Status = HAL_TIM_Base_Start_IT(&htim4);
	if(HAL_Status != HAL_OK)
	{
		return MQTT_NOT_OK;
	}
	Client_State = CLIENT_CONNECTED;
	return MQTT_OK;
}
/************************************************************************************************************************
* Function Name: MQTT_UnSubscribe
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution response
* Description: Function to unsubscribe from all topics subscribed to and stop receiving any messages
************************************************************************************************************************/
MQTT_Transaction_State MQTT_UnSubscribe(void)
{
	if(ESP8266_Check_TCP_Server_Status() == RESPONSE_OK)
	{
		return MQTT_NOT_OK;
	}
	HAL_StatusTypeDef HAL_Status = 0;
	uint8_t Ping_Buffer[DISCONNECT_PACKET_BUFFER];
	int32_t Packet_len = 0;
	//ESP8266_RESPONSE ESP_Response = 0;
	/*Disable The Timer and stop listening*/
	HAL_Status = HAL_TIM_Base_Stop_IT(&htim4);
	if(HAL_Status != HAL_OK)
	{
		return MQTT_NOT_OK;
	}
	__HAL_TIM_SET_COUNTER(&htim4, 0);
//	ESP_Response = ESP8266_Stop_Receiving_TCP_Packets();
//	if(ESP_Response != RESPONSE_OK)
//	{
//		return TRANSPORT_ERROR;
//	}
	ESP8266_Close_TCP_Connection(0);
	for(uint8_t i=0;i<MQTT_Rx_Last_Messages_Number;i++)
	{
		memset(MQTT_Rx[i],0,MQTT_Rx_Buffer_Size);	/*Clear all buffers*/
	}
	MQTT_Rx_Index = 0;	/*Reset Index*/
	MQTT_Get_Message_Index = 0; /*Reset Index*/
	Client_State = CLIENT_DISCONNECTED;
	return MQTT_OK;
}
/************************************************************************************************************************
* Function Name: MQTT_Read
* Function ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): array of characters by address to obtain a MQTT Message
* Parameters (inout): NONE
* Parameters (out):NONE
* Return value: Execution response
* Description: Function to read MQTT messages that already received and stored in private buffers in FIFO manner
* 			   FIFO size can be configured in the cfg file
* 			   Messages are read in JSON Format, and need to be parsed as mentioned
************************************************************************************************************************/
MQTT_Transaction_State MQTT_Read(uint8_t* MQTT_Rx_Read)
{
	if(ESP8266_Check_TCP_Server_Status() == RESPONSE_OK)
	{
		return MQTT_NOT_OK;
	}
	if(strlen(MQTT_Rx[MQTT_Get_Message_Index]) == 0)
	{
		return MQTT_NOT_OK;
	}
	/*Check start and end of the messages to make sure it's in JSON format*/
	if((strstr(MQTT_Rx[MQTT_Get_Message_Index],"{") == NULL) || (strstr(MQTT_Rx[MQTT_Get_Message_Index],"}") == NULL))
	{
		return MQTT_NOT_OK;
	}
	uint16_t j = 0;	/*Iterator for messages received*/
	while(MQTT_Rx[MQTT_Get_Message_Index][j++] != '{');	/*Go until you parse start of JSON format*/
	j-=1;	/*decrement the extra 1 from j++ post increment*/
	strcpy(MQTT_Rx_Read,(MQTT_Rx[MQTT_Get_Message_Index])+j);
	memset(MQTT_Rx[MQTT_Get_Message_Index],0,MQTT_Rx_Buffer_Size);/*empty the buffer*/
	MQTT_Get_Message_Index++;
	if(MQTT_Get_Message_Index == MQTT_Rx_Last_Messages_Number)
	{
		MQTT_Get_Message_Index = 0;
	}
	return MQTT_OK;
}

/************************************************************************************************************************
* Function Name: MQTT_Subscribe_Ping_Callback
* Function ID[hex]: 0x04
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Callback function called only in STM HAL Timers callback periodically each 30 seconds as configured
*   		   To ping MQTT broker to keep the connection alive
************************************************************************************************************************/
void MQTT_Subscribe_Ping_Callback(void)
{
	/*If disconnected from Internet, Unsubscribe*/
//	if(ESP8266_Check_Internet_Connection() != RESPONSE_OK)
//	{
//		MQTT_UnSubscribe();
//		return;
//	}
	uint8_t Ping_Buffer[PING_PACKET_BUFFER] = {0};
	int32_t Packet_len = 0;
	Packet_len = MQTTSerialize_pingreq(Ping_Buffer,PING_PACKET_BUFFER);
	Ping_Buffer[Packet_len]='\x0d';
	Packet_len++;
	ESP8266_Send_TCP_Packets(Ping_Buffer,Packet_len,0,SEND_ONLY,NULL);
}
/************************************************************************************************************************
* Function Name: MQTT_Subscribe_Receive_Callback
* Function ID[hex]: 0x05
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Callback function, as Client is subscribed to a topic
* 			   when a message received this callback function is executed to store the received messages
* 			   in private buffers that can be obtained and read later using MQTT_Read in JSON Format
************************************************************************************************************************/
void MQTT_Subscribe_Receive_Callback(void)
{
	if(ESP8266_Check_TCP_Server_Status() == RESPONSE_OK)	/*if TCP server is on*/
	{
		return;	/*Do nothing*/
	}
	uint32_t Data_size = 0;
	uint16_t Payload_start = 0;
	uint8_t MQTT_Rx_temp[MQTT_Rx_Buffer_Size] = {0};
	memset(MQTT_Rx[MQTT_Rx_Index],0,MQTT_Rx_Buffer_Size);	/*Clear the Rx Buffer*/
	/*no thing to receive*/
	if(ESP8266_Check_Inbox(0) != RESPONSE_OK)
	{
		return;
	}
	ESP8266_Read_TCP_Packets(MQTT_Rx_temp,&Data_size,0);
	if(strstr(MQTT_Rx_temp+32,"{") == NULL)	/*To fix any possible hard fault due to wrong memory access*/
	{
		return;	/*if no sign of MQTT message*/
	}
	while(MQTT_Rx_temp[Payload_start] != '{')
	{
		Payload_start++;
	}
	memcpy(MQTT_Rx[MQTT_Rx_Index],MQTT_Rx_temp+Payload_start,Data_size);
	if(strlen(MQTT_Rx[MQTT_Rx_Index]) > 0 && \
			((strstr(MQTT_Rx[MQTT_Rx_Index],"{") != NULL) || (strstr(MQTT_Rx[MQTT_Rx_Index],"}") != NULL)))
	{
		MQTT_Rx_Index++;
	}
	else
	{
		memset(MQTT_Rx[MQTT_Rx_Index],0,MQTT_Rx_Buffer_Size);/*empty the buffer*/
	}
	if(MQTT_Rx_Index == MQTT_Rx_Last_Messages_Number)
	{
		MQTT_Rx_Index = 0;	/*reset the index*/
	}
}
/************************************************************************************************************************
* Function Name: MQTT_Check_Inbox
* Function ID[hex]: 0x06
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to check whether to MQTT Rx Inbox has messages or not
* 			   MQTT_OK means inbox has messages, MQTT_NOT_OK means inbox is empty
************************************************************************************************************************/
MQTT_Transaction_State MQTT_Check_Inbox(void)
{
	if(ESP8266_Check_TCP_Server_Status() == RESPONSE_OK)
	{
		return MQTT_NOT_OK;
	}
	for(uint8_t k=0 ; k<MQTT_Rx_Last_Messages_Number ; k++)
	{
		if(strlen(MQTT_Rx[k]) > 0)
		{
			return MQTT_OK;
		}
	}
	return MQTT_NOT_OK;
}
/************************************************************************************************************************
* Function Name: MQTT_Check_Client_Connection
* Function ID[hex]: 0x07
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to check whether MQTT Client-Broker are connected or not
* 			   MQTT_OK means Client Connected, MQTT_NOT_OK means Client Disconnected
************************************************************************************************************************/
MQTT_Transaction_State MQTT_Check_Client_Connection(void)
{
	if(ESP8266_Check_TCP_Server_Status() == RESPONSE_OK)
	{
		return MQTT_NOT_OK;
	}
	if(Client_State == CLIENT_DISCONNECTED)
	{
		return MQTT_NOT_OK;
	}
	return MQTT_OK;
}
