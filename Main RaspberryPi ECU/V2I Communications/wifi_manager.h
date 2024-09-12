/************************************************************************************************************************
 * 	Module: WiFi Manager
 * 	File Name: wifi_manager.h
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
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>
#include "leds.h"
#include "wifi_manager_cfg.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 2.0.1*/
#define WIFI_MANAGER_SW_MAJOR_VERSION           (2U)
#define WIFI_MANAGER_SW_MINOR_VERSION           (0U)
#define WIFI_MANAGER_SW_PATCH_VERSION           (1U)
/*Software Version checking between Module Configuration file and Header file*/
#if ((WIFI_MANAGER_SW_MAJOR_VERSION != WIFI_MANAGER_CFG_SW_MAJOR_VERSION)\
 ||  (WIFI_MANAGER_SW_MINOR_VERSION != WIFI_MANAGER_CFG_SW_MINOR_VERSION)\
 ||  (WIFI_MANAGER_SW_PATCH_VERSION != WIFI_MANAGER_CFG_SW_PATCH_VERSION))
  #error "The Software version of WiFi Manager does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
//enum used to describe WiFi connections operations results
typedef enum
{
  WIFI_OK,WIFI_NOT_OK,WIFI_EXE_ERROR
}WIFI_STATE;
//enum used to describe WiFi device operations results
typedef enum 
{
  WIFI_DEVICE_ON,WIFI_DEVICE_OFF,WIFI_DEVICE_ERROR
}WIFI_DEVICE_STATE;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
WIFI_STATE WiFi_Connect(const char* SSID,const char* Password);
WIFI_STATE WiFi_Disconnect(void);
WIFI_STATE WiFi_Search_AP(const char* SSID);
WIFI_STATE WiFi_Check_AP_Connection(void);
WIFI_STATE WiFi_Get_IP_Address(char* IP_Address);
WIFI_STATE WiFi_Check_Internet_Connection(void);
WIFI_DEVICE_STATE WiFi_Enable(void);
WIFI_DEVICE_STATE WiFi_Disable(void);
WIFI_DEVICE_STATE WiFi_Check_Availability(void);
WIFI_STATE WiFi_Manager_Init(void);
#endif /*WIFI_MANAGER_H*/