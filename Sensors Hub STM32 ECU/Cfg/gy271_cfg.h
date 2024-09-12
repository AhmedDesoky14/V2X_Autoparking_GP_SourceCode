/************************************************************************************************************************
 * 	Module: GY-271
 * 	File Name: gy271_cfg.h
 *  Authors: Ahmed Desoky
 *	Date: 13/12/2023
 *	*********************************************************************************************************************
 *	Description: Driver Configurations File.
 *				 GY-271 Magnetometer Driver on STM32F103C8T6 Micro-controller.
 *				 Micro-controller Drivers are STM32 HAL Drivers.
 *	*********************************************************************************************************************
 *	This Module is programmed to support (without configuring) as default the following features.
 *	#The module must be placed face up for proper functionality
 *	#Output Datarate = 200Hz
 *	#Mainly created to get yaw angle
 ***********************************************************************************************************************/
#ifndef GY271_CFG_H_
#define GY271_CFG_H_
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
/*Current Version 1.0.0*/
#define GY271_CFG_SW_MAJOR_VERSION           (1U)
#define GY271_CFG_SW_MINOR_VERSION           (0U)
#define GY271_CFG_SW_PATCH_VERSION           (0U)
/************************************************************************************************************************
 *                            			     PRE-BUILD CONFIGURATIONS
 ***********************************************************************************************************************/
#define GY271RANGE							0		/*0:2G(12000 LSB/G), 1:8G(3000 LSB/G)*/
#define INDICATOR_LED_GY271_PORT			GPIOA	/*a Led Indicator for initialization - LED PORT*/
#define	INDICATOR_LED_GY271_PIN				GPIO_PIN_5
#define	GY271_INTERNAL_DIGITAL_LPF_VALUE	0	/*(0 to 3) to know what exact value to use, refer to registers map included*/
/************************************************************************************************************************
 *                            	       STM HAL CUBEX Configurations Notes
 ***********************************************************************************************************************/
/* -Enable any I2C Peripheral and configure it either in normal mode or fast mode, both are fine
 * -when using init function just pass the I2C typestruct by reference
 */
#endif /*GY271_CFG_H_*/
