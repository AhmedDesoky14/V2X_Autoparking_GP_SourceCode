/************************************************************************************************************************
 * 	Module: CAN Transport Layer for Raspberry Pi
 * 	File Name: can_tp_rasp.c
 *  Authors: Ahmed Desoky
 *	Date: 19/6/2024
 *	*********************************************************************************************************************
 *	Description: CAN Transport Layer Module for Raspberry Pi and Linux, designed and implemented
 *               according to ISO 15765-2 TP, its dependent on CAN module for Raspberry Pi
 *               This module supports CAN messages only up to 116 bytes
 ***********************************************************************************************************************/
 /***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "can_tp_rasp.h"
 /***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
static int CAN_TP_Initialized = 0; /*Initialization Flag*/
static pthread_t CAN_TP_Thread; /*Thread PID variable*/
static CAN_Message_Struct CAN_SYSTEM_MESSAGES[NUMBER_OF_RX_IDS];
 /***********************************************************************************************************************
 *                     					      PRIVATE FUNCTIONS PROTOTYPES
 ***********************************************************************************************************************/
static void *CAN_TP_Receive_Thread(void*);
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
* Description: Function to Initialize CAN Transport Layer moduel to start receiving and message assembly
************************************************************************************************************************/
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Init(void)
{
    if(CAN_TP_Initialized == 1)
    {
        return CAN_TRANSPORT_OK;  /*Already Initialized*/
    }
    static pthread_attr_t Thread_Attributes; /*Thread attributes to configure*/
    struct sched_param Scheduling_Priority;
     /*store all ids in the structure objects to identify each one*/
    for(int i=0;i<NUMBER_OF_RX_IDS;i++)
    {
        CAN_SYSTEM_MESSAGES[i].message_id = RxMessageIDList[i];
    }
    pthread_attr_init(&Thread_Attributes);   /*init attributes*/
    pthread_attr_setinheritsched(&Thread_Attributes,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&Thread_Attributes,SCHED_FIFO);
    Scheduling_Priority.sched_priority = 32;/*highest priority possible for the scheduling policy meeting POSIX standard*/
    pthread_attr_setschedparam(&Thread_Attributes,&Scheduling_Priority);
    if(pthread_create(&CAN_TP_Thread,&Thread_Attributes,&CAN_TP_Receive_Thread,NULL) != 0)
    {
        return CAN_TRANSPORT_NOT_OK;
    }
    pthread_attr_destroy(&Thread_Attributes);
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
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Send(unsigned char* Message,unsigned int Message_ID,unsigned short Message_Length,unsigned short Security_Level,unsigned int Device_ID)
{
    unsigned char frame[MAX_PAYLOAD] = {0};
    unsigned short frame_length = 0;
    unsigned short sequence_number = 0;
    unsigned short offset = 0;
    if(CAN_TP_Initialized == 0)
    {
        return CAN_TRANSPORT_NOT_OK;  /*CAN TP not initialized*/
    }
    if(Message_Length <= MAX_PAYLOAD-1)
    {
        memset(frame,0,MAX_PAYLOAD);
        frame[0] = ((SINGLE_FRAME<<4)|(Message_Length & FRAME_LENGTH_MASK));
        memcpy(frame+1,Message,Message_Length);
        if(CAN_Send(Message_ID,frame,Message_Length+1) != CAN_OK)
        {
            return CAN_DEVICE_ERROR;
        }
        return CAN_TRANSPORT_OK;
    }
    else
    {
        /*First Frame*/
        memset(frame,0,MAX_PAYLOAD);
        frame[0] = ((FIRST_FRAME<<4)|((Message_Length>>8) & FRAME_LENGTH_MASK));
        frame[1] = Message_Length & 0xFF;
        frame[2] = ((Security_Level<<4)|(sequence_number & SEQUENCE_MASK));
        frame[3] = (unsigned char)Device_ID;
        memcpy(frame+4,Message,4);
        if(CAN_Send(Message_ID,frame,8) != CAN_OK)
        {
            return CAN_DEVICE_ERROR;
        }
        /*Consecutive Frames*/
        sequence_number++;
        offset = 4;
        while(offset < Message_Length)
        {
            memset(frame,0,MAX_PAYLOAD);
            frame[0] = ((CONSECUTIVE_FRAME<<4)|(sequence_number & SEQUENCE_MASK));
            /*If remaining data greater than 7 then length is 7, if not copy the rest of the message*/
            frame_length = ((Message_Length-offset)>MAX_PAYLOAD-1)?(MAX_PAYLOAD-1):(Message_Length-offset);
            memcpy(frame+1,Message+offset,frame_length);
            usleep(1000);    /*1ms recover*/
            if(CAN_Send(Message_ID,frame,frame_length+1) != CAN_OK)
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
* Function Name: CAN_TP_Receive_Thread
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Thread to receive any incoming CAN frame and assemble frames to CAN messages
************************************************************************************************************************/
static void *CAN_TP_Receive_Thread(void* v)
{
    unsigned char frame[MAX_PAYLOAD] = {0};
    unsigned int frame_id = 0;
    unsigned short frame_length = 0;
    unsigned char frame_type = 0;
    unsigned short message_index = 0;
    while(1)
    {
        /*Thread Block until signaled*/
        pthread_mutex_lock(&receive_mutex);    /*Lock Mutex to ensure cond wait before signaling*/
        while(receive_ready == 0)   /*in while loop on this condition to prevent what is called spurious wake ups*/
        {
            pthread_cond_wait(&receive_thread_condition,&receive_mutex);
        }
        receive_ready = 0;
        pthread_mutex_unlock(&receive_mutex);   /*Unlock mutex*/
        if(CAN_Check_Inbox() != CAN_OK)
        {
            continue;
        }
        if(CAN_Read(&frame_id,frame,&frame_length) != CAN_OK)
        {
            continue;   
        }
        /*Get object index related to the read index*/
        for(int j=0;j<NUMBER_OF_RX_IDS;j++)
        {
            if(CAN_SYSTEM_MESSAGES[j].message_id == frame_id)
            {
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
                        //printf("Received length until now %d\n",CAN_SYSTEM_MESSAGES[message_index].received_length);
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
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Check_Inbox(unsigned int Message_ID)
{
    unsigned short message_index = 0;
    unsigned short index_found = 0;
    if(CAN_TP_Initialized == 0)
    {
        return CAN_TRANSPORT_NOT_OK;  /*CAN TP not initialized*/
    }
    /*Get object index related to the read index*/
    for(int j=0;j<NUMBER_OF_RX_IDS;j++)
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
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Read(unsigned char* Message,unsigned int Message_ID,unsigned short* Message_Length,unsigned short* Security_Level,unsigned int* Device_ID)
{
    unsigned short message_index = 0;
    unsigned short index_found = 0;
    if(CAN_TP_Initialized == 0)
    {
        return CAN_TRANSPORT_NOT_OK;  /*CAN TP not initialized*/
    }
    /*Get object index related to the read index*/
    for(int j=0;j<NUMBER_OF_RX_IDS;j++)
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
    *Device_ID = (unsigned int)CAN_SYSTEM_MESSAGES[message_index].sending_device_id;
    *Security_Level = CAN_SYSTEM_MESSAGES[message_index].message_security_level;
    *Message_Length = CAN_SYSTEM_MESSAGES[message_index].message_length;
    memcpy(Message,CAN_SYSTEM_MESSAGES[message_index].message_data,*Message_Length);
    CAN_SYSTEM_MESSAGES[message_index].message_read_flag = 1; /*Message read*/
    return CAN_TRANSPORT_OK;
}