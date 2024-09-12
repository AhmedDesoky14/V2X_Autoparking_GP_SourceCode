/************************************************************************************************************************
 * 	Module: V2I / Network Database Communications Modes Switch Module
 * 	File Name: ESP_Mode_Switch.c
 *  Authors: Ahmed Desoky
 *	Date: 7/4/2024
 *	*********************************************************************************************************************
 *	Description: This module is used to change ESP mode, either for V2I Communications or Network Database communications
 *			     If ESP is in V2I Communications mode it establishes TCP Server for this application
 *			     IF ESP is in Network Database Communications mode it disbands TCP Server to act as MQTT Client for
 *			     this application
 ***********************************************************************************************************************/
 /***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "ESP_Mode_Switch.h"
 /***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
ESP_OPERATION_MODE ESP_Mode = Network_Database_Communications;	/*Initially*/
/***********************************************************************************************************************
 *                     					        FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: Set_ESP_Operation_Mode
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): Mode to be set
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to set the desired mode for operation of the ESP
************************************************************************************************************************/
ESP8266_RESPONSE Set_ESP_Operation_Mode(ESP_OPERATION_MODE Mode)
{
	if(Mode == V2I_Communications)
	{
		if(ESP8266_Close_TCP_Connection(5) != RESPONSE_OK) 	/*Close all connections*/
		{
			return RESPONSE_NOT_OK;
		}
		if(ESP8266_Establish_TCP_Server() != RESPONSE_OK) 	/*Establish TCP Server*/
		{
			return RESPONSE_NOT_OK;
		}
		ESP_Mode = Mode;
	}
	else if(Mode == Network_Database_Communications)
	{
		if(ESP8266_Close_TCP_Connection(5) != RESPONSE_OK) 	/*Close all connections*/
		{
			return RESPONSE_NOT_OK;
		}
		if(ESP8266_Disband_TCP_Server() != RESPONSE_OK) 	/*Disband TCP Server*/
		{
			return RESPONSE_NOT_OK;
		}
		ESP_Mode = Mode;
	}
	else
	{
		/*Do Nothing*/
	}
	return RESPONSE_OK;
}
/************************************************************************************************************************
* Function Name: Get_ESP_Operation_Mode
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Mode set and being used for the current application
* Description: Function to get the current mode set and being used for the current application
************************************************************************************************************************/
ESP_OPERATION_MODE Get_ESP_Operation_Mode(void)
{
	return ESP_Mode;
}
