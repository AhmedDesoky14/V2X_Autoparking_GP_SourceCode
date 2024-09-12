/************************************************************************************************************************
 * 	Module: V2I Server Service Manager Module
 * 	File Name: v2i_server_service_manager.h
 *  Authors: Ahmed Desoky
 *	Date: 12/4/2024
 *	*********************************************************************************************************************
 *	Description: V2I Server Service Manager, Module that handles and manages V2I communications and security
 *				 from infrastructure side to deals with vehicles.
 *				 This Module function only work if TCP server is ON.
 *				 If TCP server if OFF all functions return V2I_NOT_OK.
 *				 This means that in this module and this mode the infrastructure is unable to
 *				 establish any TCP connection.
 *				 V2I Server operates with specific made protocol:
 *				 	-To connect as client to the V2I Server, send -> "V2I:CONNECT:xxxx\r", where xxxx is the vehicle ID
 *				 	-Then you can send any type of data followed by "+Security:x", where x is level of security,
 *				  		if not secured set x=0
 *				 	-To close the connection, send -> "V2I:DISCONNECT", and the connection will be closed automatically
 *				 	-Any other ways of connections are considered a bad connection and the TCP connection is closed immediately
 ***********************************************************************************************************************/
#ifndef V2I_SERVICE_MANAGER_H
#define V2I_SERVICE_MANAGER_H
/************************************************************************************************************************
 *                     							      SECURED V2I STACK OPTION
 ***********************************************************************************************************************/
#define SECURE_COMM   1
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#if(SECURE_COMM == 1)
#include "SecOC.h"
#endif
#include "esp8266.h"
#include "ESP_Mode_Switch.h"
#include "v2i_messages_ids.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.1.1*/
#define V2I_SERVER_SERVICE_MANAGER_SW_MAJOR_VERSION           (1U)
#define V2I_SERVER_SERVICE_MANAGER_SW_MINOR_VERSION           (1U)
#define V2I_SERVER_SERVICE_MANAGER_SW_PATCH_VERSION           (1U)
/*Software Version checking of Dependent Module - ESP8266 Driver - Version 2.4.4*/
#if ((ESP8266_SW_MAJOR_VERSION != (2U))\
 ||  (ESP8266_SW_MINOR_VERSION != (4U))\
 ||  (ESP8266_SW_PATCH_VERSION != (4U)))
  #error "The Software version of ESP8266 Module does not match the configurations expected version"
#endif
/*Software Version checking of Dependent Module - SecOC - Version 1.1.1*/
#if ((SECOC_SW_MAJOR_VERSION != (1U))\
 ||  (SECOC_SW_MINOR_VERSION != (1U))\
 ||  (SECOC_SW_PATCH_VERSION != (1U)))
  #error "The Software version of SecOC Module does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
/*V2I Communications commands*/
#define START_CONNECTION		          (uint8_t*)("V2I:CONNECT:")
#define READY_CONNECTION				  (uint8_t*)("V2I:READY:")
#define CONTINUE_MESSAGE				  (uint8_t*)("CONTINUE")
#define END_CONNECTION			          (uint8_t*)("V2I:DISCONNECT")
#define MORE_SERVICES					  (uint8_t*)("MORE")
#define REQUEST_MESSAGE				      (uint8_t*)("REQUEST:")
#define SEND_MESSAGE					  (uint8_t*)("SEND:")
#define ACK_SEND						  (uint8_t*)("ACKNOWLEDGE:SEND")	/*Ack to start to send from vehicle*/
#define SECURITY_LEVEL_PARSE	          (uint8_t*)("+Security:")
#define ACK_REQUEST						  (uint8_t*)("ACKNOWLEDGE")		/*Ack to the vehicle that request is received and send the requested data*/
#define VEHICLES_ID_SIZE		          8
#define SECURITY_LEVEL_PARSE_SIZE     	  10
#define MAX_SECURED_MESSAGE		          530
#define V2I_MAX_MESSAGE					  240
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
/*enum for different responses*/
typedef enum
{
	V2I_OK,V2I_NOT_OK,WIRELESS_TRANSPORT_ERROR,SECURITY_ERROR
}V2I_SERVICE_STATE;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
V2I_SERVICE_STATE V2I_Start_Reception(void);
V2I_SERVICE_STATE V2I_Stop_Reception(void);
V2I_SERVICE_STATE V2I_Check_Reception(void);
V2I_SERVICE_STATE V2I_Send_Message(uint8_t* Message,uint32_t Message_Size,uint32_t Vehicle_ID,SecurityLevel Security);
void V2I_Receive_Message_Callback(void);
V2I_SERVICE_STATE V2I_Get_Received_Message(uint8_t* Message,uint32_t* Message_Size,uint32_t Vehicle_ID);
V2I_SERVICE_STATE V2I_Check_Inbox(uint32_t Vehicle_ID);
V2I_SERVICE_STATE V2I_Check_Clients(uint32_t* Vehicle_ID);
V2I_SERVICE_STATE V2I_Check_AccessPoint(uint8_t* connected_vehicles_count);
V2I_SERVICE_STATE V2I_Check_Connected_Vehicles_Number(uint8_t* Number);
#endif /*V2I_SERVICE_MANAGER_H*/
