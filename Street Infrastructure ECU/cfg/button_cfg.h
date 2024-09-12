/************************************************************************************************************************
 * 	Module: Button Module
 * 	File Name: button_cfg.h
 *  Authors: Ahmed Desoky
 *	Date: 23/6/2024
 *	*********************************************************************************************************************
 *	Description: This simple driver for push button
 ***********************************************************************************************************************/
#ifndef BUTTON_H_CFG_H_
#define BUTTON_H_CFG_H_
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
/*Current Version 1.0.0*/
#define BUTTON_CFG_SW_MAJOR_VERSION           (1U)
#define BUTTON_CFG_SW_MINOR_VERSION           (0U)
#define BUTTON_CFG_SW_PATCH_VERSION           (0U)
/************************************************************************************************************************
 *                            			     PRE-BUILD CONFIGURATIONS
 ***********************************************************************************************************************/
#define BUTTON_PORT		GPIOA
#define BUTTON_PIN		GPIO_PIN_5
#define INTERRUPT_MODE	1			/*Trigger of Interrupt Mode*/
/************************************************************************************************************************
 *                            	       STM HAL CUBEX Configurations Notes
 ***********************************************************************************************************************/
/*
 *	1- Configure the button connected port and pin in STM Cube MX as input pin and set the pin as pull down
 *  2- Configure it in this file the chosen pin
 *  3- If you decided to choose the pin as external interrupt, choose it as well as pull down and enable
 *     the external interrupt related to this pin and set the priority
 *  4- Place the callback function inside this function in the main
 *
 *  		void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 *  		{
 *				Button_CallBack_Function();
 *  		}
 *
 *	5- NJoy!
 */
#endif /*BUTTON_H_CFG_H_*/
