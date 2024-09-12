/************************************************************************************************************************
 * 	Module: CAN Transport Layer for STM32
 * 	File Name: can_tp_stm.h
 *  Authors: Ahmed Desoky
 *	Date: 20/6/2024
 *	*********************************************************************************************************************
 *	Description: CAN Transport Layer Module for STM32, designed and implemented
 *               according to ISO 15765-2 TP, its dependent on CAN module for STM32
 *               This module supports CAN messages only up to 116 bytes
 ***********************************************************************************************************************/
#ifndef CAN_TP_STM_H
#define CAN_TP_STM_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "can_stm.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.1.0*/
#define CAN_TP_SW_MAJOR_VERSION           (1U)
#define CAN_TP_SW_MINOR_VERSION           (1U)
#define CAN_TP_SW_PATCH_VERSION           (0U)

/*Software Version checking of Dependent Module - CAN Module - Version 1.0.2*/
#if ((CAN_BUS_SW_MAJOR_VERSION != (2U))\
 ||  (CAN_BUS_SW_MINOR_VERSION != (0U))\
 ||  (CAN_BUS_SW_PATCH_VERSION != (0U)))
  #error "The Software version of CAN Module does not match the configurations expected version"
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
/*enum to describe status of the functions	*/
typedef enum
{
  CAN_TRANSPORT_OK,CAN_TRANSPORT_NOT_OK,CAN_DEVICE_ERROR
}CAN_TRANSPORT_LAYER_STATUS;

/*Struct to store data of each CAN message*/
typedef struct
{
  uint32_t message_id;
  uint8_t sending_device_id;
  uint16_t message_security_level;
  uint16_t message_length;
  uint16_t received_length;
  uint16_t current_sequence_number;
  uint8_t in_progress;
  uint8_t message_read_flag;
  uint8_t message_data[MAX_MESSAGE_SIZE];
}CAN_Message_Struct;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Init(void);
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Send(uint8_t* Message,uint32_t Message_ID,uint16_t Message_Length,uint16_t Security_Level,uint32_t Device_ID);
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Recieve_Callback(void);
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Check_Inbox(uint32_t Message_ID);
CAN_TRANSPORT_LAYER_STATUS CAN_TP_Read(uint8_t* Message,uint32_t Message_ID,uint16_t* Message_Length,uint16_t* Security_Level,uint32_t* Device_ID);
#endif /*CAN_TP_STM_H*/
