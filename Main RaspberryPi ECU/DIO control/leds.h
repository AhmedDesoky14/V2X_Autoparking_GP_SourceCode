/************************************************************************************************************************
 * Module: LEDs
 * File Name: leds.h
 * Authors: Ahmed Desoky
 * Date: 27/4/2024
 * *********************************************************************************************************************
 * Description: Simple module that uses WiringPi C library to simply configure
                and control leds without the need to configure the device tree binaries
                for building, link with wiringPi, add option -lwiringPi
 ***********************************************************************************************************************/
#ifndef LEDS_H
#define LEDS_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include <wiringPi.h>
#include "leds_cfg.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.0.0*/
#define LEDS_SW_MAJOR_VERSION           (1U)
#define LEDS_SW_MINOR_VERSION           (0U)
#define LEDS_SW_PATCH_VERSION           (0U)
/*Software Version checking between Module Configuration file and Header file*/
#if ((LEDS_SW_MAJOR_VERSION != LEDS_CFG_SW_MAJOR_VERSION)\
 ||  (LEDS_SW_MINOR_VERSION != LEDS_CFG_SW_MINOR_VERSION)\
 ||  (LEDS_SW_PATCH_VERSION != LEDS_CFG_SW_PATCH_VERSION))
  #error "The Software version of LEDs does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
void LEDs_Init(void);
void LED_OFF(unsigned char LED);
void LED_ON(unsigned char LED);
/************************************************************************************************************************
 *                    							   External Variables
 ***********************************************************************************************************************/
extern const unsigned short Configured_LEDs[Configured_LEDs_Num];
#endif /*LEDS_H*/