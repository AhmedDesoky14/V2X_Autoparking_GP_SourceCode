/************************************************************************************************************************
 * 	Module: CAN Bus
 * 	File Name: CAN_Bus_cfg
 *  Authors: Ziad Emad
 *	Date: 28/2/2024
 *	*********************************************************************************************************************
 *	Description:
 *	STM32F103 CAN handler for Generated STM32 CAN driver using STM Cube MX
 *  CAN Rx IDs Filter can be configured by following this blog
 *	URL: https://schulz-m.github.io/2017/03/23/stm32-can-id-filter/
 ***********************************************************************************************************************/
#ifndef CAN_HEADER_FILE_H
#define CAN_HEADER_FILE_H
/************************************************************************************************************************
 *                     							             INCLUDES
 ***********************************************************************************************************************/
#include "stm32f1xx_hal.h"
#include "can_stm_cfg.h"
/************************************************************************************************************************
 *                     				          VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
#define VENDOR_ID	(2024U)
/*Current Version 2.0.0*/
#define CAN_BUS_SW_MAJOR_VERSION           (2U)
#define CAN_BUS_SW_MINOR_VERSION           (0U)
#define CAN_BUS_SW_PATCH_VERSION           (0U)
/*Software Version checking between Module Configuration file and Header file*/
#if ((CAN_BUS_SW_MAJOR_VERSION != CAN_BUS_CFG_SW_MAJOR_VERSION)\
 ||  (CAN_BUS_SW_MINOR_VERSION != CAN_BUS_CFG_SW_MINOR_VERSION)\
 ||  (CAN_BUS_SW_PATCH_VERSION != CAN_BUS_CFG_SW_PATCH_VERSION))
  #error "The Software version of CAN module does not match the configurations expected version"
#endif
/************************************************************************************************************************
 *                     							      CONSTANT DEFINITIONS
 ***********************************************************************************************************************/
#define PAYLOAD_SIZE 8
#define LED_HIGH GPIO_PIN_SET
#define LED_LOW GPIO_PIN_RESET
/************************************************************************************************************************
 *                     						          MODULE DATA TYPES
 ***********************************************************************************************************************/
/*****************************************************************/
/************** CAN Activation Enums******************************/
/*****************************************************************/
typedef enum {
	CAN_BUS_MODE_SENDING_ONLY,
	CAN_BUS_MODE_RECEIVING_ONLY,
	CAN_BUS_MODE_FULL_DUPLEX
} CAN_Bus_Mode;

typedef enum {
	CAN_FILTER_NOT_ACTIVATED,
	CAN_FILTER_ACTIVATED,
} CAN_Filter_State;

typedef enum {
	CAN_BUS_ACTIVATED,
	CAN_BUS_NOT_ACTIVATED,
	CAN_BUS_RESET,
	CAN_BUS_FILTER_READY,
	CAN_BUS_FILTER_NOT_READY,
} CAN_BUS_Status;
/*****************************************************************/
/***************** CAN Sending Enums******************************/
/*****************************************************************/
typedef enum {
	CAN_TX_HEADER_READY,
	CAN_TX_HEADER_NOT_READY
} CAN_TX_Header_Status;

typedef enum {
	CAN_PACKET_PAYLOAD_SINGLE_BYTE=1,
	CAN_PACKET_PAYLOAD_DOUBLE_BYTES,
	CAN_PACKET_PAYLOAD_THREE_BYTES,
	CAN_PACKET_PAYLOAD_FOUR_BYTES,
	CAN_PACKET_PAYLOAD_FIVE_BYTES,
	CAN_PACKET_PAYLOAD_SIX_BYTES,
	CAN_PACKET_PAYLOAD_SEVEN_BYTES,
	CAN_PACKET_PAYLOAD_EIGHT_BYTES,
} CAN_Packet_Size;

typedef enum {
	CAN_BUS_SENDING_FAILURE,
	CAN_BUS_SENDING_SUCCEFULL

} CAN_Sending_Status;
/*****************************************************************/
/***************** CAN Receiving Enums****************************/
/*****************************************************************/
typedef enum {
	CAN_BUS_RECIEVED_MSG_EMPTY_ID,
	CAN_BUS_RECIEVED_MSG_OUTRANGE_ID,
	CAN_BUS_RECIEVED_ERROR,
	CAN_BUS_RECIEVED_CORRECT,
} CAN_Recieving_Status;

/*****************************************************************/
/***************** LED Indicator Enum ****************************/
/*****************************************************************/
typedef enum {
	MODULE_ERROR_EXIST,
	MODULE_ERROR_SOLVED,
	MODULE_OK,
	CAN_SENDING_ERROR,
	CAN_SENDING_SUCCEFULL,
	CAN_RECIEVING_ERROR,
	CAN_RECIEVING_SUCCEFULL,
} LED_Indicator_Status;
/*****************************************************************/
/***************** Tx Header Struct *******************/
/*****************************************************************/
typedef struct {
	uint32_t StdId ;
	uint32_t ExtId;
	uint32_t IDE;
	uint8_t RTR;
	uint32_t DLC ;
	uint8_t TransmitGlobalTime ;
} CAN_TxHeader;
/*****************************************************************/
/***************** Filter Configuration Header *******************/
/*****************************************************************/
typedef struct {

	uint32_t FilterBank;
	uint32_t FilterIdHigh;
	uint32_t FilterIdLow;
	uint32_t FilterMaskIdHigh;
	uint32_t FilterMaskIdLow;
	uint32_t FilterFIFOAssignment;
	uint32_t FilterMode;
	uint32_t FilterScale;
	uint32_t FilterActivation;
}CAN_Filter;
/************************************************************************************************************************
 *                     				             Functions Prototypes
 ***********************************************************************************************************************/
/*Functions Description in source file*/
CAN_BUS_Status Activate_CAN_Bus(CAN_HandleTypeDef hcan, CAN_Bus_Mode mode);
CAN_Sending_Status CAN_SendPacket(uint8_t* Packet, CAN_Packet_Size packetSize,uint32_t packetId);
CAN_Recieving_Status CAN_RecievePacket_Callback(void);
CAN_Recieving_Status CAN_Get_Rx_Packet(uint8_t* Packet_Ptr,uint8_t* packetSize,uint32_t packetId);
CAN_Recieving_Status CAN_Check_Inbox(uint32_t packetId);
/************************************************************************************************************************
 *                    							   External Variables
 ***********************************************************************************************************************/
extern CAN_TxHeader CAN_TxHeaderParamters ;
extern CAN_Filter CAN_FilterParamters ;
extern uint32_t RxMessageIdList[NUMBER_OF_RX_IDS];
#endif /*HEADER_FILE_H*/
