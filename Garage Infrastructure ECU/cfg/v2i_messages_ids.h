/************************************************************************************************************************
 * 	Module: V2I Module Messages IDs file
 * 	File Name: V2I_Messages_IDs.h
 *  Authors: Ahmed Desoky
 *	Date: 29/6/2024
 *	*********************************************************************************************************************
 *	Description: File that contains all V2I messages IDs to check and respond to it
 ***********************************************************************************************************************/
#ifndef V2I_MESSAGES_IDS_H_
#define V2I_MESSAGES_IDS_H_
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
/*Current Version 0.0.0*/
#define V2I_MESSAGES_IDS_SW_MAJOR_VERSION           (0U)
#define V2I_MESSAGES_IDS_SW_MINOR_VERSION           (0U)
#define V2I_MESSAGES_IDS_SW_PATCH_VERSION           (0U)
/************************************************************************************************************************
 *                            			     Messages, Requests and Send
 ***********************************************************************************************************************/
/*Infrastructure Messages*/
#define REQUEST_PARKING			  			10
#define REQUEST_GARAGE						11
#define REQUEST_MAP						  	12
#define REQUEST_WEATHER				      	14
#define REQUEST_SPEED_LIMIT					13
#define REQUEST_TRAFFIC_CONDITION			15
#define REQUEST_EMERGENCY_VEHICLE			118
#define REQUEST_ACCIDENT_ISSUE				910
#define SEND_CURRENT_LOCATION				50
#define SEND_ACCIDENT_ISSUE					911
#define SEND_EMERGENCY_VEHICLE				199
#endif /*V2I_MESSAGES_IDS_H_*/
