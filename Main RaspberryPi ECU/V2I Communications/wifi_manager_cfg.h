/************************************************************************************************************************
 * 	Module: WiFi Manager
 * 	File Name: wifi_manager_cfg.h
 *  Authors: Ahmed Desoky
 *	Date: 11/4/2024
 *	*********************************************************************************************************************
 *	Description: -Configurations file of WiFi Manager module to reconfigure before build and use
 *               This module is mainly based on "system" system call, you can check manual page -> man system.
 *               This system call is based on execvl and fork system calls which are mainly used to create
 *               a child process and replace this process with the desired process to invoke (execute a bash command)
 *               this system call differs from the mentioned others by being uninterruptible, so the running thread 
 *               will be pending the process to terminate.
 *               -This module uses a temporary file configured in the cfg file and used as a communication way
 *               between the executed command and the main program to parse commands returns and take decisions.
 *               -This module is designed and implemented for Unix based operating systems and can be configured for
 *               different WiFi interfaces devices as mentioned in Configurations Notes section.
 *               -It is possible to also control Hot-Spot Access Point using the same Network Manager Tool
 *               but it's not implemented in this module.
 *               -This module has a dedicated thread to continuously scan for nearby WiFi networks and store them in
                 a certain txt file configured in the cfg file
 ***********************************************************************************************************************/
#ifndef WIFI_MANAGER_CFG_H_
#define WIFI_MANAGER_CFG_H_
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
/*Current Version 1.0.1*/
#define WIFI_MANAGER_CFG_SW_MAJOR_VERSION           (2U)
#define WIFI_MANAGER_CFG_SW_MINOR_VERSION           (0U)
#define WIFI_MANAGER_CFG_SW_PATCH_VERSION           (1U)
/************************************************************************************************************************
 *                           			     CONFIGURATIONS
 ***********************************************************************************************************************/
#define INTERFFACE_NAME                 "wlan0"
#define OPERATIONS_FILE_PATH            "./operations_temp.txt"
#define NETWORKS_FILE_PATH              "./networks.txt"
#define WIFI_CONNECT_LED_PIN            24  /*BCM PIN ID*/
#define INTERNET_CONNECT_LED_PIN        25  /*BCM PIN ID*/
/************************************************************************************************************************
 *                            	           Configurations Notes
 ***********************************************************************************************************************/
/* 
 * 1- Check the machine's WiFi Interface name using "iwconfig" bash command
 * 2- Configure the WiFi Interface name
 * 3- Set the path and the name of the dedicated temporary files for WiFi operations and networks scan results 
 * 4- Configure indication leds in this file and in leds_Cfg.h file and leds_PBCfg.c file 
 * 5- Include "wifi_manager.h"
 * 6- NJoy! :D
 */
#endif /*WIFI_MANAGER_CFG_H_*/
