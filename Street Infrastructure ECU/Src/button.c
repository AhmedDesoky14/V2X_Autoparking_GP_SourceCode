/************************************************************************************************************************
 * 	Module: Button Module
 * 	File Name: button.c
 *  Authors: Ahmed Desoky
 *	Date: 23/6/2024
 *	*********************************************************************************************************************
 *	Description: This simple driver for push button
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "button.h"
/***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
#if(INTERRUPT_MODE == 1)
static volatile void (*Button_Callback_Function_Ptr)(void) = NULL;
#endif
/***********************************************************************************************************************
 *                     					      PRIVATE FUNCTIONS PROTOTYPES
 ***********************************************************************************************************************/
static void Delay_ms(uint32_t delay_ms);
/***********************************************************************************************************************
 *                     					        FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
#if(INTERRUPT_MODE == 0)
/************************************************************************************************************************
* Function Name: Button_Read
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Reenterant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: Button read value / Status
* Description: Function to return status of the button, used for checking status by polling
************************************************************************************************************************/
BUTTON_STATUS Button_Read(void)
{
	if(HAL_GPIO_ReadPin(BUTTON_PORT,BUTTON_PIN))
	{
		Delay_ms(100);
		if(HAL_GPIO_ReadPin(BUTTON_PORT,BUTTON_PIN))
		{
			return BUTTON_PUSHED;
		}
	}
	return BUTTON_RELEASED;
}

#elif(INTERRUPT_MODE == 1)
/************************************************************************************************************************
* Function Name: Button_Set_CallBack_Function
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Reenterant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to set the callback function to be called when external interrupt occurs
************************************************************************************************************************/
void Button_Set_CallBack_Function(void(*Function_Ptr)(void))
{
	Button_Callback_Function_Ptr = Function_Ptr;
}
/************************************************************************************************************************
* Function Name: Button_CallBack_Function
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Reenterant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to call the callback function when external interrupt occurs
************************************************************************************************************************/
void Button_CallBack_Function(void)
{
	if(Button_Callback_Function_Ptr != NULL)
	{
		(*Button_Callback_Function_Ptr)();
	}
}
#endif
#if(INTERRUPT_MODE == 0)
/************************************************************************************************************************
* Function Name: Delay_ms
* Function ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Reenterant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Delay function
************************************************************************************************************************/
static void Delay_ms(uint32_t delay_ms)
{
	uint32_t SystemClockFreq = HAL_RCC_GetHCLKFreq();
	uint32_t cycles_per_ms = SystemClockFreq / 16000;
	uint32_t total_cycles = cycles_per_ms * delay_ms;
	for (volatile uint32_t i = 0; i < total_cycles; ++i);
	return;
}
#endif
