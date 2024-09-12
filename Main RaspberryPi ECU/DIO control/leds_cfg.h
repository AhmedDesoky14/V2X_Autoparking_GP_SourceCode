/************************************************************************************************************************
 * Module: LEDs
 * File Name: leds_cfg.h
 * Authors: Ahmed Desoky
 * Date: 27/4/2024
 * *********************************************************************************************************************
 * Description: Simple module that uses WiringPi C library to simply configure
                and control leds without the need to configure the device tree binaries
                for building, link with wiringPi, add option -lwiringPi
 ***********************************************************************************************************************/
#ifndef LEDS_CFG_H_
#define LEDS_CFG_H_
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
/*Current Version 1.0.0*/
#define LEDS_CFG_SW_MAJOR_VERSION           (1U)
#define LEDS_CFG_SW_MINOR_VERSION           (0U)
#define LEDS_CFG_SW_PATCH_VERSION           (0U)
/************************************************************************************************************************
 *                            			     PRE-BUILD CONFIGURATIONS
 ***********************************************************************************************************************/
#define Configured_LEDs_Num     2
/************************************************************************************************************************
 *                            	               Configurations Notes
 ***********************************************************************************************************************/
/*
 *  1- Configure number of used leds in leds_cgh.h file
 *  2- add the dedicated gpio pins for the leds in leds_PBcfg.c file in Configured_LEDs array
 *  3- include "leds.h"
 *  4- when you build this module files, link them to wiringPi (-l wiringPi)
 *  5- NJoy!
 */
#endif /*LEDS_CFG_H_*/
