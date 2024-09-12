/************************************************************************************************************************
 * Module: LEDs
 * File Name: leds.c
 * Authors: Ahmed Desoky
 * Date: 27/4/2024
 * *********************************************************************************************************************
 * Description: Simple module that uses WiringPi C library to simply configure
                and control leds without the need to configure the device tree binaries
                for building, link with wiringPi, add option -lwiringPi
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "leds.h"
/***********************************************************************************************************************
 *                     					        FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: LEDs_Init
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to initialize LEDs functionality
************************************************************************************************************************/
void LEDs_Init(void)
{
    wiringPiSetup();
    for(unsigned short i=0 ; i<Configured_LEDs_Num ; i++)
    {
        pinMode(Configured_LEDs[i],OUTPUT);
    }
    return;
}
/************************************************************************************************************************
* Function Name: LED_OFF
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): Concerned LED
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to turn a specific LED off
************************************************************************************************************************/
void LED_OFF(unsigned char LED)
{
    digitalWrite(LED,LOW);
}
/************************************************************************************************************************
* Function Name: LED_ON
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): Concerned LED
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to turn a specific LED on
************************************************************************************************************************/
void LED_ON(unsigned char LED)
{
    digitalWrite(LED,HIGH);
}