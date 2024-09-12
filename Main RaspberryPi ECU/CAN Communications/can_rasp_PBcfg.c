/************************************************************************************************************************
 *  Module: CAN Raspberry
 *  File Name: can_PBcfg.c
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
/************************************************************************************************************************
 *                     							         INCLUDES
 ***********************************************************************************************************************/
#include "can_rasp.h"
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 2.0.0*/
#define CAN_PBCFG_SW_MAJOR_VERSION           (2U)
#define CAN_PBCFG_SW_MINOR_VERSION           (0U)
#define CAN_PBCFG_SW_PATCH_VERSION           (0U)
/*Software Version checking between Module Post Build Configuration file and Header file*/
#if ((CAN_SW_MAJOR_VERSION != CAN_PBCFG_SW_MAJOR_VERSION)\
 ||  (CAN_SW_MINOR_VERSION != CAN_PBCFG_SW_MINOR_VERSION)\
 ||  (CAN_SW_PATCH_VERSION != CAN_PBCFG_SW_PATCH_VERSION))
  #error "The Software version of CAN Raspberry does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                            			          CONFIGURATIONS
 ***********************************************************************************************************************/
const CAN_INIT_CONFIG CAN_Configuration = {"can0",500000};
const unsigned int RxMessageIDList[NUMBER_OF_RX_IDS] = {0x01,0x02,0x03,0x04,0x05};
