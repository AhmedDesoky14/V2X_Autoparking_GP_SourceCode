/************************************************************************************************************************
 * 	Module: Light System
 * 	File Name: light_system.h
 *  Authors: Ahmed Desoky
 *	Date: 20/7/2024
 *	*********************************************************************************************************************
 *	Description: Module to operate the vehicle lighing system seamlessly
 *				 All pins out are configured as the lights are +ve logic
 *				 pins out are defined in the configurations file
 ***********************************************************************************************************************/
#ifndef LIGHT_SYSTEM_H
#define LIGHT_SYSTEM_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "stm32f1xx_hal.h"
#include "light_system_cfg.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.0.0*/
#define LIGHT_SYSTEM_SW_MAJOR_VERSION           (1U)
#define LIGHT_SYSTEM_SW_MINOR_VERSION           (0U)
#define LIGHT_SYSTEM_SW_PATCH_VERSION           (0U)
/*Software Version checking between Module Configuration file and Header file*/
#if ((LIGHT_SYSTEM_SW_MAJOR_VERSION != LIGHT_SYSTEM_CFG_SW_MAJOR_VERSION)\
 ||  (LIGHT_SYSTEM_SW_MINOR_VERSION != LIGHT_SYSTEM_CFG_SW_MINOR_VERSION)\
 ||  (LIGHT_SYSTEM_SW_PATCH_VERSION != LIGHT_SYSTEM_CFG_SW_PATCH_VERSION))
  #error "The Software version of Light System does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
/*enum for different lights in the system*/
typedef enum
{
	Front_Light_H,
	Front_Light_L,
	Back_Light_H,
	Back_Light_L,
	Right_Turning,
	Left_Turning,
	Awaiting,
	Interior_Light1,
	Interior_Light2,
	Interior_Light3
}Light_Type;
/*enum for light types state*/
typedef enum
{
	Flag_Low,Flag_High
}Light_State;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
void Light_System_Update(void);
void Light_System_Set_State(Light_Type Light,Light_State State);
void Light_System_Get_State(Light_Type Light,Light_State* State);
void Light_System_Toggle_State(Light_Type Light);
#endif /*LIGHT_SYSTEM_H*/
