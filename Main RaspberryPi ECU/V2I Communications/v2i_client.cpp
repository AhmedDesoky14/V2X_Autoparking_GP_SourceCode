/************************************************************************************************************************
 * 	Module: V2I Client
 * 	File Name: v2i_client.cpp
 *  Authors: Ahmed Desoky
 *	Date: 5/5/2024
 *	*********************************************************************************************************************
 *	Description: V2I Client for Raspberry Pi 4 dedicated to V2I communications with V2I servers utilizing WiFi
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "v2i_client.h"
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
v2i_client::V2I_CLIENT_STATUS v2i_client::V2I_Search(std::string Infrastructure)
{
    if(WiFi_Search_AP(Infrastructure.c_str()) != WIFI_OK)
    {
        return V2I_NOT_OK;  /*Access Point not found*/
    }
    return V2I_OK;  /*Access Point found*/
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
v2i_client::V2I_CLIENT_STATUS v2i_client::V2I_Connect(std::string Infrastructure,std::string Password,\
                                    std::string IP_Address,unsigned short Port,unsigned int Infrastructure_ID)
{
    std::string Start_Connect = START_CONNECTION + std::string(My_Vehicle_ID) + '\r';   /*Create start connection message*/
    if(WiFi_Connect(Infrastructure.c_str(),Password().c_str) != WIFI_OK)
    {
        return WiFi_ERROR;
    }
    if(My_TCP_Connection.TCP_Establish_Connection(IP_Address,Port) != TCP_OK)
    {
        return TCP_ERROR;
    }   
    if(My_TCP_Connection.TCP_Send_Message(Start_Connect.c_str(),Start_Connect.length()) != TCP_OK)
    {
        return TCP_ERROR;
    }
    Current_Infrastructure_ID = Infrastructure_ID;
    Running_Thread = 1;
    Rx_Thread = Crt_thread std::thread(V2I_Receive_Thread); /*Start Receive Thread*/
    return V2I_OK;
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
v2i_client::V2I_CLIENT_STATUS v2i_client::V2I_Disconnect(void)
{
    std::string End_Connect = END_CONNECTION;   /*Create end connection message*/
    if(My_TCP_Connection.TCP_Send_Message(End_Connect.c_str(),End_Connect.length()) != TCP_OK)
    {
        return TCP_ERROR;
    }
    if(My_TCP_Connection.TCP_Close_Connetion() != TCP_OK)
    {
        return TCP_ERROR;
    }
    if(WiFi_Disconnect() != WIFI_OK)
    {
        return WiFi_ERROR;
    }
    /*Reset all*/
    Running_Thread = 0;
    for(unsigned int k=0;k<V2I_RX_MAX_NO_BUFFER;k++)  
    {
        memset(V2I_Rx_Buffer[k],0,MAX_SECURED_MESSAGE_SIZE);
        V2I_Rx_Buffer_Sizes[k] = 0;
    }
    Rx_Index = 0;
    V2I_Read_Index = 0;
    Current_Infrastructure_ID = 0;
    return V2I_OK;
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
v2i_client::V2I_CLIENT_STATUS v2i_client::V2I_Send_Message(std::string Message,SecurityLevel Security_Level)
{
    unsigned char Secured_Message[MAX_SECURED_MESSAGE_SIZE] =  {0};
    unsigned int Secured_Message_Size = 0;
#if(SECURE_COMM_V2I == 1)
    if(SecOC_Send_Secured(Message.c_str(),Message.length,Secured_Message,&Secured_Message_Size,Current_Infrastructure_ID) != SECURITY_OK)
    {
        return SECURITY_ERROR;
    }
    memcpy(Secured_Message+Secured_Message_Size,SECURITY_PARAMETER,SECURITY_PARAMETER_SIZE)
    Secured_Message[Secured_Message_Size+SECURITY_PARAMETER_SIZE] = Security_Level;
    Secured_Message_Size = Secured_Message_Size + SECURITY_PARAMETER_SIZE + 1;
    if(My_TCP_Connection.TCP_Send_Message(Secured_Message,Secured_Message_Size) != TCP_OK)
    {
        return TCP_ERROR;
    }
#else
    if(My_TCP_Connection.TCP_Send_Message(Message.c_str(),Message.length()) != TCP_OK)
    {
        return TCP_ERROR;
    }
#endif
    return V2I_OK;
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
void v2i_client::V2I_Receive_Thread(void)
{
    unsigned char Temp_Buffer[MAX_SECURED_MESSAGE_SIZE] = {0};
    unsigned int Temp_Rx_Size = 0;
    while(Running_Thread)
    {
        if(My_TCP_Connection.TCP_Check_Inbox() == TCP_OK)
        {
            if((My_TCP_Connection.TCP_Read_Message(Temp_Buffer,&Temp_Rx_Size) == TCP_OK))
            {
                if(Temp_Rx_Size > 0)
                {
                    memset(V2I_Rx_Buffer[Rx_Index],0,MAX_SECURED_MESSAGE_SIZE);
                    memcpy(V2I_Rx_Buffer[Rx_Index],Temp_Buffer,Temp_Rx_Size)
                    V2I_Rx_Buffer_Sizes[Rx_Index] = Temp_Rx_Size;
                    Rx_Index++;
                    if(Rx_Index == V2I_RX_MAX_NO_BUFFER)
                    {
                        Rx_Index = 0;   /*Reser Index*/
                    }
                }
            }
        }
        Temp_Rx_Size = 0;
        memset(Temp_Buffer,0,MAX_SECURED_MESSAGE_SIZE);
    }
    return; /*Thread exited*/
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
v2i_client::V2I_CLIENT_STATUS v2i_client::V2I_Read_Message(std::string &Message)
{
    unsigned char Secured_Message[MAX_SECURED_MESSAGE_SIZE] = {0};
    unsigned char Plain_Message[MAX_SECURED_MESSAGE_SIZE] = {0};
    unsigned int Secured_Message_Size = V2I_Rx_Buffer_Sizes[V2I_Read_Index];
    unsigned int Plain_Message_Size = 0;
    unsigned char* Secuirty_Parse;
    SecurityLevel Message_Security_Level = 0;
    memcpy(Secured_Message,V2I_Rx_Buffer[V2I_Read_Index],Secured_Message_Size)
    V2I_Read_Index++;
    if(V2I_Read_Index == V2I_RX_MAX_NO_BUFFER)
    {
        V2I_Read_Index = 0;
    }
    if(Secured_Message_Size == 0)
    {
        return V2I_NOT_OK;  /*No messages*/
    }
#if(SECURE_COMM_V2I == 1)
    Secuirty_Parse = strstr(Secured_Message+Secured_Message_Size-SECURITY_PARAMETER_SIZE-1,SECURITY_LEVEL_PARSE);
    for(unsigned char j=0;Secuirty_Parse[j] != ':';j++)
	{
		if(Secuirty_Parse[j] == ':')
		{
			Message_Security_Level = Secuirty_Parse[j+1];	/*Security Level obtained*/
			break;
		}
	}
    for(unsigned m=0;m<SECURITY_LEVEL_PARSE_SIZE+1;m++)	/*remove +Security:x*/
	{
		Secuirty_Parse[m] = '\0';
	}
    if(SecOC_Recieve_Secured(Secured_Message,Secured_Message_Size,Plain_Message,&Plain_Message_Size,Message_Security_Level,Current_Infrastructure_ID) != SECURITY_OK)
    {
        return SECURITY_ERROR;
    }
    Message = Plain_Message;    /*Get Message*/
#else
    Message = Secured_Message;
#endif
    return V2I_OK;
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
v2i_client::V2I_CLIENT_STATUS v2i_client::V2I_Check_Inbox(void)
{
    for(unsigned int h=0;h<V2I_RX_MAX_NO_BUFFER;h++)
    {
        if(V2I_Rx_Buffer_Sizes[h] > 0)
        {
            return V2I_OK;
        }
    }
    return V2I_NOT_OK; /*No messages found*/
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
v2i_client::V2I_CLIENT_STATUS v2i_client::V2I_Check_Connection(void)
{
    if(My_TCP_Connection.TCP_Check_Connection() != TCP_OK)
    {
        return V2I_NOT_OK;
    }
    return V2I_OK;
}