/************************************************************************************************************************
 * 	Module:     IR Sensor
 * 	File Name:  Ir_cfg.C
 *  Authors:    Dina Hamed
 *	Date:       Jan 30, 2024
 *	*********************************************************************************************************************
 *	Description: Driver Configurations File.
 *				 IR sensor Driver on STM32F103C8T6 Micro-controller.
 *				 Micro-controller Drivers are STM32 HAL Drivers.
 *	this module detects and handles events triggered by the IR sensor,
 *	such as the detection of objects or changes in infrared radiation levels.
 *
 *
 ***********************************************************************************************************************//***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "Ir.h"

/***********************************************************************************************************************
 *                     					         GLOBAL VARIABLES
 ***********************************************************************************************************************/
extern uint8_t IR_Pins[IR_NUMBERS];
extern GPIO_TypeDef *IR_Ports[IR_NUMBERS];
extern uint16_t LED_Pins[IR_NUMBERS];
extern GPIO_TypeDef *LED_Ports[IR_NUMBERS];

/***********************************************************************************************************************
 *                     					        FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
*Function Name:    isOccupied
*Function ID[hex]: 0x00
*Sync/Async:       Synchronous
*Reentrancy:       Non-reentrant
*Parameters (in):    pin - Index of the GPIO pin connected to the IR sensor
*Parameters (inout): None
*Parameters (out):   None
*Return value: State of the GPIO pin (GPIO_PIN_RESET or GPIO_PIN_SET)
*Description: This function reads data from an infrared (IR) sensor connected to a specific GPIO pin.
             It takes an integer parameter 'pin' representing the index of the pin connected to the IR sensor.
             The function utilizes the HAL library to read the state (either GPIO_PIN_RESET or GPIO_PIN_SET) of the GPIO pin
             corresponding to the IR sensor, located on the GPIO port IR_PORT and specified by the pin index IR_Pins[pin].
             Finally, it returns the state of the GPIO pin, which indicates the presence or absence of an IR signal.
************************************************************************************************************************/

void isOccupied(int array_id)
{


	if(IR_ReadData(array_id) == NOT_AVAILABLE_SLOT)
	{
		HAL_GPIO_WritePin(LED_Ports[array_id], LED_Pins[array_id],GPIO_PIN_RESET);

	}
	else
	{
		HAL_GPIO_WritePin(LED_Ports[array_id], LED_Pins[array_id],GPIO_PIN_SET);


	}
}
/************************************************************************************************************************
*Function Name:    IR_ReadData
*Function ID[hex]: 0x01
*Sync/Async:       Synchronous
*Reentrancy:       Non-reentrant
*Parameters (in):    pin - Index of the GPIO pin connected to the IR sensor
*Parameters (inout): None
*Parameters (out):   None
*Return value: State of the GPIO pin (GPIO_PIN_RESET or GPIO_PIN_SET)
*Description: This function reads data from an infrared (IR) sensor connected to a specific GPIO pin.
             It takes an integer parameter 'pin' representing the index of the pin connected to the IR sensor.
             The function utilizes the HAL library to read the state (either GPIO_PIN_RESET or GPIO_PIN_SET) of the GPIO pin
             corresponding to the IR sensor, located on the GPIO port IR_PORT and specified by the pin index IR_Pins[pin].
             Finally, it returns the state of the GPIO pin, which indicates the presence or absence of an IR signal.
************************************************************************************************************************/

slot_state IR_ReadData(int array_id)
{
	if(array_id >= IR_NUMBERS)
		{
			return HAL_ERROR;

		}

    return HAL_GPIO_ReadPin(IR_Ports[array_id], IR_Pins[array_id]);
}
