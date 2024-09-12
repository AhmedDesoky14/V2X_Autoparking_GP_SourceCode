/************************************************************************************************************************
 * 	Module: GY-521
 * 	File Name: gy521.h
 *  Authors: Ahmed Desoky
 *	Date: 27/10/2023
 *	*********************************************************************************************************************
 *	Description: Driver Header File.
 *				 GY-521 IMU Driver on STM32F103C8T6 Micro-controller.
 *				 Micro-controller Drivers are STM32 HAL Drivers.
 *	*********************************************************************************************************************
 *	This Module is programmed to support (without configuring) as default the following features.
 *	#Sampling Rate = 1KHz.
 *	#This Driver uses I2C for memory write/read in blocking mode.
 *	#The module must be placed face up for proper functionality
 ***********************************************************************************************************************/
#ifndef GY521_H
#define GY521_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "stm32f1xx_hal.h"
#include "gy521_cfg.h"
#include "gy521_registers.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.2.3*/
#define GY521_SW_MAJOR_VERSION           (1U)
#define GY521_SW_MINOR_VERSION           (2U)
#define GY521_SW_PATCH_VERSION           (3U)
/*Software Version checking between GY521 Configuration file and Header file*/
#if ((GY521_SW_MAJOR_VERSION != GY521_CFG_SW_MAJOR_VERSION)\
 ||  (GY521_SW_MINOR_VERSION != GY521_CFG_SW_MINOR_VERSION)\
 ||  (GY521_SW_PATCH_VERSION != GY521_CFG_SW_PATCH_VERSION))
  #error "The Software version of GY521 does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define GYROSCOPE_REGISTERS_NUMBER			6
#define ACCELEROMETER_REGISTERS_NUMBER		6
#define	TEMPERATURE_REGISTERS_NUMBER		2
#define CHECK_AVAILABILITY_TRIALS_DEFAULT	10
#define TIMEOUT_DEFAULT						100
#define BYTE								1
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
typedef enum
{
	TEMPERATURE_DATA,
	GYROSCOPE_DATA,
	ACCELEROMETER_DATA
}DATA_TYPE;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
HAL_StatusTypeDef GY_521_Init(I2C_HandleTypeDef* hi2c);
HAL_StatusTypeDef GY_521_Gyroscope_Read(float* Value);
HAL_StatusTypeDef GY_521_Accelerometer_Read(float* Value);
HAL_StatusTypeDef GY_521_Temperature_Read(float* Value);
void GY_521_ERROR_INDICATE(void);
void GY_521_FUNCTION_INDICATE(void);

#endif /*GY521_H*/
