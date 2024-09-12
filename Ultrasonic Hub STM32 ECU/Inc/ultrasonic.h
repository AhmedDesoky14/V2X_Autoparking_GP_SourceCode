/************************************************************************************************************************
 * 	Module:ultrasonic
 * 	File Name:ultrasonic.h
 *  Authors:Esraa Fawzy
 *	Date:Feb 20, 2024
 *	*********************************************************************************************************************
 *	Description:
 *	Interfaces for interacting with ultrasonic sensors are provided by the ultrasonic.h header file. It has functions for
 *	reading distance values, starting measurements, initializing sensors, and managing timer input capture callbacks. This
 *	file requires the ultrasonic_cfg.h configuration file and the STM32 HAL library. According to the usage guidelines,
 *	source files should contain this header file, sensors should be configured using ULTRASONIC_init(), and a callback
 *	function should be implemented to handle input capture events.
 ***********************************************************************************************************************/
#ifndef INC_ULTRASONIC_H_
#define INC_ULTRASONIC_H_
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "stm32f1xx_hal.h"
#include "ultrasonic_cfg.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.1.0*/
#define ultrasonic_SW_MAJOR_VERSION           (1U)
#define ultrasonic_SW_MINOR_VERSION           (1U)
#define ultrasonic_SW_PATCH_VERSION           (0U)
/*Software Version checking between Module Configuration file and Header file*/
#if ((ultrasonic_SW_MAJOR_VERSION != ultrasonic_CFG_SW_MAJOR_VERSION)\
		||  (ultrasonic_SW_MINOR_VERSION != ultrasonic_CFG_SW_MINOR_VERSION)\
		||  (ultrasonic_SW_PATCH_VERSION != ultrasonic_CFG_SW_PATCH_VERSION))
#error "The Software version of ultrasonic does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
typedef enum
{
	US_OK,US_NOT_OK
}ULTRASONIC_STATUS;

typedef struct {
	TIM_HandleTypeDef *timer;   // Timer handle
	uint32_t channelID;         // Timer channel ID for interrupt handling
	uint32_t interruptSource;   // Interrupt source for the timer channel
	GPIO_TypeDef *trigPort;     // TRIG GPIO port
	uint16_t trigPin;           // TRIG GPIO pin
	uint8_t index;
} UltrasonicConfig;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
ULTRASONIC_STATUS ULTRASONIC_init(void);
void TIM_IC_CaptureCallback_ultrasonic(TIM_HandleTypeDef *htim, uint8_t index);
ULTRASONIC_STATUS ULTRASONIC_readDistance(uint8_t ultrasonic_ID,float* dist);
/************************************************************************************************************************
 *                    							   External Variables
 ***********************************************************************************************************************/
extern UltrasonicConfig ultrasonicConfigs[ULTRASONIC_NUMBERS];
#endif /* INC_ULTRASONIC_H_ */
