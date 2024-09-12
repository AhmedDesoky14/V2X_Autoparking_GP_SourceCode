/* remove this comment after use.
   Versions are set only for header files and configurations files.
   Version Check is only done in header files, as implemented in this file.
*/
/************************************************************************************************************************
 * 	Module:
 * 	File Name:
 *  Authors:
 *	Date:
 *	*********************************************************************************************************************
 *	Description:
 *
 *
 ***********************************************************************************************************************/
#ifndef HEADER_FILE_H
#define HEADER_FILE_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/

/************************************************************************************************************************
 *                     							             INCLUDES
***********************************************************************************************************************/
//#define COM_SECURED /*Comment this line if you don't want to include and use communications security*/
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include "tcp_client.h"
#include "wifi_manager.h"

#ifdef COM_SECURED
#include "SecOC.h"
#endif
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 0.0.0*/
#define MODULE_NAME_SW_MAJOR_VERSION           (0U)
#define MODULE_NAME_SW_MINOR_VERSION           (0U)
#define MODULE_NAME_SW_PATCH_VERSION           (0U)
/*Software Version checking between Module Configuration file and Header file*/
#if ((MODULE_NAME_SW_MAJOR_VERSION != MODULE_NAME_CFG_SW_MAJOR_VERSION)\
 ||  (MODULE_NAME_SW_MINOR_VERSION != MODULE_NAME_CFG_SW_MINOR_VERSION)\
 ||  (MODULE_NAME_SW_PATCH_VERSION != MODULE_NAME_CFG_SW_PATCH_VERSION))
  #error "The Software version of (MODULE NAME) does not match the configurations expected version"
#endif
/*remove this comment after use, if there is no Post Build Configurations file, remove the following check statement*/
/*Software Version checking of Dependent Module - MODULE_NAMEX(Another Module) - Version 0.0.0*/
#if ((MODULE_NAMEX_SW_MAJOR_VERSION != (0U))\
 ||  (MODULE_NAMEX_SW_MINOR_VERSION != (0U))\
 ||  (MODULE_NAMEX_SW_PATCH_VERSION != (0U)))
  #error "The Software version of (MODULE_NAMEX) Module does not match the expected version"
#endif
/*Software Version checking of Dependent Module - MODULE_NAMEX(Another Module) - Version 0.0.0*/
#if ((MODULE_NAMEX_SW_MAJOR_VERSION != (0U))\
 ||  (MODULE_NAMEX_SW_MINOR_VERSION != (0U))\
 ||  (MODULE_NAMEX_SW_PATCH_VERSION != (0U)))
  #error "The Software version of (MODULE_NAMEX) Module does not match the expected version"
#endif
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define START_CONNECTION                    (char*)("V2I:CONNECT:")
#define END_CONNECTION                      (char*)("V2I:DISCONNECT")
#define SECURITY_PARAMETER                  (char*)("+Security:")
#define SECURITY_PARAMETER_SIZE             10
#define MAX_SECURED_MESSAGE_SIZE            512
#define V2I_RX_MAX_NO_BUFFER                5
#define V2I_COMMUNICATIONS_PRIORITY         31
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
typedef enum
{
  V2I_OK,V2I_NOT_OK,TCP_ERROR,WiFi_ERROR,SECURITY_ERROR
}V2I_CLIENT_STATUS;
/************************************************************************************************************************
 *                     						                MACROS
 ***********************************************************************************************************************/

/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
V2I_CLIENT_STATUS V2I_Search(string Infrastructure);
V2I_CLIENT_STATUS V2I_Connect(string Infrastructure,string Password,string IP_Address,\
            unsigned short Port,unsigned int Infrastructure_ID,unsigned int Vehicle_ID);
V2I_CLIENT_STATUS V2I_Disconnect(void);
V2I_CLIENT_STATUS V2I_Send_Message(string Message/*,SecurityLevel Security_Level*/);
V2I_CLIENT_STATUS V2I_Read_Message(string &Message);
V2I_CLIENT_STATUS V2I_Check_Inbox(void);
V2I_CLIENT_STATUS V2I_Check_Connection(void); 

/************************************************************************************************************************
 *                    							   External Variables
 ***********************************************************************************************************************/


#endif /*HEADER_FILE_H*/
