/************************************************************************************************************************
 * 	Module: Sensors CAN Application
 * 	File Name: ultrasonic_can_app.h
 *  Authors: Ahmed Desoky
 *	Date: 21/7/2024
 *	*********************************************************************************************************************
 *	Description: This application module is designed to be called in Qt main entry function or any entry
 *               point to the program to continously listen for CAN messages concerning read speed and
 *               steering angle values from encoders connected to another ECU in the CAN network and store
 *               them in certain files acting as pipes. and continously send current set point of vehicle
 *               speed and steering through CAN to the ECU controlling the motors.
 ***********************************************************************************************************************/
#ifndef Sensors_CAN_APP_H
#define Sensors_CAN_APP_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "can_service_manager_rasp.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.0.0*/
#define SENSORS_CAN_APP_SW_MAJOR_VERSION           (1U)
#define SENSORS_CAN_APP_SW_MINOR_VERSION           (0U)
#define SENSORS_CAN_APP_SW_PATCH_VERSION           (0U)

/*Software Version checking of Dependent Module - CAN Service Manager - Version 1.0.0*/
#if ((CAN_SM_SW_MAJOR_VERSION != (1U))\
 ||  (CAN_SM_SW_MINOR_VERSION != (0U))\
 ||  (CAN_SM_SW_PATCH_VERSION != (0U)))
  #error "The Software version of CAN Service Manager Module does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     							      FILES PATHS
 ***********************************************************************************************************************/
#define US0_File                  (const char*)("./files/Read_US_0.txt")
#define US1_File                  (const char*)("./files/Read_US_1.txt")
#define US2_File                  (const char*)("./files/Read_US_2.txt")
#define US3_File                  (const char*)("./files/Read_US_3.txt")
#define US0_State_File            (const char*)("./files/US_State_0.txt")
#define US1_State_File            (const char*)("./files/US_State_1.txt")
#define US2_State_File            (const char*)("./files/US_State_2.txt")
#define US3_State_File            (const char*)("./files/US_State_3.txt")
#define Gyroscope_File            (const char*)("./files/Gyroscope.txt")
#define Accelerometer_File        (const char*)("./files/Accelerometer.txt")
#define Magnetometer_File         (const char*)("./files/Magnetometer.txt")
#define Temperature_File          (const char*)("./files/Temperature.txt")
#define Shock_State_File          (const char*)("./files/Shock_State.txt")
#define Err_Log                   (const char*)("./files/Errlog.txt")
/************************************************************************************************************************
 *                     							      CAN MESSAGES IDs
 ***********************************************************************************************************************/
#define DEVICE_ID                         99
#define Ultrasonic_01_MessageID           0x001
#define Ultrasonic_23_MessageID           0x002
#define Gyroscope_MessageID               0x003
#define Accelerometer_MessageID           0x004
#define Magnetometer_MessageID            0x005
#define Temperature_MessageID             0x021
#define Shock_MessageID                   0x020
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define Error_str_size                  60
#define Files_content_size              20
#define FILE_MAX_SIZE_BYTES             52428800 /*50MBytes*/         
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
typedef enum
{
  SENSORS_APP_OK,SENSORS_APP_NOT_OK
}SENSORS_CAN_APP_STATUS;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
SENSORS_CAN_APP_STATUS Sensors_CAN_APP_Start(void);
#endif /*Sensors_CAN_APP_H*/
