/************************************************************************************************************************
 * 	Module: GY-271
 * 	File Name: gy271_registers.h
 *  Authors: Ahmed Desoky
 *	Date: 13/12/2023
 *	*********************************************************************************************************************
 *	Description: Driver Registers File.
 *				 GY-271 Magnetometer Driver on STM32F103C8T6 Micro-controller.
 *				 Micro-controller Drivers are STM32 HAL Drivers.
 *	*********************************************************************************************************************
 *	This Module is programmed to support (without configuring) as default the following features.
 *	#The module must be placed face up for proper functionality
 *	#Output Datarate = 200Hz
 *	#Mainly created to get yaw angle
 ***********************************************************************************************************************/
#ifndef GY271_REGS_H
#define GY271_REGS_H
/************************************************************************************************************************
 *                     						        	    REGISTERS
 ***********************************************************************************************************************/
/*To understand each register use and function, check HMC5883L Datasheet*/
#define MAGNET_XOUT_L			0x00	/*Read Only Register*/
#define MAGNET_XOUT_H			0x01	/*Read Only Register*/
#define MAGNET_YOUT_L			0x02	/*Read Only Register*/
#define MAGNET_YOUT_H			0x03	/*Read Only Register*/
#define MAGNET_ZOUT_L			0x04	/*Read Only Register*/
#define MAGNET_ZOUT_H			0x05	/*Read Only Register*/
#define STATUS_REG				0x06	/*Read Only Register*/
#define CONTROL_REG_A			0x09
#define CONTROL_REG_B			0x0A
#define SET_RESET_PERIOD_REG	0x0B	/*Write 0x01 to this register*/
#define ID_REG					0x0D	/*It returns 0xFF*/
#define GY271_ADDRESS			0x1A	/*0x1A*/
#define OSR_FILTER_MASK			6
#endif /*GY271_REGS_H*/
