/************************************************************************************************************************
 * 	Module: CAN Bus
 * 	File Name: CAN_Bus_cfg
 *  Authors: Ziad Emad
 *	Date: 28/2/2024
 *	*********************************************************************************************************************
 *	Description:
 *	Implementation of CAN Driver
 *	TO BE CONTIUNED
 *
 *

/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "can_stm.h"
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 2.0.0*/
#define CAN_BUS_PBCFG_SW_MAJOR_VERSION           (2U)
#define CAN_BUS_PBCFG_SW_MINOR_VERSION           (0U)
#define CAN_BUS_PBCFG_SW_PATCH_VERSION           (0U)
/*Software Version checking between Module Post Build Configuration file and Header file*/
#if ((CAN_BUS_SW_MAJOR_VERSION != CAN_BUS_PBCFG_SW_MAJOR_VERSION)\
 ||  (CAN_BUS_SW_MINOR_VERSION != CAN_BUS_PBCFG_SW_MINOR_VERSION)\
 ||  (CAN_BUS_SW_PATCH_VERSION != CAN_BUS_PBCFG_SW_PATCH_VERSION))
  #error "The Software version of CAN module does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                            			     PRE-BUILD CONFIGURATIONS
 ***********************************************************************************************************************/
CAN_TxHeader CAN_TxHeaderParamters =
{		0x4, 			// STD_ID
		0,				//Ext_ID
		CAN_ID_STD,
		CAN_RTR_DATA,	//RTR
		8,  			// DLC
		DISABLE 		//TransmitGlobalTime
};

CAN_Filter CAN_FilterParamters =
{		10, 					//FilterBank
		0x7<<5, 				//FilterIdHigh
		0x0000,				//FilterIdLow
		0x4<<5,				//FilterMaskIdHigh
		0x0000	,				//FilterMaskIdLow
		CAN_RX_FIFO0,			//FilterFIFOAssignment
		CAN_FILTERMODE_IDMASK,	//FilterMode
		CAN_FILTERSCALE_32BIT,	//FilterScale
		CAN_FILTER_ENABLE       //FilterActivation
};

uint32_t RxMessageIdList[NUMBER_OF_RX_IDS]= {0x4,0x5,0x6,0x7,0x8};
/************************************************************************************************************************
 *                            	       STM HAL CubeMX Configurations Notes
 ***********************************************************************************************************************/
/*
 *
 *
 *
 */


