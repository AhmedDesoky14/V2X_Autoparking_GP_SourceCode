/************************************************************************************************************************
 * 	Module: CAN Service Manager
 * 	File Name: can_service_manager.c
 *  Authors: Ahmed Desoky
 *	Date: 20/6/2024
 *	*********************************************************************************************************************
 *	Description: CAN Service Manager module that utilizes SecOC and CAN TP modules to send
                 and receive CAN messages seamlessly.
                 This Module is hardware independent and only depends on SecOC and CAN TP modules.
 ***********************************************************************************************************************/
 /***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "can_service_manager.h"
 /***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
unsigned char Tx_Cipher[MAX_MESSAGE_SIZE] = {0};
unsigned char Rx_Cipher[MAX_MESSAGE_SIZE] = {0};
 /***********************************************************************************************************************
 *                     					        FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: CAN_SM_Send
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Message to be sent
                   Message ID
                   Message Length
                   Security Needed for the message
                   Sender Device ID
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Status of function execution
* Description: Function to secure the given message with the given security level then send it
*              over CAN bus with the given message ID
************************************************************************************************************************/
CAN_SM_STATUS CAN_SM_Send(unsigned char* Message,unsigned int Message_ID,\
                unsigned int Message_Length,SecurityLevel Security_Level,unsigned int Device_ID)
{
    unsigned char Cipher_Text[MAX_MESSAGE_SIZE] = {0};
    unsigned int Cipher_length;
#if(SECURE_COMM == 1)
    /*Cyber Security Start*/
    if(SecOC_Send_Secured((byte*)Message,Message_Length,(byte*)Cipher_Text,(unsigned int*)&Cipher_length,Security_Level,Device_ID) != SECURITY_OK)
    {
        return CAN_SM_NOT_OK;
    }
    /*Cyber Security End*/
    memcpy(Tx_Cipher,Cipher_Text,MAX_MESSAGE_SIZE);
    if(CAN_TP_Send(Cipher_Text,Message_ID,(unsigned short)Cipher_length,(unsigned short)Security_Level,Device_ID) != CAN_TRANSPORT_OK)
    {
        return CAN_SM_NOT_OK;
    }
#else
    if(CAN_TP_Send(Message,Message_ID,(unsigned short)Message_Length,(unsigned short)Security_Level,Device_ID) != CAN_TRANSPORT_OK)
    {
        return CAN_SM_NOT_OK;
    }
#endif
    return CAN_SM_OK;
}
/************************************************************************************************************************
* Function Name: CAN_SM_Check_Inbox
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Message ID
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Status of the inbox, whether there's a message or not
* Description: Function to check inbox for a specific Message ID
************************************************************************************************************************/
CAN_SM_STATUS CAN_SM_Check_Inbox(unsigned int Message_ID)
{
    if(CAN_TP_Check_Inbox(Message_ID) != CAN_TRANSPORT_OK)
    {
        return CAN_SM_NOT_OK;
    }
    return CAN_SM_OK;
}
/************************************************************************************************************************
* Function Name: CAN_SM_Read
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Message ID
* Parameters (inout): Pointer to message
                      Pointer to message length
                      Pointer to message security level
                      Pointer to the sender device ID
* Parameters (out): Message
                    Message Length
                    Message Security Level
                    Sender Device ID
* Return value: Status of function execution
* Description: Function to read received, secured and assembled CAN messages given a message ID
************************************************************************************************************************/
CAN_SM_STATUS CAN_SM_Read(unsigned char* Message,unsigned int Message_ID,\
                unsigned int* Message_Length,SecurityLevel* Security_Level,unsigned int* Device_ID)
{
    unsigned char Rx_Message[MAX_MESSAGE_SIZE] = {0};
    unsigned int Rx_Message_Length = 0;
    unsigned short Msg_Security_Level = 0;
#if(SECURE_COMM == 1)
    if(CAN_TP_Read(Rx_Message,Message_ID,(unsigned short*)&Rx_Message_Length,&Msg_Security_Level,Device_ID) != CAN_TRANSPORT_OK)
    {
        return CAN_SM_NOT_OK;
    }
    /*Cyber Security Start*/
    *Security_Level = (SecurityLevel)Msg_Security_Level;
    memcpy(Rx_Cipher,Rx_Message,MAX_MESSAGE_SIZE);
    if(SecOC_Recieve_Secured((byte*)Rx_Message,Rx_Message_Length,Message,(unsigned int*)Message_Length,*Security_Level,*Device_ID) != SECURITY_OK)
    {
        return CAN_SM_NOT_OK;
    }
    /*Cyber Security End*/
#else
    if(CAN_TP_Read(Message,Message_ID,(unsigned short*)Message_Length,&Msg_Security_Level,Device_ID) != CAN_TRANSPORT_OK)
    {
        return CAN_SM_NOT_OK;
    }
    *Security_Level = (SecurityLevel)Msg_Security_Level;
#endif
    return CAN_SM_OK;
}
