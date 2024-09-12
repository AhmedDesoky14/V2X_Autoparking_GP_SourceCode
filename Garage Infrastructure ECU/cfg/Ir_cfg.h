/************************************************************************************************************************
 * 	Module:     IR Sensor
 * 	File Name:  Ir_cfg.h
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
 ***********************************************************************************************************************/

#ifndef INC_IR_CFG_H_
#define INC_IR_CFG_H_
#include "stm32f4xx_hal.h"
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
/*Current Version 1.0.0*/
#define IR_CFG_SW_MAJOR_VERSION           (1U)
#define IR_CFG_SW_MINOR_VERSION           (0U)
#define IR_CFG_SW_PATCH_VERSION           (0U)
/************************************************************************************************************************
 *                            			     PRE-BUILD CONFIGURATIONS
 ***********************************************************************************************************************/
#define IR_NUMBERS                           4
/*#define LED_PORT                           GPIOB
#define LED1_PIN                             GPIO_PIN_13
#define LED2_PIN                             GPIO_PIN_14
#define LED3_PIN                             GPIO_PIN_15*/



/************************************************************************************************************************
 *                            	       STM HAL CUBEX Configurations Notes
 ***********************************************************************************************************************/
/*(1) configure any pin as GPIO_input (in case of connecting 4 Ir sensors as my case
 * configure 4 pins as gpio_inputs) with adjusting clock frequency 8MHZ or any desired frequency.
 *(2)configure any pin as GPIO_output in case of connecting a led indicator to check if
 * a parking spot is occupied or not
 */

#endif /* INC_IR_CFG_H_ */
