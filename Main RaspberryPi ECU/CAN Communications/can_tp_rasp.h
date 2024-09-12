/************************************************************************************************************************
 * 	Module: CAN Transport Layer for Raspberry Pi
 * 	File Name: can_tp_rasp.h
 *  Authors: Ahmed Desoky
 *	Date: 19/6/2024
 *	*********************************************************************************************************************
 *	Description: CAN Transport Layer Module for Raspberry Pi and Linux, designed and implemented
 *               according to ISO 15765-2 TP, its dependent on CAN module for Raspberry Pi
 *               This module supports CAN messages only up to 116 bytes
 ***********************************************************************************************************************/
#ifndef CAN_TP_H
#define CAN_TP_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "can_rasp.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.1.0*/
#define CAN_TP_SW_MAJOR_VERSION           (1U)
#define CAN_TP_SW_MINOR_VERSION           (1U)
#define CAN_TP_SW_PATCH_VERSION           (0U)

/*Software Version checking of Dependent Module - CAN Raspberry Pi Module - Version 2.0.0*/
#if ((CAN_SW_MAJOR_VERSION != (2U))\
 ||  (CAN_SW_MINOR_VERSION != (0U))\
 ||  (CAN_SW_PATCH_VERSION != (0U)))
  #error "The Software version of CAN Raspberry Pi Module does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define MAX_MESSAGE_SIZE            128
#define SINGLE_FRAME                0x0
#define FIRST_FRAME                 0x1
#define CONSECUTIVE_FRAME           0x2
#define FRAME_LENGTH_MASK           0x0F
#define SEQUENCE_MASK               0x0F
#define FRAME_TYPE_MASK             0xF0
#define MESSAGE_SECURITY_MASK       0xF0
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
/*enum for CAN TP module functions status*/
typedef enum
{
  CAN_TRANSPORT_OK,CAN_TRANSPORT_NOT_OK,CAN_DEVICE_ERROR
}CAN_TRANSPORT_LAYER_STATUS;

/*Struct to store data of each CAN message*/
typedef struct
{
  unsigned int message_id;
  unsigned short message_security_level;
  unsigned short message_length;
  unsigned short received_length;
  unsigned short current_sequence_number;
  unsigned char in_progress;
  unsigned char message_read_flag;
  unsigned char sending_device_id;
  unsigned char message_data[MAX_MESSAGE_SIZE];
}CAN_Message_Struct;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Init(void);
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Send(unsigned char* Message,unsigned int Message_ID,unsigned short Message_Length,unsigned short Security_Level,unsigned int Device_ID);
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Check_Inbox(unsigned int Message_ID);
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Read(unsigned char* Message,unsigned int Message_ID,unsigned short* Message_Length,unsigned short* Security_Level,unsigned int* Device_ID);
/************************************************************************************************************************
 *                    							   External Variables
 ***********************************************************************************************************************/
extern int receive_ready; /*shared resource variable for thread signaling*/
extern pthread_mutex_t receive_mutex;  /*Mutex for upper layer thread signaling*/
extern pthread_cond_t receive_thread_condition; /*Condition variable for the same reason*/

#endif /*CAN_TP_H*/
