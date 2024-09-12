/************************************************************************************************************************
 * 	Module: Button Module
 * 	File Name: button.h
 *  Authors: Ahmed Desoky
 *	Date: 23/6/2024
 *	*********************************************************************************************************************
 *	Description: This simple driver for push button
 ***********************************************************************************************************************/
#ifndef BUTTON_H
#define BUTTON_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "button_cfg.h"
#include "stm32f4xx_hal.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.0.0*/
#define BUTTON_SW_MAJOR_VERSION           (1U)
#define BUTTON_SW_MINOR_VERSION           (0U)
#define BUTTON_SW_PATCH_VERSION           (0U)
/*Software Version checking between Module Configuration file and Header file*/
#if ((BUTTON_SW_MAJOR_VERSION != BUTTON_CFG_SW_MAJOR_VERSION)\
 ||  (BUTTON_SW_MINOR_VERSION != BUTTON_CFG_SW_MINOR_VERSION)\
 ||  (BUTTON_SW_PATCH_VERSION != BUTTON_CFG_SW_PATCH_VERSION))
  #error "The Software version of Button does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
#if(INTERRUPT_MODE == 0)
typedef enum
{
	BUTTON_PUSHED,BUTTON_RELEASED
}BUTTON_STATUS;
#endif
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
#if(INTERRUPT_MODE == 0)
BUTTON_STATUS Button_Read(void);
#elif(INTERRUPT_MODE == 1)
void Button_Set_CallBack_Function(void(*Function_Ptr)(void));
void Button_CallBack_Function(void);
#endif

#endif /*BUTTON_H*/
