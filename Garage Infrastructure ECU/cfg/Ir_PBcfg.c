/************************************************************************************************************************
 * 	Module:     IR Sensor
 * 	File Name:  Ir_PBcfg.C
 *  Authors:    Dina Hamed
 *	Date:       Jan 30, 2024
 *	*********************************************************************************************************************
 *	Description: Driver Configurations File.
 *				 IR sensor Driver on STM32F103C8T6 Micro-controller.
 *				 Micro-controller Drivers are STM32 HAL Drivers.
 *	this module detects and handles events triggered by the IR sensor,
 *	such as the detection of objects or changes in infrared radiation levels.
 *
 *
 ***********************************************************************************************************************//***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "Ir.h"
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.0.0*/
#define IR_PBCFG_SW_MAJOR_VERSION           (1U)
#define IR_PBCFG_SW_MINOR_VERSION           (0U)
#define IR_PBCFG_SW_PATCH_VERSION           (0U)
/*Software Version checking between IR Module Post Build Configuration file and Header file*/
#if ((IR_SW_MAJOR_VERSION != IR_PBCFG_SW_MAJOR_VERSION)\
 ||  (IR_SW_MINOR_VERSION != IR_PBCFG_SW_MINOR_VERSION)\
 ||  (IR_SW_PATCH_VERSION != IR_PBCFG_SW_PATCH_VERSION))
  #error "The Software version of (MODULE NAME) does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                            			          CONFIGURATIONS
 ***********************************************************************************************************************/
/* 2 Arrays of GPIO PIN & PORT configurations for the IR sensor(s).  */
uint8_t IR_Pins[IR_NUMBERS]={GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_10};
GPIO_TypeDef *IR_Ports[IR_NUMBERS] = {GPIOB, GPIOB, GPIOB, GPIOB};
uint16_t LED_Pins[IR_NUMBERS]={GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};
GPIO_TypeDef *LED_Ports[IR_NUMBERS] = {GPIOA, GPIOA, GPIOA, GPIOA};


