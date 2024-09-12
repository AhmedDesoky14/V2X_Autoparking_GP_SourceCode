/************************************************************************************************************************
 * 	Module: GY-521
 * 	File Name: gy521_registers.h
 *  Authors: Ahmed Desoky
 *	Date: 27/10/2023
 *	*********************************************************************************************************************
 *	Description: Driver Registers File.
 *				 GY-521 IMU Driver on STM32F103C8T6 Micro-controller.
 *				 Micro-controller Drivers are STM32 HAL Drivers.
 *	*********************************************************************************************************************
 *	This Module is programmed to support (without configuring) as default the following features.
 *	#Sampling Rate = 1KHz.
 *	#This Driver uses I2C for memory write/read in blocking mode.
 *	#The module must be placed face up for proper functionality
 ***********************************************************************************************************************/
#ifndef GY521_REGISTERS_H
#define GY521_REGISTERS_H
/************************************************************************************************************************
 *                     						        	    REGISTERS
 ***********************************************************************************************************************/
/*To understand each register use and function, check MPU60X0 Registers Map*/
#define SMPLRT_DIV							0x19
#define CONFIG 								0X1A
#define GYRO_CONFIG							0x1B
#define ACCEL_CONFIG						0x1C
#define I2C_MST_STATUS						0x36
#define INT_PIN_CFG							0x37
#define INT_ENABLE							0x38
#define INT_STATUS 							0x3A
#define ACCEL_XOUT_H						0x3B
#define ACCEL_XOUT_L						0x3C
#define ACCEL_YOUT_H						0x3D
#define ACCEL_YOUT_L						0x3E
#define ACCEL_ZOUT_H						0x3F
#define ACCEL_ZOUT_L						0x40
#define GY521_TEMP_OUT_H					0x41
#define GY521_TEMP_OUT_L					0x42
#define GYRO_XOUT_H							0x43
#define GYRO_XOUT_L							0x44
#define GYRO_YOUT_H							0x45
#define GYRO_YOUT_L							0x46
#define GYRO_ZOUT_H							0x47
#define GYRO_ZOUT_L							0x48
#define SIGNAL_PATH_RES						0x68
#define USER_CTRL							0x6A
#define PWR_MGMT_1							0x6B
#define PWR_MGMT_2							0x6C
#define WHO_AM_I							0x75
#if(AD0_PIN_LEVEL == 1)
#define GY521_ADDRESS						0xD2
#elif(AD0_PIN_LEVEL == 0)
#define GY521_ADDRESS						0xD0
#endif
#endif /*GY521_REGISTERS_H*/
