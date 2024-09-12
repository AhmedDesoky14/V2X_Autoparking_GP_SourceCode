/************************************************************************************************************************
 * 	Module: V2I Server Service Manager Module
 * 	File Name: v2i_server_service_manager.c
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
/***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "v2i_server_service_manager.h"
/***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
/*Array of buffers to store data from different vehicles connections, same size as ESP8266 Rx Buffers*/
static uint8_t Denial_of_serivce_emergency_flag = 0;	/*Flag to indicate a bad connection*/
static uint8_t Denial_of_service_array[ESP8266_MAX_TCP_CONNECTIONS] = {0};	/*Array to show which connection is bad*/
static uint8_t Receiving_Indicator = 0;	/*Flag to indicate if receiving or not*/
static uint8_t Previous_connection[ESP8266_MAX_TCP_CONNECTIONS] = {0};/*Used to fix an issue in checking connected clients*/
static uint8_t V2I_Rx_Messages[ESP8266_MAX_TCP_CONNECTIONS][ESP8266_Rx_Buffers_Sizes]={0};
static uint32_t V2I_Rx_Messages_Size[ESP8266_MAX_TCP_CONNECTIONS] = {0};/*Rx Messages Size*/
static uint8_t V2I_Read_Messages[ESP8266_MAX_TCP_CONNECTIONS][ESP8266_Rx_Buffers_Sizes]={0};
static uint32_t V2I_Read_Messages_Size[ESP8266_MAX_TCP_CONNECTIONS] = {0};/*Rx Messages Size*/
static uint32_t Vehicle_Connection_Correspondence[ESP8266_MAX_TCP_CONNECTIONS] = {0};/*buffer to store Vehicles IDs corresponding to certain connection*/
static uint8_t Enabled_Connections[ESP8266_MAX_TCP_CONNECTIONS] = {0};/*buffer to indicate that their is an ongoing connection in the specified TCP connection*/
/***********************************************************************************************************************
 *                     					      PRIVATE FUNCTIONS PROTOTYPES
 ***********************************************************************************************************************/
static void V2I_DoS_Defender(void);
/***********************************************************************************************************************
 *                     					        FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: V2I_Start_Reception
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution response
* Description: Function to start receiving Messages
************************************************************************************************************************/
V2I_SERVICE_STATE V2I_Start_Reception(void)
{
	if(ESP8266_Check_TCP_Server_Status() != RESPONSE_OK)	/*If TCP server is not ON return not OK*/
	{
		return V2I_NOT_OK;
	}
	V2I_DoS_Defender();
	if(Receiving_Indicator == 1)
	{
		return V2I_OK;	/*already started*/
	}
	if(ESP8266_Start_Receive_TCP_Packets() != RESPONSE_OK)
	{
		return V2I_NOT_OK;
	}
	for(uint8_t k=0;k<ESP8266_MAX_TCP_CONNECTIONS;k++)
	{
		Denial_of_serivce_emergency_flag = 0;	/*using the denial of service method to close the connection after end message*/
		Denial_of_service_array[k] = 0;	/*Close the TCP connection*/
	}
	Receiving_Indicator = 1;
	return V2I_OK;
}
/************************************************************************************************************************
* Function Name: V2I_Stop_Reception
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution response
* Description: Function to stop receiving Messages
************************************************************************************************************************/
V2I_SERVICE_STATE V2I_Stop_Reception(void)
{
	if(ESP8266_Check_TCP_Server_Status() != RESPONSE_OK)	/*If TCP server is not ON return not OK*/
	{
		return V2I_NOT_OK;
	}
	V2I_DoS_Defender();
	if(Receiving_Indicator == 0)
	{
		return V2I_OK;	/*already stopped*/
	}
//	if(ESP8266_Stop_Receiving_TCP_Packets() != RESPONSE_OK)
//	{
//		return V2I_NOT_OK;
//	}
	for(uint8_t k=0;k<ESP8266_MAX_TCP_CONNECTIONS;k++)
	{
		/*Reset All*/
		Vehicle_Connection_Correspondence[k] = 0;
		Enabled_Connections[k] = 0;
		V2I_Rx_Messages_Size[k] = 0;
		Previous_connection[k] = 0;	/*Used for checking for connected clients*/
		memset(V2I_Rx_Messages[k],0,ESP8266_Rx_Buffers_Sizes);
		V2I_Read_Messages_Size[k] = 0;
		memset(V2I_Read_Messages[k],0,ESP8266_Rx_Buffers_Sizes);
		Denial_of_serivce_emergency_flag = 1;	/*using the denial of service method to close the connection after end message*/
		Denial_of_service_array[k] = 1;	/*Close the TCP connection*/
	}
	Receiving_Indicator = 0;
	return V2I_OK;
}
/************************************************************************************************************************
* Function Name: V2I_Check_Reception
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution response
* Description: Function to check if receiving Messages is ON or OFF
************************************************************************************************************************/
V2I_SERVICE_STATE V2I_Check_Reception(void)
{
	if(ESP8266_Check_TCP_Server_Status() != RESPONSE_OK)	/*If TCP server is not ON return not OK*/
	{
		return V2I_NOT_OK;
	}
	V2I_DoS_Defender();
	if(Receiving_Indicator == 0)
	{
		return V2I_NOT_OK;
	}
	return V2I_OK;
}
/************************************************************************************************************************
* Function Name: V2I_Send_Message
* Function ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Message to be sent
* 				   Message size
* 				   Concerned Vehicle ID
* 				   Security level of the message
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution response
* Description: Function used to send a message to clients and secure the message according to security level
************************************************************************************************************************/
V2I_SERVICE_STATE V2I_Send_Message(uint8_t* Message,uint32_t Message_Size,uint32_t Vehicle_ID,SecurityLevel Security)
{
	uint8_t Vehicle_Connection_ID = 0;
	uint8_t ID_Found_Flag = 0;
	uint8_t Secured_Message[MAX_SECURED_MESSAGE] = {0};
	uint32_t Secured_Message_Size = 0;
	uint8_t Send_Security_Char[2] = {0};
	if(ESP8266_Check_TCP_Server_Status() != RESPONSE_OK)	/*If TCP server is not ON return not OK*/
	{
		return V2I_NOT_OK;
	}
	V2I_DoS_Defender();
	for(unsigned int k=0;k<ESP8266_MAX_TCP_CONNECTIONS;k++)
	{
		if(Vehicle_Connection_Correspondence[k] == Vehicle_ID)
		{
			Vehicle_Connection_ID = k;
			ID_Found_Flag = 1;	/*ID found: means that Vehicle with that ID is in connection state*/
		}
	}
	if(ID_Found_Flag == 0)	/*Only send Data to connected */
	{
		return V2I_NOT_OK;
	}

#if(SECURE_COMM == 1)
	if(SecOC_Send_Secured(Message,Message_Size,Secured_Message,&Secured_Message_Size,Security,Vehicle_ID) != SECURITY_OK)
	{
		return SECURITY_ERROR;
	}
	itoa(Security,Send_Security_Char,10);
	memcpy(Secured_Message+Secured_Message_Size,SECURITY_LEVEL_PARSE,SECURITY_LEVEL_PARSE_SIZE);
	Secured_Message[Secured_Message_Size+SECURITY_LEVEL_PARSE_SIZE] = Send_Security_Char[0];
	Secured_Message_Size = Secured_Message_Size + SECURITY_LEVEL_PARSE_SIZE + 1;
	if(ESP8266_Send_TCP_Packets(Secured_Message,Secured_Message_Size,Vehicle_Connection_ID,SEND_ONLY,NULL) != RESPONSE_OK)
	{
		return WIRELESS_TRANSPORT_ERROR;
	}
#elif(SECURE_COMM == 0)
	if(ESP8266_Send_TCP_Packets(Message,Message_Size,Vehicle_Connection_ID,SEND_ONLY,NULL) != RESPONSE_OK)
	{
		return WIRELESS_TRANSPORT_ERROR;
	}
#endif
	return V2I_OK;
}
/************************************************************************************************************************
* Function Name: V2I_Receive_Message_Callback
* Function ID[hex]: 0x04
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution response
* Description: Callback function, don't call it in your application, just call it in the specific HAL
* 			   callback function called "HAL_UARTEx_RxEventCallback" beside ESP8266 Receive callback function
* 			   This function privately handles establishing and closing connections and storing received messages
************************************************************************************************************************/
void V2I_Receive_Message_Callback(void)
{
	if(Get_ESP_Operation_Mode() != V2I_Communications)	/*If not in V2I mode*/
	{
		return;
	}
	for(uint8_t k=0 ; k<ESP8266_MAX_TCP_CONNECTIONS ; k++)
	{
		if(ESP8266_Check_Inbox(k) == RESPONSE_OK)	/*If there's pending message in ESP8266 buffer*/
		{
			memset(V2I_Rx_Messages[k],0,ESP8266_Rx_Buffers_Sizes);	/*Clear Buffer before reading*/
			ESP8266_Read_TCP_Packets(V2I_Rx_Messages[k],&V2I_Rx_Messages_Size[k],k);
			if((strlen(V2I_Rx_Messages[k])==0) || (strlen(V2I_Rx_Messages[k]+1)==0))
			{
				continue;	/*Empty Buffer - False interrupt*/
			}
			if(strstr(V2I_Rx_Messages[k],START_CONNECTION) != NULL)	/*Connection Established*/
			{
				uint8_t Start_ID[VEHICLES_ID_SIZE] = {0};
				uint8_t count = 0;
				if(strstr(V2I_Rx_Messages[k],"\r") == NULL)	/*If \r not found could cause system hanging,
				 (security threat) causing denial of service*/
				{
					Denial_of_serivce_emergency_flag = 1;
					Denial_of_service_array[k] = 1;
					return;	/*refuse connection*/
				}
				for(uint8_t n=12;V2I_Rx_Messages[k][n] != '\r';n++)
				{
					Start_ID[count] = V2I_Rx_Messages[k][n];
					count++;
				}
				Vehicle_Connection_Correspondence[k] = atoi(Start_ID);
				Enabled_Connections[k] = 1;	/*Connection became ongoing*/
			}
			else if(strstr(V2I_Rx_Messages[k],END_CONNECTION) != NULL)
			{
				/*Vehicle Disconnected - Reset All*/
				Vehicle_Connection_Correspondence[k] = 0;
				Enabled_Connections[k] = 0;
				V2I_Rx_Messages_Size[k] = 0;
				Previous_connection[k] = 0;	/*Used for checking for connected clients*/
				memset(V2I_Rx_Messages[k],0,ESP8266_Rx_Buffers_Sizes);
				V2I_Read_Messages_Size[k] = 0;
				memset(V2I_Read_Messages[k],0,ESP8266_Rx_Buffers_Sizes);
				Denial_of_serivce_emergency_flag = 1;	/*using the denial of service method to close the connection after end message*/
				Denial_of_service_array[k] = 1;	/*Close the TCP connection*/
			}
			else
			{
				if(Enabled_Connections[k] != 1)	/*If connection not established correctly, bad connection*/
				{
					Denial_of_serivce_emergency_flag = 1;
					Denial_of_service_array[k] = 1;
					return;
				}
				else if(strstr(V2I_Rx_Messages[k]+V2I_Rx_Messages_Size[k]-SECURITY_LEVEL_PARSE_SIZE-1,SECURITY_LEVEL_PARSE) == NULL)	/*Message received doesn't include security parameter, BAD MESSAGE*/
				{
					Denial_of_serivce_emergency_flag = 1;
					Denial_of_service_array[k] = 1;
					return;
				}
				V2I_Read_Messages_Size[k] = V2I_Rx_Messages_Size[k];
				memcpy(V2I_Read_Messages[k],V2I_Rx_Messages[k],V2I_Read_Messages_Size[k]);
				V2I_Rx_Messages_Size[k] = 0;
				memset(V2I_Rx_Messages[k],0,ESP8266_Rx_Buffers_Sizes);
			}
			return;
		}
	}
}
/************************************************************************************************************************
* Function Name: V2I_Get_Received_Message
* Function ID[hex]: 0x05
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Pointer to string to store message
* 				   Pointer to uint32 to store message size
* 				   Vehicle connection ID
* Parameters (inout): NONE
* Parameters (out): Pointer to string to store message
* 				    Pointer to uint32 to store message size
* Return value: Execution response
* Description: Function to copy and get received messages of the given Vehicle ID
* 			   It handles security operations internally according to the received message security parameter
************************************************************************************************************************/
V2I_SERVICE_STATE V2I_Get_Received_Message(uint8_t* Message,uint32_t* Message_Size,uint32_t Vehicle_ID)
{
	uint8_t Vehicle_Connection_ID = 0;
	uint8_t ID_Found_Flag = 0;
	SecurityLevel ReceiveSecurity_Level = 0;
	uint8_t ReceiveSecurity_Level_char[2] = {0};
	uint32_t security_counter = 0;
	//SecurityLevel Rx_Security_Level = 0;
	if(ESP8266_Check_TCP_Server_Status() != RESPONSE_OK)	/*If TCP server is not ON return not OK*/
	{
		return V2I_NOT_OK;
	}
	V2I_DoS_Defender();
	for(uint8_t k=0 ; k<ESP8266_MAX_TCP_CONNECTIONS ; k++)
	{
		if(Vehicle_ID == Vehicle_Connection_Correspondence[k])
		{
			Vehicle_Connection_ID = k;
			ID_Found_Flag = 1;
		}
	}
	if(ID_Found_Flag == 0)
	{
		return V2I_NOT_OK;
	}
	if(V2I_Read_Messages_Size[Vehicle_Connection_ID] == 0)
	{
		return V2I_NOT_OK;	/*No message found*/
	}
#if(SECURE_COMM == 1)
	ReceiveSecurity_Level_char[0] = V2I_Read_Messages[Vehicle_Connection_ID][V2I_Read_Messages_Size[Vehicle_Connection_ID]-1];
	ReceiveSecurity_Level = atoi(ReceiveSecurity_Level_char);
	security_counter = V2I_Read_Messages_Size[Vehicle_Connection_ID]-SECURITY_LEVEL_PARSE_SIZE-1;
	while(V2I_Read_Messages[Vehicle_Connection_ID][security_counter] != '\0')
	{
		V2I_Read_Messages[Vehicle_Connection_ID][security_counter] = '\0';
		security_counter++;
	}
	if(SecOC_Recieve_Secured(V2I_Read_Messages[Vehicle_Connection_ID],V2I_Read_Messages_Size[Vehicle_Connection_ID]-SECURITY_LEVEL_PARSE_SIZE-1,Message,Message_Size,ReceiveSecurity_Level,Vehicle_ID) != SECURITY_OK)
	{
		return SECURITY_NOT_OK;
	}
#elif(SECURE_COMM == 0)
	*Message_Size = V2I_Read_Messages_Size[Vehicle_Connection_ID];
	memcpy(Message,V2I_Read_Messages[Vehicle_Connection_ID],V2I_Read_Messages_Size[Vehicle_Connection_ID]);
	V2I_Read_Messages_Size[Vehicle_Connection_ID] = 0;	/*Message ready, so fine*/
	memset(V2I_Read_Messages[Vehicle_Connection_ID],0,ESP8266_Rx_Buffers_Sizes); /*Message ready, so fine*/
#endif
	return V2I_OK;
}
/************************************************************************************************************************
* Function Name: V2I_Check_Inbox
* Function ID[hex]: 0x06
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): Concerned vehicle connection ID
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution response
* Description: Function to check inbox for any incoming messages for concerned vehicle ID stored as a connected vehicle
************************************************************************************************************************/
V2I_SERVICE_STATE V2I_Check_Inbox(uint32_t Vehicle_ID)
{
	if(ESP8266_Check_TCP_Server_Status() != RESPONSE_OK)	/*If TCP server is not ON return not OK*/
	{
		return V2I_NOT_OK;
	}
	V2I_DoS_Defender();
	for(uint8_t c=0;c<ESP8266_MAX_TCP_CONNECTIONS;c++)
	{
		if(Vehicle_Connection_Correspondence[c] == Vehicle_ID)
		{
			if(V2I_Read_Messages_Size[c] > 0)
			{
				return V2I_OK;	/*There's a message in the inbox*/
			}
		}
	}
	return V2I_NOT_OK;	/*No messages*/
}
/************************************************************************************************************************
* Function Name: V2I_Check_Clients
* Function ID[hex]: 0x07
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Vehicle ID variable by reference
* Parameters (inout): NONE
* Parameters (out): Vehicle ID connected
* Return value: Execution response
* Description: Function to check if there's any connected vehicles and return the vehicle ID
************************************************************************************************************************/
V2I_SERVICE_STATE V2I_Check_Clients(uint32_t* Vehicle_ID)
{
	uint8_t TCP_connections = 0;
	if(ESP8266_Check_TCP_Server_Status() != RESPONSE_OK)	/*If TCP server is not ON return not OK*/
	{
		return V2I_NOT_OK;
	}
	V2I_DoS_Defender();
	if(ESP8266_Check_TCP_Connections_Number(&TCP_connections) != RESPONSE_OK)
	{
		return WIRELESS_TRANSPORT_ERROR;
	}
	if(TCP_connections == 0)
	{
		return V2I_NOT_OK;
	}
	for(uint8_t c=0;c<ESP8266_MAX_TCP_CONNECTIONS;c++)
	{
		if(Enabled_Connections[c] == 1)
		{
			if(Previous_connection[c] == 1)
			{
				continue;	/*This connection is checked before*/
			}
			/*If not, register that it's checked now and return the vehicle ID*/
			Previous_connection[c] = 1;
			*Vehicle_ID = Vehicle_Connection_Correspondence[c];
			return V2I_OK;
		}
	}
	/*No new connections*/
	*Vehicle_ID = 0;
	return V2I_NOT_OK;
}
/************************************************************************************************************************
* Function Name: V2I_Check_AccessPoint
* Function ID[hex]: 0x08
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Execution response
* Description: Function to check if there's any device is connected to my Access Point
************************************************************************************************************************/
V2I_SERVICE_STATE V2I_Check_AccessPoint(uint8_t* connected_vehicles_count)
{
	uint8_t connected_count = 0;
	V2I_DoS_Defender();
	if(ESP8266_Get_Connected_Devices_Number(&connected_count) != RESPONSE_OK)
	{
		return WIRELESS_TRANSPORT_ERROR;
	}
	if(connected_count == 0)
	{
		return V2I_NOT_OK;
	}
	*connected_vehicles_count = connected_count;
	return V2I_OK;
}
/************************************************************************************************************************
* Function Name: V2I_Check_Connected_Vehicles_Number
* Function ID[hex]: 0x09
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): Number of connected vehicles variable by reference
* Parameters (inout): NONE
* Parameters (out): Number of connected vehicles
* Return value: Execution response
* Description: Function to check number of connected vehicles to the infrastructures
************************************************************************************************************************/
V2I_SERVICE_STATE V2I_Check_Connected_Vehicles_Number(uint8_t* Number)
{
	uint8_t counter = 0;
	V2I_DoS_Defender();
	for(uint8_t j = 0;j<ESP8266_MAX_TCP_CONNECTIONS;j++)
	{
		if(Enabled_Connections[j] == 1)
		{
			counter++;
		}
	}
	*Number = counter;
	return V2I_OK;
}
/************************************************************************************************************************
* Function Name: V2I_DoS_Defender
* Function ID[hex]: 0x10
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to execute DoS Defender procedure, if there's any bad connection , close it
* 			   called internally at the start of all functions of the module
************************************************************************************************************************/
static void V2I_DoS_Defender(void)
{
	if(Denial_of_serivce_emergency_flag == 1)
	{
		for(uint8_t z=0;z<ESP8266_MAX_TCP_CONNECTIONS;z++)
		{
			if(Denial_of_service_array[z] == 1)
			{
				ESP8266_Close_TCP_Connection(z);	/*bad connection is closed*/
				Denial_of_service_array[z] = 0;
			}
		}
		Denial_of_serivce_emergency_flag = 0;
	}
}
