/************************************************************************************************************************
 * 	Module:
 * 	File Name:
 *  Authors:
 *	Date:
 *	*********************************************************************************************************************
 *	Description:
 *
 *
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "v2i_client.h"
/***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
static tcp_client My_TCP_Connection;
static pthread_t V2I_Rx_Thread; /*Thread PID variable*/
static unsigned int My_Vehicle_ID;
static unsigned int Current_Infrastructure_ID;
static unsigned char V2I_Rx_Buffer[V2I_RX_MAX_NO_BUFFER][MAX_SECURED_MESSAGE_SIZE] = {0};
static unsigned int V2I_Rx_Buffer_Sizes[V2I_RX_MAX_NO_BUFFER] = {0};
static unsigned char Rx_Index = 0;
static unsigned char V2I_Read_Index = 0;
/***********************************************************************************************************************
 *                     					      PRIVATE FUNCTIONS PROTOTYPES
 ***********************************************************************************************************************/
static void *V2I_Receive_Thread(void* v);
/***********************************************************************************************************************
 *                     					        FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name:
* Function ID[hex]: 0x
* Sync/Async:
* Reentrancy:
* Parameters (in):
* Parameters (inout):
* Parameters (out):
* Return value:
* Description: 
* 
*
************************************************************************************************************************/
V2I_CLIENT_STATUS V2I_Search(string Infrastructure)
{
    if(WiFi_Search_AP(Infrastructure.c_str()) != WIFI_OK)
    {
        return V2I_NOT_OK;  /*Access Point not found*/
    }
    return V2I_OK;  /*Access Point found*/
}
/************************************************************************************************************************
* Function Name:
* Function ID[hex]: 0x
* Sync/Async:
* Reentrancy:
* Parameters (in):
* Parameters (inout):
* Parameters (out):
* Return value:
* Description: 
* 
*
************************************************************************************************************************/
V2I_CLIENT_STATUS V2I_Connect(string Infrastructure,string Password,string IP_Address,\
            unsigned short Port,unsigned int Infrastructure_ID,unsigned int Vehicle_ID)
{
    string Start_Connect = START_CONNECTION + '\0' + to_string(Vehicle_ID) + '\r';   /*Create start connection message*/
    static pthread_attr_t Rx_Thread_Attributes; /*Thread attributes to configure*/
    // cpu_set_t Rx_CPU;   /*Variable to set the dediacated core to run the thread*/
    struct sched_param Scheduling_Priority;
    if(WiFi_Connect(Infrastructure.c_str(),Password.c_str()) != WIFI_OK)
    {
        return WiFi_ERROR;
    }
    if(My_TCP_Connection.TCP_Establish_Connection(IP_Address,Port) != tcp_client::TCP_OK)
    {
        return TCP_ERROR;
    }   
    pthread_attr_init(&Rx_Thread_Attributes);   /*init attributes*/
    pthread_attr_setinheritsched(&Rx_Thread_Attributes,PTHREAD_EXPLICIT_SCHED);
    // CPU_ZERO(&Rx_CPU);
    // CPU_SET(1,&Rx_CPU);    /*set to run on core id: 1*/
    // pthread_attr_setaffinity_np(&Rx_Thread_Attributes,sizeof(cpu_set_t),&Rx_CPU);
    pthread_attr_setschedpolicy(&Rx_Thread_Attributes,SCHED_FIFO);
    Scheduling_Priority.sched_priority = V2I_COMMUNICATIONS_PRIORITY;
    pthread_attr_setschedparam(&Rx_Thread_Attributes,&Scheduling_Priority);
    if(pthread_create(&V2I_Rx_Thread,&Rx_Thread_Attributes,&V2I_Receive_Thread,NULL) != 0)
    {
        return V2I_NOT_OK;
    }
    pthread_attr_destroy(&Rx_Thread_Attributes);
    if(My_TCP_Connection.TCP_Send_Message((unsigned char*)Start_Connect.c_str(),(unsigned int)Start_Connect.length()) != tcp_client::TCP_OK)
    {
        return TCP_ERROR;
    }
    Current_Infrastructure_ID = Infrastructure_ID;
    My_Vehicle_ID = Vehicle_ID;
    return V2I_OK;
}
/************************************************************************************************************************
* Function Name:
* Function ID[hex]: 0x
* Sync/Async:
* Reentrancy:
* Parameters (in):
* Parameters (inout):
* Parameters (out):
* Return value:
* Description: 
* 
*
************************************************************************************************************************/
static void *V2I_Receive_Thread(void* v)
{
    unsigned char Temp_Buffer[MAX_SECURED_MESSAGE_SIZE] = {0};
    unsigned int Temp_Rx_Size = 0;
    while(1)
    {
        if(My_TCP_Connection.TCP_Check_Inbox() == tcp_client::TCP_OK)
        {
            if((My_TCP_Connection.TCP_Read_Message(Temp_Buffer,&Temp_Rx_Size) == tcp_client::TCP_OK))
            {
                if(Temp_Rx_Size > 0)
                {
                    memset(V2I_Rx_Buffer[Rx_Index],0,MAX_SECURED_MESSAGE_SIZE);
                    memcpy(V2I_Rx_Buffer[Rx_Index],Temp_Buffer,Temp_Rx_Size);
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
}
/************************************************************************************************************************
* Function Name:
* Function ID[hex]: 0x
* Sync/Async:
* Reentrancy:
* Parameters (in):
* Parameters (inout):
* Parameters (out):
* Return value:
* Description: 
* 
*
************************************************************************************************************************/
V2I_CLIENT_STATUS V2I_Disconnect(void)
{
    string End_Connect = END_CONNECTION;
    if(My_TCP_Connection.TCP_Send_Message((unsigned char*)End_Connect.c_str(),(unsigned int)End_Connect.length()) != tcp_client::TCP_OK)
    {
        return TCP_ERROR;
    }
    if(pthread_cancel(V2I_Rx_Thread) != 0)
    {
        return V2I_NOT_OK;
    }
    if(My_TCP_Connection.TCP_Close_Connetion() != tcp_client::TCP_OK)
    {
        return TCP_ERROR;
    }
    if(WiFi_Disconnect() != WIFI_OK)
    {
        return WiFi_ERROR;
    }
    /*Clear buffers, variables and resources*/
    for(unsigned int k=0;k<V2I_RX_MAX_NO_BUFFER;k++)  
    {
        memset(V2I_Rx_Buffer[k],0,MAX_SECURED_MESSAGE_SIZE);
        V2I_Rx_Buffer_Sizes[k] = 0;
    }
    Rx_Index = 0;
    V2I_Read_Index = 0;
    Current_Infrastructure_ID = 0;
    My_Vehicle_ID = 0;
    return V2I_OK;
}
/************************************************************************************************************************
* Function Name:
* Function ID[hex]: 0x
* Sync/Async:
* Reentrancy:
* Parameters (in):
* Parameters (inout):
* Parameters (out):
* Return value:
* Description: 
* 
*
************************************************************************************************************************/
V2I_CLIENT_STATUS V2I_Send_Message(string Message/*,SecurityLevel Security_Level*/)
{
    unsigned char Secured_Message[MAX_SECURED_MESSAGE_SIZE] =  {0};
    unsigned int Secured_Message_Size = 0;

    #ifdef COM_SECURED


    /*Cyber Security Start*/
    if(SecOC_Send_Secured(Message.c_str(),Message.length(),Secured_Message,&Secured_Message_Size,Current_Infrastructure_ID) != SECURITY_OK)
    {
        return SECURITY_ERROR;
    }
    memcpy(Secured_Message+Secured_Message_Size,SECURITY_PARAMETER,SECURITY_PARAMETER_SIZE)
    Secured_Message[Secured_Message_Size+SECURITY_PARAMETER_SIZE] = Security_Level;
    Secured_Message_Size = Secured_Message_Size + SECURITY_PARAMETER_SIZE + 1;


    if(My_TCP_Connection.TCP_Send_Message(Secured_Message,Secured_Message_Size) != tcp_client::TCP_OK)
    {
        return TCP_ERROR;
    }
    /*Cyber Security End*/


    #else
    if(My_TCP_Connection.TCP_Send_Message((unsigned char*)Message.c_str(),(unsigned int)Message.length()) != tcp_client::TCP_OK)
    {
        return TCP_ERROR;
    }

#endif
    return V2I_OK;
}
/************************************************************************************************************************
* Function Name:
* Function ID[hex]: 0x
* Sync/Async:
* Reentrancy:
* Parameters (in):
* Parameters (inout):
* Parameters (out):
* Return value:
* Description: 
* 
*
************************************************************************************************************************/
V2I_CLIENT_STATUS V2I_Read_Message(string &Message)
{
    unsigned char Secured_Message[MAX_SECURED_MESSAGE_SIZE] = {0};
    unsigned char Plain_Message[MAX_SECURED_MESSAGE_SIZE] = {0};
    unsigned int Plain_Message_Size = 0;
    unsigned char* Secuirty_Parse;
    unsigned int Secured_Message_Size = V2I_Rx_Buffer_Sizes[V2I_Read_Index];
    // SecurityLevel Message_Security_Level = 0;
    if(Secured_Message_Size == 0)
    {
        return V2I_NOT_OK;  /*No messages*/
    }
    memcpy(Secured_Message,V2I_Rx_Buffer[V2I_Read_Index],Secured_Message_Size);
    memset(V2I_Rx_Buffer[V2I_Read_Index],0,MAX_SECURED_MESSAGE_SIZE);
    V2I_Rx_Buffer_Sizes[V2I_Read_Index] = 0;
    V2I_Read_Index++;
    if(V2I_Read_Index == V2I_RX_MAX_NO_BUFFER)
    {
        V2I_Read_Index = 0;
    }

    #ifdef COM_SECURED


    Secuirty_Parse = strstr(Secured_Message+Secured_Message_Size-SECURITY_PARAMETER_SIZE-1,SECURITY_PARAMETER);
    if(Secuirty_Parse == NULL)
    {
            return V2I_NOT_OK;
    }
    for(unsigned char j=0;Secuirty_Parse[j] != ':';j++)
	{
		if(Secuirty_Parse[j] == ':')
		{
			Message_Security_Level = Secuirty_Parse[j+1];	/*Security Level obtained*/
			break;
		}
	}
    for(unsigned m=Secured_Message_Size-SECURITY_PARAMETER_SIZE;m<SECURITY_PARAMETER_SIZE+1;m++)	/*remove +Security:x*/
	{
		Secured_Message[m] = '\0';
	}
    if(SecOC_Recieve_Secured(Secured_Message,Secured_Message_Size-SECURITY_PARAMETER_SIZE-1,Plain_Message,&Plain_Message_Size,Message_Security_Level,Current_Infrastructure_ID) != SECURITY_OK)
    {
        return SECURITY_ERROR;
    }
    Message = Plain_Message;    /*Get Message*/
#else



    Message = (char*)(Secured_Message);

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
V2I_CLIENT_STATUS V2I_Check_Inbox(void)
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
V2I_CLIENT_STATUS V2I_Check_Connection(void)
{
    if(My_TCP_Connection.TCP_Check_Connection() != tcp_client::TCP_OK)
    {
        return V2I_NOT_OK;
    }
    return V2I_OK;
}