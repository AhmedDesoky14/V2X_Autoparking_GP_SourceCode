/************************************************************************************************************************
 * 	Module: GY-521
 * 	File Name: gy521.c
 *  Authors: Ahmed Desoky
 *	Date: 27/10/2023
 *	*********************************************************************************************************************
 *	Description: Driver Source File.
 *				 GY-521 IMU Driver on STM32F103C8T6 Micro-controller.
 *				 Micro-controller Drivers are STM32 HAL Drivers.
 *	*********************************************************************************************************************
 *	This Module is programmed to support (without configuring) as default the following features.
 *	#Sampling Rate = 1KHz.
 *	#This Driver uses I2C for memory write/read in blocking mode.
 *	#The module must be placed face up for proper functionality
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include "gy521.h"
/***********************************************************************************************************************
 *                     					        PRIVATE GLOBAL VARIABLES
 ***********************************************************************************************************************/
static I2C_HandleTypeDef* hi2cx;	/*Initialization Structure to use in all HAL Functions in the source file*/
static float Accelerometer_Offset_Error[3] = {0,0,0};	/*For Accelerometer Initial Calibrations*/
static float Gyroscope_Offset_Error[3] = {0,0,0};		/*For Gyroscope Initial Calibration*/
/***********************************************************************************************************************
 *                     					      PRIVATE FUNCTIONS PROTOTYPES
 ***********************************************************************************************************************/
static HAL_StatusTypeDef GY_521_Extract_Raw_Data(DATA_TYPE Data_Type,int16_t* Data);
static void Delay_ms(uint32_t delay_ms);
/***********************************************************************************************************************
 *                     					        FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
* Function Name: GY_521_Init
* Function ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non-Reentrant
* Parameters (in): I2C Handle Structure for initialization
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: HAL Function Status
* Description: Function to Initialize GY521 Accelerometer & Gyroscope
* 			   and finally calibrate the module initially
* 			   #The module must be in flat plain position while initialization and calibration
************************************************************************************************************************/
HAL_StatusTypeDef GY_521_Init(I2C_HandleTypeDef* hi2c)
{
	hi2cx = hi2c;
	uint8_t Recent_Status = 0;
	uint8_t	Loaded_Register_Value = 0;
	Delay_ms(20);		/*Delay for 20ms*/
	if(HAL_I2C_IsDeviceReady(hi2cx,GY521_ADDRESS,CHECK_AVAILABILITY_TRIALS_DEFAULT,TIMEOUT_DEFAULT) != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}
	Loaded_Register_Value = 0x80;	/*Re-setting the IMU*/
	Delay_ms(10);		/*Delay for 10ms*/
	Recent_Status = HAL_I2C_Mem_Write(hi2cx,GY521_ADDRESS,PWR_MGMT_1,BYTE,&Loaded_Register_Value,BYTE,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}
	Loaded_Register_Value = 0x01;	/*Re-setting the IMU Extended*/
	Delay_ms(10);		/*Delay for 10ms*/
	Recent_Status = HAL_I2C_Mem_Write(hi2cx,GY521_ADDRESS,USER_CTRL,BYTE,&Loaded_Register_Value,BYTE,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}

	Delay_ms(20);		/*Delay for 20ms after reset*/
	Loaded_Register_Value = 0x07;	/*Setting sampling rate = 1KHz*/
	Recent_Status = HAL_I2C_Mem_Write(hi2cx,GY521_ADDRESS,SMPLRT_DIV,BYTE,&Loaded_Register_Value,BYTE,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}

	Loaded_Register_Value = GY521_INTERNAL_DIGITAL_LPF_VALUE;	/*Setting the Digital LPF*/
	Delay_ms(10);		/*Delay for 10ms*/
	Recent_Status = HAL_I2C_Mem_Write(hi2cx,GY521_ADDRESS,CONFIG,BYTE,&Loaded_Register_Value,BYTE,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}

#if(GY521_GYROSCOPE_RANGE == 0)
	Loaded_Register_Value = 0x00;	/*Setting Gyroscope Full Scale Range = +-250 degree/second*/
	Delay_ms(10);		/*Delay for 10ms*/
	Recent_Status = HAL_I2C_Mem_Write(hi2cx,GY521_ADDRESS,GYRO_CONFIG,BYTE,&Loaded_Register_Value,BYTE,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}
#elif(GY521_GYROSCOPE_RANGE == 1)
	Loaded_Register_Value = 0x08;	/*Setting Gyroscope Full Scale Range = +-500 degree/second*/
	Delay_ms(10);		/*Delay for 10ms*/
	Recent_Status = HAL_I2C_Mem_Write(hi2cx,GY521_ADDRESS,GYRO_CONFIG,BYTE,&Loaded_Register_Value,BYTE,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}
#elif(GY521_GYROSCOPE_RANGE == 2)
	Loaded_Register_Value = 0x10;	/*Setting Gyroscope Full Scale Range = +-1000 degree/second*/
	Delay_ms(10);		/*Delay for 10ms*/
	Recent_Status = HAL_I2C_Mem_Write(hi2cx,GY521_ADDRESS,GYRO_CONFIG,BYTE,&Loaded_Register_Value,BYTE,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}
#elif(GY521_GYROSCOPE_RANGE == 3)
	Loaded_Register_Value = 0x18;	/*Setting Gyroscope Full Scale Range = +-2000 degree/second*/
	Delay_ms(10);		/*Delay for 10ms*/
	Recent_Status = HAL_I2C_Mem_Write(hi2cx,GY521_ADDRESS,GYRO_CONFIG,BYTE,&Loaded_Register_Value,BYTE,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}
#endif
#if(GY521_ACCELEROMETER_RANGE == 0)
	Loaded_Register_Value = 0x00;	/*Setting Accelerometer Full Scale Range = +-2g*/
	Delay_ms(10);		/*Delay for 10ms*/
	Recent_Status = HAL_I2C_Mem_Write(hi2cx,GY521_ADDRESS,ACCEL_CONFIG,BYTE,&Loaded_Register_Value,BYTE,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}
#elif(GY521_ACCELEROMETER_RANGE == 1)
	Loaded_Register_Value = 0x08;	/*Setting Accelerometer Full Scale Range = +-4g*/
	Delay_ms(10);		/*Delay for 10ms*/
	Recent_Status = HAL_I2C_Mem_Write(hi2cx,GY521_ADDRESS,ACCEL_CONFIG,BYTE,&Loaded_Register_Value,BYTE,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}
#elif(GY521_ACCELEROMETER_RANGE == 2)
	Loaded_Register_Value = 0x10;	/*Setting Accelerometer Full Scale Range = +-8g*/
	Delay_ms(10);		/*Delay for 10ms*/
	Recent_Status = HAL_I2C_Mem_Write(hi2cx,GY521_ADDRESS,ACCEL_CONFIG,BYTE,&Loaded_Register_Value,BYTE,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}
#elif(GY521_ACCELEROMETER_RANGE == 3)
	Loaded_Register_Value = 0x18;	/*Setting Accelerometer Full Scale Range = +-16g*/
	Delay_ms(10);		/*Delay for 10ms*/
	Recent_Status = HAL_I2C_Mem_Write(hi2cx,GY521_ADDRESS,ACCEL_CONFIG,BYTE,&Loaded_Register_Value,BYTE,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}
#endif

	Loaded_Register_Value = 0x0;	/*Power ON the IMU*/
	Delay_ms(10);		/*Delay for 10ms*/
	Recent_Status = HAL_I2C_Mem_Write(hi2cx,GY521_ADDRESS,PWR_MGMT_1,BYTE,&Loaded_Register_Value,BYTE,TIMEOUT_DEFAULT);
	if(Recent_Status != HAL_OK)
	{
		return HAL_ERROR;	/*Error Occurred*/
	}
	Delay_ms(20);		/*Delay for 20ms to start up*/
	/* Initialization Section End */
	/* Initial Calibration Section Start to reduce reading errors */
	float Accelerometer_Offset_Accumulator_X = 0;
	float Accelerometer_Offset_Accumulator_Y = 0;
	float Accelerometer_Offset_Accumulator_Z = 0;
	float Gyroscope_Offset_Accumulator_X = 0;
	float Gyroscope_Offset_Accumulator_Y = 0;
	float Gyroscope_Offset_Accumulator_Z = 0;
	float Accelerometer_Offset[3] = {0,0,0};
	float Gyroscope_Offset[3]= {0,0,0};
	for(unsigned int i=0 ; i<CALIBRATION_ITERATIONS ; i++)
	{
		/*Go to used functions headlines in the same file to understand them*/
		if(GY_521_Accelerometer_Read(Accelerometer_Offset) != HAL_OK)
		{
			return HAL_ERROR;
		}
		if(GY_521_Gyroscope_Read(Gyroscope_Offset) != HAL_OK)
		{
			return HAL_ERROR;
		}
		Accelerometer_Offset_Accumulator_X += Accelerometer_Offset[0];
		Accelerometer_Offset_Accumulator_Y += Accelerometer_Offset[1];
		Accelerometer_Offset_Accumulator_Z += Accelerometer_Offset[2];
		Gyroscope_Offset_Accumulator_X += Gyroscope_Offset[0];
		Gyroscope_Offset_Accumulator_Y += Gyroscope_Offset[1];
		Gyroscope_Offset_Accumulator_Z += Gyroscope_Offset[2];
		/*Dynamic delay depending on */
		Delay_ms(GY521_GYROSCOPE_RANGE*2+2);
	}
	float default_g = 1.0;	/*After Calibration gravity effect became absent from readings, so this variable is to
 	 	 	 	 	 	 	retrieve its effect to z-axis*/
	Accelerometer_Offset_Error[0] =(Accelerometer_Offset_Accumulator_X/CALIBRATION_ITERATIONS);
	Accelerometer_Offset_Error[1] = Accelerometer_Offset_Accumulator_Y/CALIBRATION_ITERATIONS;
	Accelerometer_Offset_Error[2] = (Accelerometer_Offset_Accumulator_Z/CALIBRATION_ITERATIONS) - default_g;
	Gyroscope_Offset_Error[0] = Gyroscope_Offset_Accumulator_X/CALIBRATION_ITERATIONS;
	Gyroscope_Offset_Error[1] = Gyroscope_Offset_Accumulator_Y/CALIBRATION_ITERATIONS;
	Gyroscope_Offset_Error[2] = Gyroscope_Offset_Accumulator_Z/CALIBRATION_ITERATIONS ;
	GY_521_FUNCTION_INDICATE();
	return HAL_OK;
}
/************************************************************************************************************************
* Function Name: GY_521_Extract_Raw_Data
* Function ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): Data Type needed to be extracted from the registers, array of 16-bit int to store data
* Parameters (inout): NONE
* Parameters (out): array of 16-bit int to store data
* Return value: HAL Function Status
* Description: Function to Extract Raw Data of Gyroscope, Accelerometer and Temperature Sensor.
* 			   This function is private and only used by other functions
************************************************************************************************************************/
static HAL_StatusTypeDef GY_521_Extract_Raw_Data(DATA_TYPE Data_Type,int16_t* Data)
{
	uint8_t Recent_Status = 0;
	if(Data_Type == TEMPERATURE_DATA)	/*Temperature Data*/
	{
		uint8_t Temp_Data[2];
		Recent_Status = HAL_I2C_Mem_Read(hi2cx,GY521_ADDRESS,GY521_TEMP_OUT_H,BYTE,Temp_Data,TEMPERATURE_REGISTERS_NUMBER,TIMEOUT_DEFAULT);
		if(Recent_Status != HAL_OK)
		{
			return HAL_ERROR;	/*Error Occurred*/
		}
		/*Temperature Raw Data*/
		*Data = (int16_t)(Temp_Data[0]<<8 | Temp_Data[1]);
	}
	else if(Data_Type == GYROSCOPE_DATA)	/*Gyroscope Data*/
	{
		uint8_t Temp_Data[6];
		Recent_Status = HAL_I2C_Mem_Read(hi2cx,GY521_ADDRESS,GYRO_XOUT_H,BYTE,Temp_Data,GYROSCOPE_REGISTERS_NUMBER,TIMEOUT_DEFAULT);
		if(Recent_Status != HAL_OK)
		{
			return HAL_ERROR;	/*Error Occurred*/
		}
		Data[0] = (int16_t)(Temp_Data[0]<<8 | Temp_Data[1]);
		Data[1] = (int16_t)(Temp_Data[2]<<8 | Temp_Data[3]);
		Data[2] = (int16_t)(Temp_Data[4]<<8 | Temp_Data[5]);
	}
	else if(Data_Type == ACCELEROMETER_DATA)	/*Accelerometer Data*/
	{
		uint8_t Temp_Data[6];
		Recent_Status = HAL_I2C_Mem_Read(hi2cx,GY521_ADDRESS,ACCEL_XOUT_H,BYTE,Temp_Data,ACCELEROMETER_REGISTERS_NUMBER,TIMEOUT_DEFAULT);
		if(Recent_Status != HAL_OK)
		{
			return HAL_ERROR;	/*Error Occurred*/
		}
		Data[0] = (int16_t)(Temp_Data[0]<<8 | Temp_Data[1]);
		Data[1] = (int16_t)(Temp_Data[2]<<8 | Temp_Data[3]);
		Data[2] = (int16_t)(Temp_Data[4]<<8 | Temp_Data[5]);
	}
	else
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}
/************************************************************************************************************************
* Function Name: GY_521_Gyroscope_Read
* Function ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): array of float variables, to store read gyroscope data
* Parameters (inout): NONE
* Parameters (out): array of float variables, to store read gyroscope data
* Return value: HAL Function Status
* Description: Function to read gyroscope data and store them in the array in degree/second units.
************************************************************************************************************************/
HAL_StatusTypeDef GY_521_Gyroscope_Read(float* Value)
{
	int16_t Data[3];
	if(GY_521_Extract_Raw_Data(GYROSCOPE_DATA,Data) != HAL_OK)
	{
		GY_521_ERROR_INDICATE();
		return HAL_ERROR;	/*Error Occurred*/
	}
	/*Values*/
#if(GY521_GYROSCOPE_RANGE == 0)
	for(unsigned int i=0 ; i<=2 ; i++)
	{
		Value[i] = (Data[i] / 131.0) - Gyroscope_Offset_Error[i];
	}
#elif(GY521_GYROSCOPE_RANGE == 1)
	for(unsigned int i=0 ; i<=2 ; i++)
	{
		Value[i] = (Data[i] / 65.5) - Gyroscope_Offset_Error[i];
	}
#elif(GY521_GYROSCOPE_RANGE == 2)
	for(unsigned int i=0 ; i<=2 ; i++)
	{
		Value[i] = (Data[i] / 32.8) - Gyroscope_Offset_Error[i];
	}
#elif(GY521_GYROSCOPE_RANGE == 3)
	for(unsigned int i=0 ; i<=2 ; i++)
	{
		Value[i] = (Data[i] / 16.4) - Gyroscope_Offset_Error[i];
	}
#endif
	return HAL_OK;
}
/************************************************************************************************************************
* Function Name: GY_521_Accelerometer_Read
* Function ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): array of float variables, to store read accelerometer data
* Parameters (inout): NONE
* Parameters (out): array of float variables, to store read accelerometer data
* Return value: HAL Function Status
* Description: Function to read accelerometer data and store them in the array in g units.
************************************************************************************************************************/
HAL_StatusTypeDef GY_521_Accelerometer_Read(float* Value)
{
	int16_t Data[3];
	if(GY_521_Extract_Raw_Data(ACCELEROMETER_DATA,Data) != HAL_OK)
	{
		GY_521_ERROR_INDICATE();
		return HAL_ERROR;	/*Error Occurred*/
	}
	/*Values*/

#if(GY521_ACCELEROMETER_RANGE == 0)
	for(unsigned int i=0 ; i<=2 ; i++)
	{
		Value[i] = (Data[i] / 16384.0) -  Accelerometer_Offset_Error[i];
	}
#elif(GY521_ACCELEROMETER_RANGE == 1)
	for(unsigned int i=0 ; i<=2 ; i++)
	{
		Value[i] = (Data[i] / 8192.0) -  Accelerometer_Offset_Error[i];
	}
#elif(GY521_ACCELEROMETER_RANGE == 2)
	for(unsigned int i=0 ; i<=2 ; i++)
	{
		Value[i] = (Data[i] / 4096.0) -  Accelerometer_Offset_Error[i];
	}
#elif(GY521_ACCELEROMETER_RANGE == 3)
	for(unsigned int i=0 ; i<=2 ; i++)
	{
		Value[i] = (Data[i] / 2048.0) -  Accelerometer_Offset_Error[i];
	}
#endif
	return HAL_OK;
}
/************************************************************************************************************************
* Function Name: GY_521_Temperature_Read
* Function ID[hex]: 0x04
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): pointer to float variable to store temperature
* Parameters (inout): NONE
* Parameters (out): pointer to float variable to store temperature
* Return value: HAL Function Status
* Description: Function to read temperature in Celsius units.
************************************************************************************************************************/
HAL_StatusTypeDef GY_521_Temperature_Read(float* Value)
{
	int16_t Data;
	if(GY_521_Extract_Raw_Data(TEMPERATURE_DATA,&Data) != HAL_OK)
	{
		GY_521_ERROR_INDICATE();
		return HAL_ERROR;	/*Error Occurred*/
	}
	*Value = (Data/340)+36.53;
	return HAL_OK;
}
/************************************************************************************************************************
* Function Name: GY_521_ERROR_INDICATE
* Function ID[hex]: 0x05
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to turn off functionality LED to indicate error.
* 			   This function can be used by other modules that depend on this one.
************************************************************************************************************************/
void GY_521_ERROR_INDICATE(void)
{
	HAL_GPIO_WritePin(INDICATOR_LED_GY521_PORT,INDICATOR_LED_GY521_PIN,GPIO_PIN_RESET);	/*INDICATOR LED is OFF - ERROR*/
}
/************************************************************************************************************************
* Function Name: GY_521_FUNCTION_INDICATE
* Function ID[hex]: 0x06
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): NONE
* Parameters (inout): NONE
* Parameters (out): NONE
* Return value: NONE
* Description: Function to turn on functionality LED to indicate OK.
* 			   This function can be used by other modules that depend on this one.
************************************************************************************************************************/
void GY_521_FUNCTION_INDICATE(void)
{
	HAL_GPIO_WritePin(INDICATOR_LED_GY521_PORT,INDICATOR_LED_GY521_PIN,GPIO_PIN_SET);
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
