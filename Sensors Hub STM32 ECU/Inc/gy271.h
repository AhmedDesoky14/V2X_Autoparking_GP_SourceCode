/************************************************************************************************************************
 * 	Module: GY-271
 * 	File Name: gy271.h
 *  Authors: Ahmed Desoky
 *	Date: 13/12/2023
 *	*********************************************************************************************************************
 *	Description: Driver Header File.
 *				 GY-271 Magnetometer Driver on STM32F103C8T6 Micro-controller.
 *				 Micro-controller Drivers are STM32 HAL Drivers.
 *	*********************************************************************************************************************
 *	This Module is programmed to support (without configuring) as default the following features.
 *	#The module must be placed face up for proper functionality
 *	#Output Datarate = 200Hz
 *	#Mainly created to get yaw angle
 ***********************************************************************************************************************/
#ifndef GY271_H
#define GY271_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "stm32f1xx_hal.h"
#include "gy271_cfg.h"
#include "gy271_registers.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.0.0*/
#define GY271_SW_MAJOR_VERSION           (1U)
#define GY271_SW_MINOR_VERSION           (0U)
#define GY271_SW_PATCH_VERSION           (0U)
/*Software Version checking between Module Configuration file and Header file*/
#if ((GY271_SW_MAJOR_VERSION != GY271_CFG_SW_MAJOR_VERSION)\
 ||  (GY271_SW_MINOR_VERSION != GY271_CFG_SW_MINOR_VERSION)\
 ||  (GY271_SW_PATCH_VERSION != GY271_CFG_SW_PATCH_VERSION))
  #error "The Software version of GY271 does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define MAGNETOMETER_REGISTERS_NUMBER			6
#define CHECK_AVAILABILITY_TRIALS_DEFAULT		10
#define TIMEOUT_DEFAULT							100
#define BYTE									1
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
HAL_StatusTypeDef GY_271_Init(I2C_HandleTypeDef* hi2c);
HAL_StatusTypeDef GY_271_Magnetometer_Read(float* Value);
void GY_271_ERROR_INDICATE(void);
void GY_271_FUNCTION_INDICATE(void);
#endif /*GY271_H*/
