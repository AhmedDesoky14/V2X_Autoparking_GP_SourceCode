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
 *                     							      SECURED CAN STACK OPTION
 ***********************************************************************************************************************/
#define SECURE_COMM_V2I   1
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "tcp_client.h"
#include "wifi_manager.h"
#if(SECURE_COMM_V2I == 1)
#include "SecOC.h"
#endif
#include <thread>
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.0.0*/
#define V2I_CLIENT_SW_MAJOR_VERSION           (1U)
#define V2I_CLIENT_SW_MINOR_VERSION           (0U)
#define V2I_CLIENT_SW_PATCH_VERSION           (0U)
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define START_CONNECTION                    std::string("V2I:CONNECT:")
#define END_CONNECTION                      std::string("V2I:DISCONNECT")
#define SECURITY_PARAMETER                  (unsigned char*)("+Security:")
#define SECURITY_PARAMETER_SIZE             10
#define MAX_SECURED_MESSAGE_SIZE            510
#define V2I_RX_MAX_NO_BUFFER                5
/************************************************************************************************************************
 *                     						            MODULE CLASS
 ***********************************************************************************************************************/
class v2i_client
{
private:
/************************************************************************************************************************
 *                    							         Private Variables
 ***********************************************************************************************************************/
tcp_client My_TCP_Connection;
unsigned int My_Vehicle_ID;
unsigned int Current_Infrastructure_ID;
unsigned char Running_Thread;
std::thread *Rx_Thread = NULL;
unsigned char V2I_Rx_Buffer[V2I_RX_MAX_NO_BUFFER][MAX_SECURED_MESSAGE_SIZE] = {0};
unsigned int V2I_Rx_Buffer_Sizes[V2I_RX_MAX_NO_BUFFER] = {0};
unsigned char Rx_Index = 0;
unsigned char V2I_Read_Index = 0;
public:
/************************************************************************************************************************
 *                     				          Private Functions Prototypes
 ***********************************************************************************************************************/
void v2i_client::V2I_Receive_Thread(void);
/************************************************************************************************************************
 *                     						              CONSTRUCTOR
 ***********************************************************************************************************************/
v2i_client(unsigned int Vehicle_ID)
{
    My_Vehicle_ID = Vehicle_ID;
}
/************************************************************************************************************************
 *                     						          PUBLIC DATA TYPES
 ***********************************************************************************************************************/
typedef enum
{
  V2I_OK,V2I_NOT_OK,TCP_ERROR,WiFi_ERROR,SECURITY_ERROR
}V2I_CLIENT_STATUS;
/************************************************************************************************************************
 *                     				          Public Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
V2I_CLIENT_STATUS V2I_Search(std::string Infrastructure);
V2I_CLIENT_STATUS V2I_Connect(std::string Infrastructure,std::string Password,\
            std::string IP_Address,unsigned short Port,unsigned int Infrastructure_ID);
V2I_CLIENT_STATUS V2I_Disconnect(void);
V2I_CLIENT_STATUS V2I_Send_Message(std::string Message,SecurityLevel Security_Level);
V2I_CLIENT_STATUS V2I_Read_Message(std::string &Message);
V2I_CLIENT_STATUS V2I_Check_Inbox(void);
V2I_CLIENT_STATUS V2I_Check_Connection(void)
};
/************************************************************************************************************************
 *                     						             CLASS END
 ***********************************************************************************************************************/
#endif /*V2I_CLIENT_H*/
