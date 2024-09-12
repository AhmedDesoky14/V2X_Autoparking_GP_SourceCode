/************************************************************************************************************************
 * Module: esp8266
 * File Name: esp8266_PBcfg.c
 * Authors: Ahmed Desoky
 * Date: 19/2/2024
 *	*********************************************************************************************************************
 *	Description: ESP8266 Driver Post-build configurations file
 *				 ESP8266 Driver on STM32F103C8T6 Micro-controller.
 *				 Micro-controller Drivers are STM32 HAL Drivers.
 *				 This Driver is designed to support generally any application it's needed to be used in
 *				 You can skip looking in source file functions if you intend you use it only
 *				 This Driver uses the dedicated UART peripheral resources, and no other device or instance can share it
 ***********************************************************************************************************************/
 /************************************************************************************************************************
 *                     							         INCLUDES
 ***********************************************************************************************************************/
#include "esp8266.h"
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 2.4.4*/
#define ESP8266_PBCFG_SW_MAJOR_VERSION           (2U)
#define ESP8266_PBCFG_SW_MINOR_VERSION           (4U)
#define ESP8266_PBCFG_SW_PATCH_VERSION           (4U)
/*Software Version checking between Module Post Build Configuration file and Header file*/
#if ((ESP8266_SW_MAJOR_VERSION != ESP8266_PBCFG_SW_MAJOR_VERSION)\
 ||  (ESP8266_SW_MINOR_VERSION != ESP8266_PBCFG_SW_MINOR_VERSION)\
 ||  (ESP8266_SW_PATCH_VERSION != ESP8266_PBCFG_SW_PATCH_VERSION))
  #error "The Software version of ESP8266 does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                            			          CONFIGURATIONS
 ***********************************************************************************************************************/
/*ESP8266 Initial Configuration object parameters*/
const ESP8266_INIT_CONFIG ESP8266_Configuration = {AP_STATION_MODE,			/*Mode*/
													ESP8266_CONNECT,		/*Auto connect initially or not*/
													"9c:c6:3a:b6:dc:66",	/*Station MAC Address*/
													"8c:c6:3a:b6:dc:66",	/*Access Point MAC Address*/
													"EECE2024",				/*Default Connect SSID*/
													"CUFE@123",				/*Default Connect Password*/
													"INF_01x",				/*Access Point SSID*/
													"12345678",				/*Access Point Password*/
													"192.168.10.10",		/*Access Point Static IP*/
													1,	/*Max stations can connect to the Access Point, Limit = 4*/
														/*For power consumption issue and power limitation, please
														 * set the max stations less than 4*/
													3};				/*Channel ID*/
