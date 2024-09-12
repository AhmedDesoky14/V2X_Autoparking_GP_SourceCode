/************************************************************************************************************************
 * 	Module: TCP Client Module
 * 	File Name: tcp_client.cpp
 *  Authors: Ahmed Desoky, Omar Tolba
 *	Date: 17/4/2024
 *	*********************************************************************************************************************
 *	Description: TCP Client Module, it contains implementation of a class named tcp_Client
 *               each object of this class is dedicated to a single TCP connections 
 *               and data exchange with that connection.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "tcp_client.h"
/***********************************************************************************************************************
 *                     					     CLASS FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: TCP_Establish_Connection
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): IP Address string concerned for connection
                   Port to open that socket to
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Result of execution
* Description: Functio to establish TCP connection with the concerned end point
************************************************************************************************************************/
tcp_client::TCP_CLIENT_STATUS tcp_client::TCP_Establish_Connection(std::string IP_Address,unsigned short Socket_Port)
{
    /*This part is to check if there's an established connection or not*/
    FILE* netstat;
    char command[100];
    char read[200];
    sprintf(command,"netstat -ant | grep %s:%d",Current_Server_IP_Address.c_str(),Current_Server_Port);
    netstat = popen(command,"r");   //open pipe with option read-only
    if(netstat == NULL)
    {
        return TCP_SYSTEM_ERROR;
    }
    if((fgets(read,sizeof(read),netstat)) != NULL)  //means there's ongoing connection
    {
        pclose(netstat);
        return TCP_NOT_OK;
    }
    pclose(netstat); //close pipe
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);    /*Create Socket*/
    if (socket_fd == -1) 
    {
        return TCP_SYSTEM_ERROR;
    }
    Server.sin_family = AF_INET;
    Server.sin_port = htons(Socket_Port);   
    if((inet_pton(AF_INET, IP_Address.c_str(), &Server.sin_addr)) != 1)//Convert IPV4 Address to binary form
    {
        return TCP_SYSTEM_ERROR;
    }
    if ((connect(socket_fd,(struct sockaddr*)&Server,sizeof(Server))) != 0) 
    {
        return TCP_NOT_OK;
    }
    Current_Server_IP_Address = IP_Address;
    Current_Server_Port = Socket_Port;
    return TCP_OK;
}
/************************************************************************************************************************
* Function Name: TCP_Close_Connetion
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Result of execution
* Description: Function to close the established TCP connection
************************************************************************************************************************/
tcp_client::TCP_CLIENT_STATUS tcp_client::TCP_Close_Connetion(void)
{
    if((close(socket_fd)) != 0)
    {
       return TCP_NOT_OK;
    }
    Current_Server_IP_Address.clear();
    Current_Server_Port = 0;
    return TCP_OK;
}
/************************************************************************************************************************
* Function Name: TCP_Send_Message
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): String of the message to be sent
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Result of execution
* Description: Function to send a message to the established TCP connection
************************************************************************************************************************/
tcp_client::TCP_CLIENT_STATUS tcp_client::TCP_Send_Message(unsigned char* Message,unsigned int Message_Size)
{
    if (send(socket_fd,Message,Message_Size,0) < 0) 
    {
        return TCP_NOT_OK;
    }
    return TCP_OK;
}
/************************************************************************************************************************
* Function Name: TCP_Check_Inbox
* Function ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Result of execution
* Description: Function to check inbox for any received messages from the TCP connection
*              TCP_OK -> means there's a message ready for reading
*              TCP_NOT_OK -> means there's no message to read
*              TCP_SYSTEM_ERROR -> means error occured during checking
************************************************************************************************************************/
tcp_client::TCP_CLIENT_STATUS tcp_client::TCP_Check_Inbox(void)
{
    fd_set read_check_fds; //create set of read file descriptors
    int check_return;
    struct timeval check_timeout;
    check_timeout.tv_sec = 0;
    check_timeout.tv_usec = 0; //just to check the file descriptor
    FD_ZERO(&read_check_fds); //clear file descriptors set
    FD_SET(socket_fd,&read_check_fds);    //add the socket_fd to the set
    check_return = select(socket_fd+1,&read_check_fds,NULL,NULL,&check_timeout);
    if(check_return == -1)
    {
        return TCP_SYSTEM_ERROR;
    }
    else if(check_return == 0)
    {
        return TCP_NOT_OK;
    }
    return TCP_OK;  //messages to read are ready
}
/************************************************************************************************************************
* Function Name: TCP_Read_Message
* Function ID[hex]: 0x04
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Message string that concerned to be read and copied, by reference
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Result of execution
* Description: Function to read the received message after checking that connection inbox
************************************************************************************************************************/
tcp_client::TCP_CLIENT_STATUS tcp_client::TCP_Read_Message(unsigned char* Message,unsigned int* Message_Size)
{
    char Receive_Buffer[Max_Rx_Buffer_Size];
    ssize_t Bytes_received = recv(socket_fd,Receive_Buffer,Max_Rx_Buffer_Size,MSG_DONTWAIT);
    if(Bytes_received <= 0)
    {
        return TCP_NOT_OK;   
    }
    memcpy(Message,Receive_Buffer,Max_Rx_Buffer_Size);
    *Message_Size = Bytes_received;
    //Message = Receive_Buffer;
    return TCP_OK;
}
/************************************************************************************************************************
* Function Name: TCP_Check_Connection
* Function ID[hex]: 0x05
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): Message string that concerned to be read and copied, by reference
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Result of execution
* Description: Function to check if the TCP connection is still ongoing or not
*              TCP_OK -> means conenction is still ongoing
*              TCP_NOT_OK -> means there's no connection
*              TCP_SYSTEM_ERROR -> means error occured during checking
************************************************************************************************************************/
tcp_client::TCP_CLIENT_STATUS tcp_client::TCP_Check_Connection(void)
{
    FILE* netstat;
    char command[100];
    char read[200];
    sprintf(command,"netstat -ant | grep %s:%d",Current_Server_IP_Address.c_str(),Current_Server_Port);
    netstat = popen(command,"r");   //open pipe with option read-only
    if(netstat == NULL)
    {
        return TCP_SYSTEM_ERROR;
    }
    if((fgets(read,sizeof(read),netstat)) == NULL)  //if NULL, means that concerned connection is not established and not found
    {
        pclose(netstat);
        return TCP_NOT_OK;
    }
    pclose(netstat);
    return TCP_OK;
}