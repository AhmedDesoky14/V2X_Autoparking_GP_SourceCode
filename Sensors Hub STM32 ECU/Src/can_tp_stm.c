/************************************************************************************************************************
 * 	Module: CAN Transport Layer for STM32
 * 	File Name: can_tp_stm.c
 *  Authors: Ahmed Desoky
 *	Date: 20/6/2024
 *	*********************************************************************************************************************
 *	Description: CAN Transport Layer Module for STM32, designed and implemented
 *               according to ISO 15765-2 TP, its dependent on CAN module for STM32
 *               This module supports CAN messages only up to 116 bytes
 ***********************************************************************************************************************/
 /***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "can_tp_stm.h"
 /***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
static uint8_t CAN_TP_Initialized = 0; /*Initialization Flag*/
static CAN_Message_Struct CAN_SYSTEM_MESSAGES[NUMBER_OF_RX_IDS];
/***********************************************************************************************************************
 *                     					      PRIVATE FUNCTIONS PROTOTYPES
 ***********************************************************************************************************************/
static void Delay_ms(uint32_t delay_ms);
 /***********************************************************************************************************************
 *                     					        FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: CAN_TP_Init
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Init Status
* Description: Function to Initialize CAN Transport Layer module
************************************************************************************************************************/
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Init(void)
{
    if(CAN_TP_Initialized == 1)
    {
        return CAN_TRANSPORT_OK;  /*Already Initialized*/
    }
    for(uint16_t i=0;i<NUMBER_OF_RX_IDS;i++)
    {
        CAN_SYSTEM_MESSAGES[i].message_id = RxMessageIdList[i];
    }
    CAN_TP_Initialized = 1;
    return CAN_TRANSPORT_OK;
}
/************************************************************************************************************************
* Function Name: CAN_TP_Send
* Function ID[hex]: 0x01
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
* Description: Function splits the given messages into frames and send them over CAN module given Message ID
************************************************************************************************************************/
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Send(uint8_t* Message,uint32_t Message_ID,uint16_t Message_Length,uint16_t Security_Level,uint32_t Device_ID)
{
    unsigned char frame[PAYLOAD_SIZE] = {0};
    unsigned short frame_length = 0;
    unsigned short sequence_number = 0;
    unsigned short offset = 0;
    if(CAN_TP_Initialized == 0)
    {
        return CAN_TRANSPORT_NOT_OK;  /*CAN TP not initialized*/
    }
    if(Message_Length <= PAYLOAD_SIZE-1)
    {
        memset(frame,0,PAYLOAD_SIZE);
        frame[0] = ((SINGLE_FRAME<<4)|(Message_Length & FRAME_LENGTH_MASK));
        memcpy(frame+1,Message,Message_Length);
        Delay_ms(1);
        if(CAN_SendPacket(frame,Message_Length+1,Message_ID) != CAN_BUS_SENDING_SUCCEFULL)
        {
            return CAN_DEVICE_ERROR;
        }
        return CAN_TRANSPORT_OK;
    }
   else
    {
        /*First Frame*/
        memset(frame,0,PAYLOAD_SIZE);
        frame[0] = ((FIRST_FRAME<<4)|((Message_Length>>8) & FRAME_LENGTH_MASK));
        frame[1] = Message_Length & 0xFF;
        frame[2] = ((Security_Level<<4)|(sequence_number & SEQUENCE_MASK));
        frame[3] = (unsigned char)Device_ID;
        memcpy(frame+4,Message,4);
        Delay_ms(1);
        if(CAN_SendPacket(frame,8,Message_ID) != CAN_BUS_SENDING_SUCCEFULL)
        {
            return CAN_DEVICE_ERROR;
        }
        /*Consecutive Frames*/
        sequence_number++;
        offset = 4;
        while(offset < Message_Length)
        {
            memset(frame,0,PAYLOAD_SIZE);
            frame[0] = ((CONSECUTIVE_FRAME<<4)|(sequence_number & SEQUENCE_MASK));
            /*If remaining data greater than 7 then length is 7, if not copy the rest of the message*/
            frame_length = ((Message_Length-offset)>PAYLOAD_SIZE-1)?(PAYLOAD_SIZE-1):(Message_Length-offset);
            memcpy(frame+1,Message+offset,frame_length);
            Delay_ms(1);
            if(CAN_SendPacket(frame,frame_length+1,Message_ID) != CAN_BUS_SENDING_SUCCEFULL)
            {
                return CAN_DEVICE_ERROR;
            }
            offset += frame_length;
            sequence_number++;
        }
    }
    return CAN_TRANSPORT_OK;
}
/************************************************************************************************************************
* Function Name: CAN_TP_Recieve_Callback
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Status of the callback function execution
* Description: STM32 Callback function to receive frames from CAN module and assemble CAN messages
************************************************************************************************************************/
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Recieve_Callback(void)
{
    uint8_t frame[PAYLOAD_SIZE] = {0};
    uint32_t frame_id = 0;
    uint16_t frame_length = 0;
    uint8_t frame_type = 0;
    uint16_t message_index = 0;
    uint16_t index_found = 0;
	for(uint16_t IdIterator = 0;IdIterator<NUMBER_OF_RX_IDS;IdIterator++)
	{
		if(CAN_Check_Inbox(RxMessageIdList[IdIterator]) == CAN_BUS_RECIEVED_CORRECT)/*If there's data*/
		{
			if(CAN_Get_Rx_Packet(frame,&frame_length,RxMessageIdList[IdIterator]) == CAN_BUS_RECIEVED_CORRECT)
			{
				/*Frame/Packet checked and received successfully*/
		        for(uint32_t j=0;j<NUMBER_OF_RX_IDS;j++)
		        {
		            if(CAN_SYSTEM_MESSAGES[j].message_id == frame_id)
		            {
		            	/*Got Object Index*/
		                message_index = j;
		                break;
		            }
		        }
		        frame_type = ((frame[0] & FRAME_TYPE_MASK) >> 4);
		        switch(frame_type)
		        {
		            case SINGLE_FRAME:
		                CAN_SYSTEM_MESSAGES[message_index].received_length = (frame[0] & FRAME_LENGTH_MASK);
		                CAN_SYSTEM_MESSAGES[message_index].message_length = CAN_SYSTEM_MESSAGES[message_index].received_length;
		                CAN_SYSTEM_MESSAGES[message_index].current_sequence_number = 0;
		                CAN_SYSTEM_MESSAGES[message_index].message_security_level = 0;
		                CAN_SYSTEM_MESSAGES[message_index].in_progress = 0;
		                CAN_SYSTEM_MESSAGES[message_index].message_read_flag = 0;
		                memset(CAN_SYSTEM_MESSAGES[message_index].message_data,0,MAX_MESSAGE_SIZE);
		                memcpy(CAN_SYSTEM_MESSAGES[message_index].message_data,frame+1,CAN_SYSTEM_MESSAGES[message_index].message_length);
		                break;
		            case FIRST_FRAME:
                        CAN_SYSTEM_MESSAGES[message_index].message_length = ((frame[0]&FRAME_LENGTH_MASK)<<8) | (frame[1]);
                        CAN_SYSTEM_MESSAGES[message_index].current_sequence_number = (frame[2]&SEQUENCE_MASK);
                        CAN_SYSTEM_MESSAGES[message_index].message_security_level = ((frame[2]&MESSAGE_SECURITY_MASK)>>4);
                        CAN_SYSTEM_MESSAGES[message_index].sending_device_id = frame[3];
                        CAN_SYSTEM_MESSAGES[message_index].received_length = 4;
                        CAN_SYSTEM_MESSAGES[message_index].in_progress = 1;
                        CAN_SYSTEM_MESSAGES[message_index].message_read_flag = 0;
                        memset(CAN_SYSTEM_MESSAGES[message_index].message_data,0,MAX_MESSAGE_SIZE);
                        memcpy(CAN_SYSTEM_MESSAGES[message_index].message_data,frame+4,4); /*received data by then = 4 bytes*/
		                break;
		            case CONSECUTIVE_FRAME:
		                if(CAN_SYSTEM_MESSAGES[message_index].in_progress == 1)
		                {
		                    /*if it's in progress and the expected next sequence number*/
		                    if((frame[0]&SEQUENCE_MASK) == (CAN_SYSTEM_MESSAGES[message_index].current_sequence_number+1))
		                    {
		                        frame_length -= 1;
		                        memcpy(CAN_SYSTEM_MESSAGES[message_index].message_data+CAN_SYSTEM_MESSAGES[message_index].received_length,\
		                        frame+1,frame_length);
		                        CAN_SYSTEM_MESSAGES[message_index].current_sequence_number++;
		                        CAN_SYSTEM_MESSAGES[message_index].received_length += frame_length;
		                        if(CAN_SYSTEM_MESSAGES[message_index].received_length >= CAN_SYSTEM_MESSAGES[message_index].message_length)
		                        {
		                            /*Message received completely*/
		                            CAN_SYSTEM_MESSAGES[message_index].in_progress = 0;
		                        }
		                    }
		                }
		                break;
		        }
			}
		}
    }
}
/************************************************************************************************************************
* Function Name: CAN_TP_Check_Inbox
* Function ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Message ID
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Status of the inbox, whether there's a message or not
* Description: Function to check inbox for a specific Message ID
************************************************************************************************************************/
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Check_Inbox(uint32_t Message_ID)
{
    uint16_t message_index = 0;
    uint16_t index_found = 0;
    if(CAN_TP_Initialized == 0)
    {
        return CAN_TRANSPORT_NOT_OK;  /*CAN TP not initialized*/
    }
    /*Get object index related to the read index*/
    for(uint32_t j=0;j<NUMBER_OF_RX_IDS;j++)
    {
        if(CAN_SYSTEM_MESSAGES[j].message_id == Message_ID)
        {
            message_index = j;
            index_found = 1;
            break;
        }
    }
    if(index_found == 0)
    {
        return CAN_TRANSPORT_NOT_OK;  /*Index out of range*/
    }
    if(CAN_SYSTEM_MESSAGES[message_index].in_progress == 1)
    {
        return CAN_TRANSPORT_NOT_OK;
    }
    if(CAN_SYSTEM_MESSAGES[message_index].message_read_flag == 1)
    {
        /*Message read before*/
        return CAN_TRANSPORT_NOT_OK;
    }
    if(CAN_SYSTEM_MESSAGES[message_index].message_length == 0)
    {
        return CAN_TRANSPORT_NOT_OK;    /*No message found*/
    }
    return CAN_TRANSPORT_OK;
}
/************************************************************************************************************************
* Function Name: CAN_TP_Read
* Function ID[hex]: 0x04
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
* Description: Function to read received and assembled CAN messages given a message ID
************************************************************************************************************************/
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Read(uint8_t* Message,uint32_t Message_ID,uint16_t* Message_Length,uint16_t* Security_Level,uint32_t* Device_ID)
{
	uint16_t message_index = 0;
	uint16_t index_found = 0;
    if(CAN_TP_Initialized == 0)
    {
        return CAN_TRANSPORT_NOT_OK;  /*CAN TP not initialized*/
    }
    /*Get object index related to the read index*/
    for(uint32_t j=0;j<NUMBER_OF_RX_IDS;j++)
    {
        if(CAN_SYSTEM_MESSAGES[j].message_id == Message_ID)
        {
            message_index = j;
            index_found = 1;
            break;
        }
    }
    if(index_found == 0)
    {
        return CAN_TRANSPORT_NOT_OK;  /*Index out of range*/
    }
    if((CAN_SYSTEM_MESSAGES[message_index].in_progress == 1) || (CAN_SYSTEM_MESSAGES[message_index].message_length == 0))
    {
        /*Message still in progress of assembling*/
        return CAN_TRANSPORT_NOT_OK;
    }
    *Security_Level = CAN_SYSTEM_MESSAGES[message_index].message_security_level;
    *Message_Length = CAN_SYSTEM_MESSAGES[message_index].message_length;
    memcpy(Message,CAN_SYSTEM_MESSAGES[message_index].message_data,*Message_Length);
    CAN_SYSTEM_MESSAGES[message_index].message_read_flag = 1; /*Message read*/
    return CAN_TRANSPORT_OK;
}
/************************************************************************************************************************
* Function Name: Delay_ms
* Function ID[hex]: 0x05
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): time of delay in ms
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Private Delay Function
************************************************************************************************************************/
static void Delay_ms(uint32_t delay_ms)
{
	uint32_t SystemClockFreq = HAL_RCC_GetHCLKFreq();
	uint32_t cycles_per_ms = SystemClockFreq / 16000;
	uint32_t total_cycles = cycles_per_ms * delay_ms;
	for (volatile uint32_t i = 0; i < total_cycles; ++i);
	return;
}
