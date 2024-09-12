/************************************************************************************************************************
 * 	Module: TCP Client Module
 * 	File Name: tcp_client.h
 *  Authors: Ahmed Desoky, Omar Tolba
 *	Date: 17/4/2024
 *	*********************************************************************************************************************
 *	Description: TCP Client Module, it contains implementation of a class named tcp_Client
 *               each object of this class is dedicated to a single TCP connections 
 *               and data exchange with that connection.
 ***********************************************************************************************************************/
#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string>
#include <cstring>
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.0.0*/
#define TCP_CLIENT_SW_MAJOR_VERSION           (1U)
#define TCP_CLIENT_SW_MINOR_VERSION           (0U)
#define TCP_CLIENT_SW_PATCH_VERSION           (0U)
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define Max_Rx_Buffer_Size  512
/************************************************************************************************************************
 *                     						            MODULE CLASS
 ***********************************************************************************************************************/
class tcp_client
{
private:
/************************************************************************************************************************
 *                    							         Private Variables
 ***********************************************************************************************************************/
int socket_fd = 0;  //Socket file descriptor for data exchange
struct sockaddr_in Server;
std::string Current_Server_IP_Address;
unsigned short Current_Server_Port;
public:
/************************************************************************************************************************
 *                     						          PUBLIC DATA TYPES
 ***********************************************************************************************************************/
typedef enum
{
  TCP_OK,TCP_NOT_OK,TCP_SYSTEM_ERROR
}TCP_CLIENT_STATUS;
/************************************************************************************************************************
 *                     				          Public Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
TCP_CLIENT_STATUS TCP_Establish_Connection(std::string IP_Address,unsigned short Socket_Port);
TCP_CLIENT_STATUS TCP_Close_Connetion(void);
TCP_CLIENT_STATUS TCP_Send_Message(unsigned char* Message,unsigned int Message_Size);
TCP_CLIENT_STATUS TCP_Check_Inbox(void);
TCP_CLIENT_STATUS TCP_Read_Message(unsigned char* Message,unsigned int* Message_Size);
TCP_CLIENT_STATUS TCP_Check_Connection(void);
};
/************************************************************************************************************************
 *                     						             CLASS END
 ***********************************************************************************************************************/
#endif /*TCP_CLIENT_H*/
