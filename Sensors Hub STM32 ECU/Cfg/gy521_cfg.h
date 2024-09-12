/************************************************************************************************************************
 * 	Module: GY-521
 * 	File Name: gy521_cfg.h
 *  Authors: Ahmed Desoky
 *	Date: 27/10/2023
 *	*********************************************************************************************************************
 *	Description: Driver Configurations File.
 *				 GY-521 IMU Driver on STM32F103C8T6 Micro-controller.
 *				 Micro-controller Drivers are STM32 HAL Drivers.
 *	*********************************************************************************************************************
 *	This Module is programmed to support (without configuring) as default the following features.
 *	#Sampling Rate = 1KHz.
 *	#This Driver uses I2C for memory write/read in blocking mode.
 *	#The module must be placed face up for proper functionality
 ***********************************************************************************************************************/
#ifndef GY521_CFG_H_
#define GY521_CFG_H_
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
/*Current Version 1.2.3*/
#define GY521_CFG_SW_MAJOR_VERSION         	 (1U)
#define GY521_CFG_SW_MINOR_VERSION           (2U)
#define GY521_CFG_SW_PATCH_VERSION           (3U)
/************************************************************************************************************************
 *                            			     PRE-BUILD CONFIGURATIONS
 ***********************************************************************************************************************/
#define AD0_PIN_LEVEL						0		/*High Level = 1 - Low Level = 0*/
#define GY521_ACCELEROMETER_RANGE			0		/*0:+-2g, 1:+-4g, 2:+-8g, 3:+-16g*/
#define GY521_GYROSCOPE_RANGE				0		/*0:+-250 Degree/s, 1:+-500 Degree/s, 2:+-1000 Degree/s, 3:+-2000 Degree/s*/
#define INDICATOR_LED_GY521_PORT		  GPIOA	    /*a Led Indicator for initialization - LED PORT*/
#define	INDICATOR_LED_GY521_PIN			  GPIO_PIN_5
#define CALIBRATION_ITERATIONS			   500
#define	GY521_INTERNAL_DIGITAL_LPF_VALUE	6	/*(0 to 6) to know what exact value to use, refer to registers map included
												in the documentation*/
/************************************************************************************************************************
 *                            	       STM HAL CUBEX Configurations Notes
 ***********************************************************************************************************************/
/* -Enable any I2C Peripheral and configure it either in normal mode or fast mode, both are fine
 * -when using init function just pass the I2C typestruct by reference
 */
#endif /*GY521_CFG_H_*/
