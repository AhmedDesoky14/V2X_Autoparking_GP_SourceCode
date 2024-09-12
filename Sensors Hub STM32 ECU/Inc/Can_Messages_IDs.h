
/************************************************************************************************************************
 * 	Module:CAN
 * 	File Name:Can_Messages_IDs
 *  Authors: Dina Hamed
 *	Date:30/6/2024
 *	*********************************************************************************************************************
 *	Description: This file defines specific ID's to the Mostly common used Can messages
 ***********************************************************************************************************************/
#ifndef CAN_MESSAGES_IDS_H
#define CAN_MESSAGES_IDS_H
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 1.0.0*/
#define CAN_MESSAGES_IDS_SW_MAJOR_VERSION           (1U)
#define CAN_MESSAGES_IDS_SW_MINOR_VERSION           (0U)
#define CAN_MESSAGES_IDS_SW_PATCH_VERSION           (0U)

/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define ZERO_VALUE                0x000
#define GyroscopeReadingsID       0x003
#define AccelerometerReadingsID   0x004
#define MagnetometerReadingsID    0x005
#define TemperatureReadingsID     0x021
#define ShockSensorCrashID        0x020
#define HaltMessageID             0x006
#define BreakHaltMessageID        0x011

#endif /*HEADER_FILE_H*/
