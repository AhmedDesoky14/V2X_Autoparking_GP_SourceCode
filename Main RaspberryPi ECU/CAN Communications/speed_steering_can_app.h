/************************************************************************************************************************
 * 	Module: Speed/Steering CAN Application
 * 	File Name: speed_steering_can_app.h
 *  Authors: Ahmed Desoky
 *	Date: 18/7/2024
 *	*********************************************************************************************************************
 *	Description: This application module is designed to be called in Qt main entry function or any entry
 *               point to the program to continously listen for CAN messages concerning read speed and
 *               steering angle values from encoders connected to another ECU in the CAN network and store
 *               them in certain files acting as pipes. and continously send current set point of vehicle
 *               speed and steering through CAN to the ECU controlling the motors.
 ***********************************************************************************************************************/
#ifndef SS_CAN_APP_H
#define SS_CAN_APP_H
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
#define SS_CAN_APP_SW_MAJOR_VERSION           (1U)
#define SS_CAN_APP_SW_MINOR_VERSION           (0U)
#define SS_CAN_APP_SW_PATCH_VERSION           (0U)

/*Software Version checking of Dependent Module - CAN Service Manager - Version 1.0.0*/
#if ((CAN_SM_SW_MAJOR_VERSION != (1U))\
 ||  (CAN_SM_SW_MINOR_VERSION != (0U))\
 ||  (CAN_SM_SW_PATCH_VERSION != (0U)))
  #error "The Software version of CAN Service Manager Module does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     							      FILES PATHS
 ***********************************************************************************************************************/
#define Set_Speed_File                  (const char*)("./files/Set_Speed.txt")
#define Set_Steering_File               (const char*)("./files/Set_Steering.txt")
#define Read_Speed_File                 (const char*)("./files/Read_Speed.txt")
#define Read_Steering_File               (const char*)("./files/Read_Steering.txt")
#define Err_Log                         (const char*)("./files/Errlog.txt")
/************************************************************************************************************************
 *                     							      CAN MESSAGES IDs
 ***********************************************************************************************************************/
#define DEVICE_ID                         99
#define Set_Speed_MessageID               0x009
#define Set_Steering_Angle_MessageID      0x010
#define Read_Speed_MessageID              0x007
#define Read_Steering_Angle_MessageID     0x008
#define Speed_Encoder_Error_MessageID      0x018
#define Steering_Encoder_Error_MessageID   0x019
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
  SS_APP_OK,SS_APP_NOT_OK
}SS_CAN_APP_STATUS;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
SS_CAN_APP_STATUS Speed_Steering_CAN_APP_Start(void);
#endif /*SS_CAN_APP_H*/
