/************************************************************************************************************************
 * 	Module: Light System
 * 	File Name: light_system.c
 *  Authors: Ahmed Desoky
 *	Date: 20/7/2024
 *	*********************************************************************************************************************
 *	Description: Module to operate the vehicle lighing system seamlessly
 *				 All pins out are configured as the lights are +ve logic
 *				 pins out are defined in the configurations file
 ***********************************************************************************************************************/
/***********************************************************************************************************************
*                     							      INCLUDES
***********************************************************************************************************************/
#include "light_system.h"
/***********************************************************************************************************************
*                     					           GLOBAL VARIABLES
***********************************************************************************************************************/
static Light_State Front_Light_H_Flag = Flag_Low;
static Light_State Front_Light_L_Flag = Flag_Low;
static Light_State Back_Light_H_Flag = Flag_Low;
static Light_State Back_Light_L_Flag = Flag_Low;
static Light_State Right_Turning_Flag = Flag_Low;
static Light_State Left_Turning_Flag = Flag_Low;
static Light_State Awaiting_Flag = Flag_Low;
static Light_State Interior_Light1_Flag = Flag_Low;
static Light_State Interior_Light2_Flag = Flag_Low;
static Light_State Interior_Light3_Flag = Flag_Low;
/***********************************************************************************************************************
*                     					        FUNCTIONS DEFINTITIONS
***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: Light_System_Update
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to update pins states according to corresponding light situation
************************************************************************************************************************/
void Light_System_Update(void)
{
	/*Front Light High*/
	if(Front_Light_H_Flag == Flag_Low)
	{
		HAL_GPIO_WritePin(Front_Light_H_PORT,Front_Light_H_PIN,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(Front_Light_H_PORT,Front_Light_H_PIN,GPIO_PIN_SET);
	}
	/*Front Light Low*/
	if(Front_Light_L_Flag == Flag_Low)
	{
		HAL_GPIO_WritePin(Front_Light_L_PORT,Front_Light_L_PIN,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(Front_Light_L_PORT,Front_Light_L_PIN,GPIO_PIN_SET);
	}
	/*Back Light High*/
	if(Back_Light_H_Flag == Flag_Low)
	{
		HAL_GPIO_WritePin(Back_Light_H_PORT,Back_Light_H_PIN,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(Back_Light_H_PORT,Back_Light_H_PIN,GPIO_PIN_SET);
	}
	/*Back Light Low*/
	if(Back_Light_L_Flag == Flag_Low)
	{
		HAL_GPIO_WritePin(Back_Light_L_PORT,Back_Light_L_PIN,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(Back_Light_L_PORT,Back_Light_L_PIN,GPIO_PIN_SET);
	}
	/*Awaiting Light*/
	if(Awaiting_Flag == Flag_Low)
	{
		/*Right Turning Light*/
		if(Right_Turning_Flag == Flag_Low)
		{
			HAL_GPIO_WritePin(Right_Turning_PORT,Right_Turning_PIN,GPIO_PIN_RESET);
		}
		else
		{
			HAL_GPIO_TogglePin(Right_Turning_PORT,Right_Turning_PIN);
		}
		/*Left Turning Light*/
		if(Left_Turning_Flag == Flag_Low)
		{
			HAL_GPIO_WritePin(Left_Turning_PORT,Left_Turning_PIN,GPIO_PIN_RESET);
		}
		else
		{
			HAL_GPIO_TogglePin(Left_Turning_PORT,Left_Turning_PIN);
		}
		HAL_GPIO_WritePin(Awaiting_PORT,Awaiting_PIN,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_TogglePin(Awaiting_PORT,Awaiting_PIN);
		HAL_GPIO_TogglePin(Right_Turning_PORT,Right_Turning_PIN);
		HAL_GPIO_TogglePin(Left_Turning_PORT,Left_Turning_PIN);
	}
	/*Interior Light 1*/
	if(Interior_Light1_Flag == Flag_Low)
	{
		HAL_GPIO_WritePin(Interior_Light1_PORT,Interior_Light1_PIN,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(Interior_Light1_PORT,Interior_Light1_PIN,GPIO_PIN_SET);
	}
	/*Interior Light 1*/
	if(Interior_Light2_Flag == Flag_Low)
	{
		HAL_GPIO_WritePin(Interior_Light2_PORT,Interior_Light2_PIN,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(Interior_Light2_PORT,Interior_Light2_PIN,GPIO_PIN_SET);
	}
	/*Interior Light 1*/
	if(Interior_Light3_Flag == Flag_Low)
	{
		HAL_GPIO_WritePin(Interior_Light3_PORT,Interior_Light3_PIN,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(Interior_Light3_PORT,Interior_Light3_PIN,GPIO_PIN_SET);
	}
}
/************************************************************************************************************************
* Function Name: Light_System_Set_State
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): Light Type
* 				   State to set
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to set state of certain light in the light system
************************************************************************************************************************/
void Light_System_Set_State(Light_Type Light,Light_State State)
{
	switch(Light)
	{
		case Front_Light_H:
			Front_Light_H_Flag = State;
			break;
		case Front_Light_L:
			Front_Light_L_Flag = State;
			break;
		case Back_Light_H:
			Back_Light_H_Flag = State;
			break;
		case Back_Light_L:
			Back_Light_L_Flag = State;
			break;
		case Right_Turning:
			Right_Turning_Flag = State;
			break;
		case Left_Turning:
			Left_Turning_Flag = State;
			break;
		case Awaiting:
			Awaiting_Flag = State;
			break;
		case Interior_Light1:
			Interior_Light1_Flag = State;
			break;
		case Interior_Light2:
			Interior_Light2_Flag = State;
			break;
		case Interior_Light3:
			Interior_Light3_Flag = State;
			break;
	}
}
/************************************************************************************************************************
* Function Name: Light_System_Get_State
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): Light Type
* 				   light state variable by reference
* Parameters (inout): NONE
* Parameters (out): light state
* Return value: NONE
* Description: Function to get the state of certain light in the light system
************************************************************************************************************************/
void Light_System_Get_State(Light_Type Light,Light_State* State)
{
	switch(Light)
	{
		case Front_Light_H:
			*State = Front_Light_H_Flag;
			break;
		case Front_Light_L:
			*State = Front_Light_L_Flag;
			break;
		case Back_Light_H:
			*State = Back_Light_H_Flag;
			break;
		case Back_Light_L:
			*State = Back_Light_L_Flag;
			break;
		case Right_Turning:
			*State = Right_Turning_Flag;
			break;
		case Left_Turning:
			*State = Left_Turning_Flag;
			break;
		case Awaiting:
			*State = Awaiting_Flag;
			break;
		case Interior_Light1:
			*State = Interior_Light1_Flag;
			break;
		case Interior_Light2:
			*State = Interior_Light2_Flag;
			break;
		case Interior_Light3:
			*State = Interior_Light3_Flag;
			break;
	}
}
/************************************************************************************************************************
* Function Name: Light_System_Toggle_State
* Function ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): Light Type
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to toggle the state of certain light in the light system
************************************************************************************************************************/
void Light_System_Toggle_State(Light_Type Light)
{
	switch(Light)
	{
		case Front_Light_H:
			if(Front_Light_H_Flag == Flag_Low)
			{
				Front_Light_H_Flag = Flag_High;
			}
			else
			{
				Front_Light_H_Flag = Flag_Low;
			}
			break;
		case Front_Light_L:
			if(Front_Light_L_Flag == Flag_Low)
			{
				Front_Light_L_Flag = Flag_High;
			}
			else
			{
				Front_Light_L_Flag = Flag_Low;
			}
			break;
		case Back_Light_H:
			break;
		case Back_Light_L:
			if(Back_Light_L_Flag == Flag_Low)
			{
				Back_Light_L_Flag = Flag_High;
			}
			else
			{
				Back_Light_L_Flag = Flag_Low;
			}
			break;
		case Right_Turning:
			if(Right_Turning_Flag == Flag_Low)
			{
				Right_Turning_Flag = Flag_High;
			}
			else
			{
				Right_Turning_Flag = Flag_Low;
			}
			break;
		case Left_Turning:
			if(Left_Turning_Flag == Flag_Low)
			{
				Left_Turning_Flag = Flag_High;
			}
			else
			{
				Left_Turning_Flag = Flag_Low;
			}
			break;
		case Awaiting:
			if(Awaiting_Flag == Flag_Low)
			{
				Awaiting_Flag = Flag_High;
			}
			else
			{
				Awaiting_Flag = Flag_Low;
			}
			break;
		case Interior_Light1:
			if(Interior_Light1_Flag == Flag_Low)
			{
				Interior_Light1_Flag = Flag_High;
			}
			else
			{
				Interior_Light1_Flag = Flag_Low;
			}
			break;
		case Interior_Light2:
			if(Interior_Light2_Flag == Flag_Low)
			{
				Interior_Light2_Flag = Flag_High;
			}
			else
			{
				Interior_Light2_Flag = Flag_Low;
			}			break;
		case Interior_Light3:
			if(Interior_Light3_Flag == Flag_Low)
			{
				Interior_Light3_Flag = Flag_High;
			}
			else
			{
				Interior_Light3_Flag = Flag_Low;
			}			break;
	}
}
