/************************************************************************************************************************
 * 	Module: GY-271
 * 	File Name: gy271.c
 *  Authors: Ahmed Desoky
 *	Date: 13/12/2023
 *	*********************************************************************************************************************
 *	Description: Driver Source File.
 *				 GY-271 Magnetometer Driver on STM32F103C8T6 Micro-controller.
 *				 Micro-controller Drivers are STM32 HAL Drivers.
 *	*********************************************************************************************************************
 *	This Module is programmed to support (without configuring) as default the following features.
 *	#The module must be placed face up for proper functionality
 *	#Output Datarate = 200Hz
 *	#Mainly created to get yaw angle
 ***********************************************************************************************************************/
 /***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "gy271.h"
/***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
static I2C_HandleTypeDef* hi2cx;	/*Initialization Structure to use in all HAL Functions in the source file*/
/***********************************************************************************************************************
 *                     					      PRIVATE FUNCTIONS PROTOTYPES
 ***********************************************************************************************************************/
static void Delay_ms(uint32_t delay_ms);




// Define I2C address and registers for QMC5883L
#define QMC5883L_ADDRESS 0x0D
#define QMC5883L_REG_CTRL1 0x09
#define QMC5883L_REG_CTRL2 0x0A

// Configuration values
#define QMC5883L_MODE_CONTINUOUS 0x01
#define QMC5883L_ODR_200HZ 0x0C  // Maximum data rate
#define QMC5883L_OSR_256 0x80
#define QMC5883L_RANGE_8G 0x10









/***********************************************************************************************************************
 *                     					        FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: GY_271_Init
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): I2C Handle Structure for initialization
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: HAL Function Status
* Description: Function to Initialize GY271 Magnetometer
* 			   #The module must be in flat plain position while initialization
************************************************************************************************************************/
HAL_StatusTypeDef GY_271_Init(I2C_HandleTypeDef* hi2c)
{
	hi2cx = hi2c;
	uint8_t Recent_Status = 0;
	uint8_t	Loaded_Register_Value = 0;
	Delay_ms(20);		/*Delay for 20ms after startup*/
	if(HAL_I2C_IsDeviceReady(hi2cx,GY271_ADDRESS,CHECK_AVAILABILITY_TRIALS_DEFAULT,TIMEOUT_DEFAULT) != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}
	Loaded_Register_Value = 0x81;	/*Reset the module and disable interrupt mode*/
	Recent_Status = HAL_I2C_Mem_Write(hi2cx,GY271_ADDRESS,CONTROL_REG_B,BYTE,&Loaded_Register_Value,BYTE,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}
	Delay_ms(10);		/*Delay for 10ms after startup*/
	Loaded_Register_Value = 0x01 | 0x0C;	/*Continuous Mode*/
#if(GY271_INTERNAL_DIGITAL_LPF_VALUE == 0)
	Loaded_Register_Value |= 0x00;
#elif(GY271_INTERNAL_DIGITAL_LPF_VALUE  == 1)
	Loaded_Register_Value |= 0x40;
#elif(GY271_INTERNAL_DIGITAL_LPF_VALUE == 2)
	Loaded_Register_Value |= 0x80;
#elif(GY271_INTERNAL_DIGITAL_LPF_VALUE == 3)
	Loaded_Register_Value |= 0xC0;
#endif
#if(GY271RANGE == 1)
	Loaded_Register_Value |= 0x10;
#endif
	Delay_ms(10);		/*Delay for 10ms*/
	Recent_Status = HAL_I2C_Mem_Write(hi2cx,GY271_ADDRESS,CONTROL_REG_A,BYTE,&Loaded_Register_Value,BYTE,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}
	Delay_ms(10);		/*Delay for 10ms before any readings*/
	GY_271_FUNCTION_INDICATE();
	return HAL_OK;
}
/************************************************************************************************************************
* Function Name: GY_271_Magnetometer_Read
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in):  pointer to float variable to store Magnetic Fields
* Parameters (inout): NONE
* Parameters (out): pointer to float variable to store Magnetic Fields
* Return value: HAL Function Status
* Description: Function to read magnetic field in 3-axis in milli Gauss units.
************************************************************************************************************************/
HAL_StatusTypeDef GY_271_Magnetometer_Read(float* Value)
{
	uint8_t Recent_Status = 0;
	uint8_t Temp_Data[6];
	int16_t Data[3];
	Recent_Status = HAL_I2C_Mem_Read(hi2cx,GY271_ADDRESS,MAGNET_XOUT_L,BYTE,Temp_Data,MAGNETOMETER_REGISTERS_NUMBER,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		GY_271_ERROR_INDICATE();
		return HAL_ERROR;	/*Error Occurred*/
	}
	Data[0] = (int16_t)(Temp_Data[0] | Temp_Data[1]<<8);
	Data[1] = (int16_t)(Temp_Data[2] | Temp_Data[3]<<8);
	Data[2] = (int16_t)(Temp_Data[4] | Temp_Data[5]<<8);
#if(GY271RANGE == 0)
	for(unsigned int i=0 ; i<=2 ; i++)
	{
		Value[i] = Data[i]/12000.0;
	}
#elif(GY271RANGE == 1)
	for(unsigned int i=0 ; i<=2 ; i++)
	{
		Value[i] = Data[i]/3000.0;
	}
#endif
	return HAL_OK;
}
/************************************************************************************************************************
* Function Name: GY_271_ERROR_INDICATE
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to turn off functionality LED to indicate error.
* 			   This function can be used by other modules that depend on this one.
************************************************************************************************************************/
void GY_271_ERROR_INDICATE(void)
{
	HAL_GPIO_WritePin(INDICATOR_LED_GY271_PORT,INDICATOR_LED_GY271_PIN,GPIO_PIN_RESET);	/*INDICATOR LED is OFF - ERROR*/
}
/************************************************************************************************************************
* Function Name: GY_271_FUNCTION_INDICATE
* Function ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to turn on functionality LED to indicate OK.
* 			   This function can be used by other modules that depend on this one.
************************************************************************************************************************/
void GY_271_FUNCTION_INDICATE(void)
{
	HAL_GPIO_WritePin(INDICATOR_LED_GY271_PORT,INDICATOR_LED_GY271_PIN,GPIO_PIN_SET);
}
/************************************************************************************************************************
* Function Name: Delay_ms
* Function ID[hex]: 0x05
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): time of delay in ms
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Private Delay Function
************************************************************************************************************************/
static void Delay_ms(uint32_t delay_ms)
{
	uint32_t SystemClockFreq = HAL_RCC_GetHCLKFreq();
	uint32_t cycles_per_ms = SystemClockFreq / 16000;
	uint32_t total_cycles = cycles_per_ms * delay_ms;
	for (volatile uint32_t i = 0; i < total_cycles; ++i);
	return;
}
