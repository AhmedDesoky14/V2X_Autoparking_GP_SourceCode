/************************************************************************************************************************
 * Module: LEDs
 * File Name: leds_PBcfg.h
 * Authors: Ahmed Desoky
 *	Date: 27/4/2024
 *	*********************************************************************************************************************
 *	Description: Simple module that uses WiringPi C library to simply configure
                and control leds without the need to configure the device tree binaries
                for building, link with wiringPi, add option -lwiringPi
 ***********************************************************************************************************************/
/************************************************************************************************************************
 *                     							         INCLUDES
 ***********************************************************************************************************************/
#include "leds.h"
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.0.0*/
#define LEDS_PBCFG_SW_MAJOR_VERSION           (1U)
#define LEDS_PBCFG_SW_MINOR_VERSION           (0U)
#define LEDS_PBCFG_SW_PATCH_VERSION           (0U)
/*Software Version checking between Module Post Build Configuration file and Header file*/
#if ((LEDS_SW_MAJOR_VERSION != LEDS_PBCFG_SW_MAJOR_VERSION)\
 ||  (LEDS_SW_MINOR_VERSION != LEDS_PBCFG_SW_MINOR_VERSION)\
 ||  (LEDS_SW_PATCH_VERSION != LEDS_PBCFG_SW_PATCH_VERSION))
  #error "The Software version of LEDs does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                            			          CONFIGURATIONS
 ***********************************************************************************************************************/
/* before configuring the gpios, run "gpio readall" bash command 
 * and map your chosen physical gpio pin to wPi pin, ex: Physicall pin37 is mapped to wPi pin25
 * so to setup up pin37 write pin25
 */
const unsigned short Configured_LEDs[Configured_LEDs_Num] = {24,25};


