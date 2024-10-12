/************************************************************************************************************************
 * 	Module: V2I Client
 * 	File Name: v2i_client.h
 *  Authors: Ahmed Desoky
 *	Date: 5/5/2024
 *	*********************************************************************************************************************
 *	Description: V2I Client for Raspberry Pi 4 dedicated to V2I communications with V2I servers utilizing WiFi
 ***********************************************************************************************************************/
#ifndef V2I_CLIENT_H
#define V2I_CLIENT_H
/************************************************************************************************************************
 *                     							             INCLUDES
***********************************************************************************************************************/
#define COM_SECURED_V2I /*Comment this line if you don't want to include and use communications security*/
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include "tcp_client.h"
#include "wifi_manager.h"

#ifdef COM_SECURED_V2I
#include "SecOC.h"
#endif
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.0.0*/
#define V2I_CLIENT_SW_MAJOR_VERSION           (1U)
#define V2I_CLIENT_SW_MINOR_VERSION           (0U)
#define V2I_CLIENT_SW_PATCH_VERSION           (0U)
/*Software Version checking of Dependent Module - MTCP Client Class - Version 1.0.0*/
#if ((TCP_CLIENT_SW_MAJOR_VERSION != (1U))\
 ||  (TCP_CLIENT_SW_MINOR_VERSION != (0U))\
 ||  (TCP_CLIENT_SW_PATCH_VERSION != (0U)))
  #error "The Software version of TCP Client Class does not match the expected version"
#endif
/*Software Version checking of Dependent Module - WiFi Manager - Version 2.0.1*/
#if ((WIFI_MANAGER_SW_MAJOR_VERSION != (2U))\
 ||  (WIFI_MANAGER_SW_MINOR_VERSION != (0U))\
 ||  (WIFI_MANAGER_SW_PATCH_VERSION != (1U)))
  #error "The Software version of WiFi Manager Module does not match the expected version"
#endif
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define START_CONNECTION                    (char*)("V2I:CONNECT:")
#define END_CONNECTION                      (char*)("V2I:DISCONNECT")
#define SECURITY_PARAMETER                  (char*)("+Security:")
#define SECURITY_PARAMETER_SIZE             10
#define MAX_SECURED_MESSAGE_SIZE            512
#define V2I_RX_MAX_NO_BUFFER                5
#define V2I_COMMUNICATIONS_PRIORITY         31
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
typedef enum
{
  V2I_OK,V2I_NOT_OK,TCP_ERROR,WiFi_ERROR,SECURITY_ERROR
}V2I_CLIENT_STATUS;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
V2I_CLIENT_STATUS V2I_Search(string Infrastructure);
V2I_CLIENT_STATUS V2I_Connect(string Infrastructure,string Password,string IP_Address,\
            unsigned short Port,unsigned int Infrastructure_ID,unsigned int Vehicle_ID);
V2I_CLIENT_STATUS V2I_Disconnect(void);
V2I_CLIENT_STATUS V2I_Send_Message(string Message/*,SecurityLevel Security_Level*/);
V2I_CLIENT_STATUS V2I_Read_Message(string &Message);
V2I_CLIENT_STATUS V2I_Check_Inbox(void);
V2I_CLIENT_STATUS V2I_Check_Connection(void); 
#endif /*V2I_CLIENT_H*/
