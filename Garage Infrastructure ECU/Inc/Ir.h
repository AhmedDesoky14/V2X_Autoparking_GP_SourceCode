/************************************************************************************************************************
 * 	Module:     IR Sensor
 * 	File Name:  Ir.h
 *  Authors:    Dina Hamed
 *	Date:       Jan 30, 2024
 *	*********************************************************************************************************************
 *	Description: Driver Header File.
 *				 IR sensor Driver on STM32F103C8T6 Micro-controller.
 *				 Micro-controller Drivers are STM32 HAL Drivers.
 *	this module detects and handles events triggered by the IR sensor,
 *	such as the detection of objects or changes in infrared radiation levels.
 *
 *
 ***********************************************************************************************************************/
#ifndef INC_IR_H_
#define INC_IR_H_
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "Ir_cfg.h"

/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.0.0*/
#define IR_SW_MAJOR_VERSION           (1U)
#define IR_SW_MINOR_VERSION           (0U)
#define IR_SW_PATCH_VERSION           (0U)
/*Software Version checking between IR Configuration file and Header file*/
#if ((IR_SW_MAJOR_VERSION != IR_CFG_SW_MAJOR_VERSION)\
 ||  (IR_SW_MINOR_VERSION != IR_CFG_SW_MINOR_VERSION)\
 ||  (IR_SW_PATCH_VERSION != IR_CFG_SW_PATCH_VERSION))
  #error "The Software version of IR does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
typedef enum
{
	AVAILABLE_SLOT,NOT_AVAILABLE_SLOT
} slot_state;

/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/

slot_state IR_ReadData(int id);  /*Function to read data from an infrared (IR) sensor */
void isOccupied(int array_id);

#endif /* INC_IR_H_ */
