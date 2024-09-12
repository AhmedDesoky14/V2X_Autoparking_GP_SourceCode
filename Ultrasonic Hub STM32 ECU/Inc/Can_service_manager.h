/************************************************************************************************************************
 * 	Module: CAN Service Manager
 * 	File Name: can_service_manager.h
 *  Authors: Ahmed Desoky
 *	Date: 20/6/2024
 *	*********************************************************************************************************************
 *	Description: CAN Service Manager module that utilizes SecOC and CAN TP modules to send
                 and receive CAN messages seamlessly.
                 This Module is hardware independent and only depends on SecOC and CAN TP modules.
 ***********************************************************************************************************************/
#ifndef CAN_SM_H
#define CAN_SM_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "can_tp_stm.h"
#include "SecOC.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.0.0*/
#define CAN_SM_SW_MAJOR_VERSION           (1U)
#define CAN_SM_SW_MINOR_VERSION           (0U)
#define CAN_SM_SW_PATCH_VERSION           (0U)

/*Software Version checking of Dependent Module - CAN Transport Layer - Version 1.1.0*/
#if ((CAN_TP_SW_MAJOR_VERSION != (1U))\
 ||  (CAN_TP_SW_MINOR_VERSION != (1U))\
 ||  (CAN_TP_SW_PATCH_VERSION != (0U)))
  #error "The Software version of CAN Transport Layer Module does not match the configurations expected version"
#endif

/*Software Version checking of Dependent Module - SecOC - Version 1.1.1*/
#if ((SECOC_SW_MAJOR_VERSION != (1U))\
 ||  (SECOC_SW_MINOR_VERSION != (1U))\
 ||  (SECOC_SW_PATCH_VERSION != (1U)))
  #error "The Software version of SecOC Module does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define SECURE_COMM   1
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
typedef enum
{
  CAN_SM_OK,CAN_SM_NOT_OK
}CAN_SM_STATUS;
/************************************************************************************************************************
 *                     						                MACROS
 ***********************************************************************************************************************/

/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
CAN_SM_STATUS CAN_SM_Send(unsigned char* Message,unsigned int Message_ID,\
                unsigned int Message_Length,SecurityLevel Security_Level,unsigned int Device_ID);
CAN_SM_STATUS CAN_SM_Check_Inbox(unsigned int Message_ID);
CAN_SM_STATUS CAN_SM_Read(unsigned char* Message,unsigned int Message_ID,\
                unsigned int* Message_Length,SecurityLevel* Security_Level,unsigned int* Device_ID);
#endif /*CAN_SM_H*/
