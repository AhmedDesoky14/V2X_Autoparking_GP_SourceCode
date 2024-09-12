/************************************************************************************************************************
 * 	Module: V2I / Network Database Communications Modes Switch Module
 * 	File Name: ESP_Mode_Switch.h
 *  Authors: Ahmed Desoky
 *	Date: 7/4/2024
 *	*********************************************************************************************************************
 *	Description: This module is used to change ESP mode, either for V2I Communications or Network Database communications
 *			     If ESP is in V2I Communications mode it establishes TCP Server for this application
 *			     IF ESP is in Network Database Communications mode it disbands TCP Server to act as MQTT Client for
 *			     this application
 ***********************************************************************************************************************/
#ifndef MODE_SWITCH_H
#define MODE_SWITCH_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "esp8266.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.0.1*/
#define MODULE_NAME_SW_MAJOR_VERSION           (1U)
#define MODULE_NAME_SW_MINOR_VERSION           (0U)
#define MODULE_NAME_SW_PATCH_VERSION           (1U)
/*Software Version checking of Dependent Module -ESP8266 Driver - Version 2.4.4*/
#if ((ESP8266_SW_MAJOR_VERSION != (2U))\
 ||  (ESP8266_SW_MINOR_VERSION != (4U))\
 ||  (ESP8266_SW_PATCH_VERSION != (4U)))
  #error "The Software version of ESP8266 Module does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
typedef enum
{
	V2I_Communications,Network_Database_Communications
}ESP_OPERATION_MODE;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
ESP8266_RESPONSE Set_ESP_Operation_Mode(ESP_OPERATION_MODE Mode);
ESP_OPERATION_MODE Get_ESP_Operation_Mode(void);
#endif /*MODE_SWITCH_H*/
