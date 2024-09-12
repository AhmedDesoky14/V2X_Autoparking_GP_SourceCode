/************************************************************************************************************************
 * 	Module: Light System
 * 	File Name: light_system_cfg.h
 *  Authors: Ahmed Desoky
 *	Date: 20/7/2024
 *	*********************************************************************************************************************
 *	Description: Module to operate the vehicle lighing system seamlessly
 *				 All pins out are configured as the lights are +ve logic
 *				 pins out are defined in the configurations file
 ***********************************************************************************************************************/
#ifndef LIGHT_SYSTEM_CFG_H_
#define LIGHT_SYSTEM_CFG_H_
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
/*Current Version 1.0.0*/
#define LIGHT_SYSTEM_CFG_SW_MAJOR_VERSION           (1U)
#define LIGHT_SYSTEM_CFG_SW_MINOR_VERSION           (0U)
#define LIGHT_SYSTEM_CFG_SW_PATCH_VERSION           (0U)
/************************************************************************************************************************
 *                            			     PRE-BUILD CONFIGURATIONS
 ***********************************************************************************************************************/
/*Light System Ports*/
#define Front_Light_H_PORT			GPIOB
#define Front_Light_L_PORT			GPIOB
#define Back_Light_H_PORT			GPIOB
#define Back_Light_L_PORT			GPIOB
#define Right_Turning_PORT			GPIOA
#define Left_Turning_PORT			GPIOA
#define Awaiting_PORT				GPIOA
#define Interior_Light1_PORT		GPIOB
#define Interior_Light2_PORT		GPIOB
#define Interior_Light3_PORT		GPIOB
/*Light System Pins*/
#define Front_Light_H_PIN			GPIO_PIN_12
#define Front_Light_L_PIN			GPIO_PIN_13
#define Back_Light_H_PIN			GPIO_PIN_14
#define Back_Light_L_PIN			GPIO_PIN_15
#define Right_Turning_PIN			GPIO_PIN_8
#define Left_Turning_PIN			GPIO_PIN_9
#define Awaiting_PIN				GPIO_PIN_10
#define Interior_Light1_PIN			GPIO_PIN_6
#define Interior_Light2_PIN			GPIO_PIN_5
#define Interior_Light3_PIN			GPIO_PIN_4
#endif /*LIGHT_SYSTEM_CFG_H_*/
