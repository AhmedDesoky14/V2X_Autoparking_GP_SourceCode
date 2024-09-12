/************************************************************************************************************************
 * Module:ultrasonic
 * File Name: ultrasonic_PBcfg.c
 *  Authors: Esraa Fawzy
 *  Date: Feb 20, 2024
 *	*********************************************************************************************************************
 *	Description:This file contains the configuration data for ultrasonic sensors in the system.
 *	It specifies the hardware configurations such as timer handlers, timer channels, timer interrupts,
 *	 GPIO ports, and GPIO pins used for each ultrasonic sensor.
 ***********************************************************************************************************************/
/************************************************************************************************************************
 *                     							         INCLUDES
 ***********************************************************************************************************************/
#include"ultrasonic.h"
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.1.0*/
#define ultrasonic_PBCFG_SW_MAJOR_VERSION           (1U)
#define ultrasonic_PBCFG_SW_MINOR_VERSION           (1U)
#define ultrasonic_PBCFG_SW_PATCH_VERSION           (0U)

/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Software Version checking between Module Post Build Configuration file and Header file*/
#if ((ultrasonic_SW_MAJOR_VERSION != ultrasonic_PBCFG_SW_MAJOR_VERSION )\
		||  (ultrasonic_SW_MINOR_VERSION !=ultrasonic_PBCFG_SW_MINOR_VERSION)\
		||  (ultrasonic_SW_PATCH_VERSION != ultrasonic_PBCFG_SW_PATCH_VERSION))
#error "The Software version of ultrasonic does not match the configurations expected version"
#endif

/************************************************************************************************************************
 *                            			          CONFIGURATIONS
 ***********************************************************************************************************************/
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim4;

UltrasonicConfig ultrasonicConfigs[ULTRASONIC_NUMBERS] ={
		{&htim1, TIM_CHANNEL_1, TIM_IT_CC1, GPIOA, GPIO_PIN_0,0},
		{&htim1, TIM_CHANNEL_2, TIM_IT_CC2, GPIOA, GPIO_PIN_4,1}//,
		//{&htim4, TIM_CHANNEL_1, TIM_IT_CC1, GPIOA, GPIO_PIN_6,2}//,
		//{&htim4, TIM_CHANNEL_2, TIM_IT_CC2, GPIOB, GPIO_PIN_13,3}
};
