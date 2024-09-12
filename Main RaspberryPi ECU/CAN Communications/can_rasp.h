/************************************************************************************************************************
 *  Module: CAN Raspberry
 *  File Name: can.h
 *  Authors: Ahmed Desoky
 *	Date: 25/4/2024
 *	*********************************************************************************************************************
 *	Description: CAN Raspberry Pi module to control CAN on Raspberry Pi using MCP2515 CAN controller interface
 *               This module only works if can-utils is installed and can interface is verified
 *               To check CAN interface, run "ip link show" shell command
 *               Before use, run "sudo ip link set (interface name) down" shell command
 *               This module supports receiving CAN frames, single frames and 
 *               contains software frames IDs filter, please check can_PBcfg.c
 *               To know how to install can-utils to enable this module functionality, please check can_cfg.h
 *               for building, link to pthread, add -lpthread option, and build as super user
 ***********************************************************************************************************************/
#ifndef CAN_H
#define CAN_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>
#include "can_rasp_cfg.h"
#include "leds.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 2.0.0*/
#define CAN_SW_MAJOR_VERSION           (2U)
#define CAN_SW_MINOR_VERSION           (0U)
#define CAN_SW_PATCH_VERSION           (0U)
/*Software Version checking between Module Configuration file and Header file*/
#if ((CAN_SW_MAJOR_VERSION != CAN_CFG_SW_MAJOR_VERSION)\
 ||  (CAN_SW_MINOR_VERSION != CAN_CFG_SW_MINOR_VERSION)\
 ||  (CAN_SW_PATCH_VERSION != CAN_CFG_SW_PATCH_VERSION))
  #error "The Software version of CAN Raspberry does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define MAX_PAYLOAD   8
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
/*enumeration to specify can status*/
typedef enum
{
  CAN_OK,CAN_NOT_OK,CAN_ERROR
}CAN_STATUS;

/*typedef for initialization structure to hold CAN interface name and configured Bitrate*/
typedef struct
{
  char CAN_Interface_Name[10];
  unsigned int Bitrate;
}CAN_INIT_CONFIG;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
CAN_STATUS CAN_Init(const CAN_INIT_CONFIG* CAN_Configuration);
CAN_STATUS CAN_Send(unsigned int Message_ID,unsigned char* Message_Frame,unsigned short Message_Frame_Size);
CAN_STATUS CAN_Check_Inbox(void);
CAN_STATUS CAN_Read(unsigned int* Message_ID,unsigned char* Message,unsigned short* Message_Size);
/************************************************************************************************************************
 *                    							   External Variables
 ***********************************************************************************************************************/
extern const CAN_INIT_CONFIG CAN_Configuration; /*Initialization Object of CAN*/
extern const unsigned int RxMessageIDList[NUMBER_OF_RX_IDS];
#endif /*CAN_H*/