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
/***********************************************************************************************************************
 *                     							      INCLUDES
 ***********************************************************************************************************************/
#include <string.h>
#include "can_stm.h"
/***********************************************************************************************************************
 *                     					           GLOBAL VARIABLES
 ***********************************************************************************************************************/
static CAN_HandleTypeDef _hcan; // Initialize The CAN Configariton Struct
static CAN_TxHeaderTypeDef TxHeader; // Initialize Header of transmitted Frame
static CAN_RxHeaderTypeDef RxHeader; // Initialize Header of Received  Frame
static uint8_t TxData[PAYLOAD_SIZE]={0}; // Data Array of 8 Bytes to be sent
static uint32_t TxMailBox={0}; // 4 Byte Packet
static uint8_t RxDataBaseBuffers[NUMBER_OF_RX_IDS][PAYLOAD_SIZE]={0};
static CAN_TX_Header_Status TxHeader_Status =CAN_TX_HEADER_READY;
static uint8_t RxData[PAYLOAD_SIZE]={0}; // Data Array of 8 Bytes to be Received
static uint8_t RxDataBaseBuffers_Size[NUMBER_OF_RX_IDS] = {0};	/*Array to store received data sizes*/
/***********************************************************************************************************************
 *                     					      PRIVATE FUNCTIONS PROTOTYPES
 ***********************************************************************************************************************/
static CAN_TX_Header_Status CAN_SetTxHeaderParamters(CAN_TxHeader* HeaderParamters);
static void CAN_SetFilterConfigurations(CAN_Filter* CAN_FilterParamters);
static void Activate_LED_Indicator(LED_Indicator_Status state) ;
/***********************************************************************************************************************/
/***********************************************************************************************************************
 *                     					        FUNCTIONS DEFINTITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
 * Function Name: Activate_CAN_Bus
 * Function ID[hex]: 0x1
 * Sync/Async: Async
 * Reentrancy: Reentrant
 * ---------------------------------
 * Parameters (in):
 * 	--> CAN_HandleTypeDef hcan 		: Struct include general configuration of CAN BUS
 * 	--> CAN_Bus_Mode mode      		: Choose which mode to operate the CAN Bus with it
 * 	--> CAN_Filter_State filterState: Choose either the Receiving Filter is Activated or not
 * 	--------------------------------
 * Parameters (inout): NAN
 * Parameters (out): NAN
 * Return value:
 * --> CAN_State: Return The Activation Function State of the CAN BUS either (CAN_BUS_ACTIVATED - CAN_BUS_NOT_ACTIVATED)
 *				  based on the CAN_Start Function
 * Description:
 * This function is used to Activate the CAN bus Beside the Original CAN_Init Function from CMSIS , Add More Configuration
 * for CAN Bus to Work
 * CAN Bus Module CAN be Activated by 3 Modes :
 * --> CAN_BUS_MODE_SENDING_ONLY :
 * 		==> In which CAN bus will send packets only & Reception is Terminated
 * 		==> Inserting Initial Header Configuration inside TxHeader
 *
 * --> CAN_BUS_MODE_RECEIVING_ONLY :
 *	    ==> In which CAN bus will receive packets only & Transmitting is Terminated
 * 		==> Activate the Notification Center with Filter bank Through The passed struct CAN_FilterParamters
 *
 * --> CAN_BUS_MODE_FULL_DUPLEX :
 * 		Both The Previous Modes are activated together
 *
 * After that Run the Function CAN_START to Start the CAN BUS
 *
 * Note That CMISS Configuration for CAN BUS (Tested) :
 * Parameter Settings
 * 	Timing Parameters :
 * 		Prescaler : 2
 * 		Time Quanta in Bit 1 : 2 Time
 *	    Time Quanta in Bit 2 : 1 Time
 * 		Resynchnization : 1 Time
 * 		Basic Pramters : All Disable
 * 		Advanced Parameters :
 * 		Test Mode : Normal
 * NVIC Settings:
 * 	Activate the NVIC Interrupt Table in RX0 and RX1
 * GPIO Settings:
 * 	PA11 Rx is Input Mode and Pull Up
 ************************************************************************************************************************/
CAN_BUS_Status Activate_CAN_Bus(CAN_HandleTypeDef hcan, CAN_Bus_Mode mode)
{
	CAN_BUS_Status CAN_State;
	_hcan=hcan;
	switch (mode) {
	case CAN_BUS_MODE_SENDING_ONLY:
	{
		if (CAN_SetTxHeaderParamters(&CAN_TxHeaderParamters)!= CAN_TX_HEADER_READY)
		{
			Activate_LED_Indicator(MODULE_ERROR_EXIST);
		}
	}
	break;
	case CAN_BUS_MODE_RECEIVING_ONLY:
	{
		CAN_SetFilterConfigurations(&CAN_FilterParamters);
		if(HAL_CAN_ActivateNotification(&_hcan, CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK)
		{
			Activate_LED_Indicator(MODULE_ERROR_EXIST);
		}
	}
	break;
	case CAN_BUS_MODE_FULL_DUPLEX:
	{
		if (CAN_SetTxHeaderParamters(&CAN_TxHeaderParamters)!= CAN_TX_HEADER_READY)
		{
			Activate_LED_Indicator(MODULE_ERROR_EXIST);
		}
		CAN_SetFilterConfigurations(&CAN_FilterParamters);
		if(HAL_CAN_ActivateNotification(&_hcan, CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK)
		{
			Activate_LED_Indicator(MODULE_ERROR_EXIST);
		}
	}
	break;
	default:
		break;
	}
	if(HAL_CAN_Start(&_hcan)!=HAL_OK)
	{
		Activate_LED_Indicator(MODULE_ERROR_EXIST);
		CAN_State = CAN_BUS_NOT_ACTIVATED;
	}
	else
	{
		CAN_State = CAN_BUS_ACTIVATED;
	}
	return CAN_State;
}
/************************************************************************************************************************
 * Function Name:CAN_SendPacket
 * Function ID[hex]: 0x2
 * Sync/Async: Async
 * Reentrancy: Reentrant
 * Parameters (in):
 * 	--> uint8_t* Packet 	       : Single array of 0 to 8 Bytes including data To be sent
 * 	--> CAN_Packet_Size packetSize : include the size of the packet itself can have values:
 		CAN_PACKET_PAYLOAD_SINGLE_BYTE=1,
		CAN_PACKET_PAYLOAD_DOUBLE_BYTES,
		CAN_PACKET_PAYLOAD_THREE_BYTES,
		CAN_PACKET_PAYLOAD_FOUR_BYTES,
		CAN_PACKET_PAYLOAD_FIVE_BYTES,
		CAN_PACKET_PAYLOAD_SIX_BYTES,
		CAN_PACKET_PAYLOAD_SEVEN_BYTES,
		CAN_PACKET_PAYLOAD_EIGHT_BYTES,
 * --> uint32_t packetId : The Specific ID Needed of packet to be sent
 * Parameters (inout): NAN
 * Parameters (out):   NAN
 * Return value:
 * CAN_Sending_Status : return the status of the Sent which could be
 * 	CAN_BUS_SENDING_SUCCEFULL,
	CAN_BUS_SENDING_FAILURE,
 * Description:
 *last modification on CAN_packetSize
 * || In This function we insert the incoming data inform of array to TxData that will be merged together
 * with the TxHeader in TxMailBox that will be sent on CANBus
 * || Steps Simply
 * 		Clear Old Buffer
 * 		Check on The Incoming Size and ID
 * 		Assign the New Values in TxData
 * 		Merge TxData with TxHeader in Mailbox
 * 		Finally Check The state of Sending and Return it
 *
 ************************************************************************************************************************/
/***********************************************************************************************************************/
CAN_Sending_Status CAN_SendPacket(uint8_t* Packet, CAN_Packet_Size packetSize,uint32_t packetId)
{
	CAN_Sending_Status currentSendStatus;
	uint8_t byteNumber=0;
	memset(TxData,0,PAYLOAD_SIZE); //Clear the Buffer before sending new one
	//Assigning new Data size and packetId in the TxHeader
	if(IS_CAN_DLC(packetSize) && IS_CAN_STDID(packetId))
	{
		TxHeader.StdId=packetId;
		TxHeader.DLC=packetSize;
	}
	else
	{
		TxHeader_Status=CAN_TX_HEADER_NOT_READY;
		Activate_LED_Indicator(MODULE_ERROR_EXIST);
	}
	//------------------------------------------------------
	//Inserting The Packet to the Buffer of TxData to be sent
	for(byteNumber ; byteNumber < packetSize; byteNumber++)
	{
		TxData[byteNumber]=Packet[byteNumber];
	}
	// Start Sending and Check if the packet sent or not
	if(HAL_CAN_AddTxMessage(&_hcan, &TxHeader, TxData, &TxMailBox)!= HAL_OK)
	{
		currentSendStatus=CAN_BUS_SENDING_FAILURE;
		Activate_LED_Indicator(CAN_SENDING_ERROR);
	}
	else
	{
		currentSendStatus=CAN_BUS_SENDING_SUCCEFULL;
		Activate_LED_Indicator(CAN_SENDING_SUCCEFULL);
	}
	return currentSendStatus;
}
/***********************************************************************************************************************/
/************************************************************************************************************************
 * Function Name: CAN_RecievePacket
 * Function ID[hex]: 0x3
 * Sync/Async: Async
 * Reentrancy: Re-entrant
 * Parameters (in): void
 * Parameters (inout): NAN
 * Parameters (out): NAN
 * Return value: CAN_Recieving_Status Indicate the Reception state it can be either
 * 	CAN_BUS_RECIEVED_MSG_EMPTY_ID,
 *	CAN_BUS_RECIEVED_MSG_OUTRANGE_ID,
 *  CAN_BUS_RECIEVED_ERROR,
 *  CAN_BUS_RECIEVED_CORRECT,
 *
 * Description:
 * || When an Interrupt is triggered and callback function is called, this function will also called
 * inside the callback , it first filters the incoming packets based on the configured Filter bank
 * For example(FIFO0) and save the incoming correct message in Rxbuffers Database
 *
 * ||Flow
 * First Clear the old RxData
 * Then Check if we received the message correctly or not
 * Then if we received it correctly check if the ID is empty or not
 * If the ID is not empty check if the incoming Id Matches the saved IDs or not
 * If The ID Matchs, Put it the incoming packet in the Database 2D array with its specific row (ID)
 *
 ************************************************************************************************************************/
CAN_Recieving_Status CAN_RecievePacket_Callback(void)
{
	//--------------------------
	uint8_t IdIterator=0;
	uint8_t DataIterator=0;
	uint8_t incomingPacketSize=0;
	CAN_Recieving_Status currentRecieveStatus;
	//--------------------------
	memset(RxData,0,PAYLOAD_SIZE); //Clear the Buffer before Receiving new one
	//--------------------------
	if (HAL_CAN_GetRxMessage(&_hcan,CAN_FilterParamters.FilterFIFOAssignment,&RxHeader,RxData) != HAL_OK)
	{
		// Receiver failed to receive the incoming message
		currentRecieveStatus = CAN_BUS_RECIEVED_ERROR;
		Activate_LED_Indicator(CAN_RECIEVING_ERROR);
	}
	else
	{
		// Receiver could received the incoming message successfully
		incomingPacketSize=RxHeader.DLC;
		// Check That the received message with empty ID
		if (RxHeader.StdId==0x0000)
		{
			currentRecieveStatus= CAN_BUS_RECIEVED_MSG_EMPTY_ID;
		}
		else
		{
			for (IdIterator ;IdIterator  < NUMBER_OF_RX_IDS; IdIterator++)
			{
				if (RxHeader.StdId==RxMessageIdList[IdIterator])
				{
					// Clearing the array with incoming ID in RxDataBaseBufffer before parsing the new one
					memset(RxDataBaseBuffers[IdIterator],0,PAYLOAD_SIZE);
					// Parsing the Data from the incoming Buffer to RxDataBaseBufffer
					for(DataIterator;DataIterator<incomingPacketSize ;DataIterator++)
					{
						RxDataBaseBuffers[IdIterator][DataIterator]=RxData[DataIterator];
					}
					RxDataBaseBuffers_Size[IdIterator] = incomingPacketSize;	/*Store received frame size*/
					currentRecieveStatus= CAN_BUS_RECIEVED_CORRECT;
					Activate_LED_Indicator(CAN_RECIEVING_SUCCEFULL);
					break;
				}
				else
				{
					currentRecieveStatus=CAN_BUS_RECIEVED_MSG_OUTRANGE_ID;
				}
			}
			if(currentRecieveStatus != CAN_BUS_RECIEVED_CORRECT)
			{
				// After Iterating on all saved Ids if the message wasnot found light Warning led
				Activate_LED_Indicator(CAN_RECIEVING_ERROR);
			}
		}
	}
	return currentRecieveStatus;
}
/***********************************************************************************************************************/
/************************************************************************************************************************
 * Function Name: CAN_TX_Header_Status
 * Function ID[hex]: 0x4
 * Sync/Async: Async
 * Reentrancy: Reentrant
 * Parameters (in): CAN_TxHeader* HeaderParamters Struct including the Header Configurations
 * Parameters (inout): NAN
 * Parameters (out): NAN
 * Return value: CAN_TX_Header_Status Return The status of adjusting the header Configurations it is either
 * 				 CAN_TX_HEADER_READY,
				 CAN_TX_HEADER_NOT_READY
 * Description:
 * Adjusting the Initial parameters of the Header from configured struct , these paramters are
 * 	StdId : ID of the Message
	ExtId : ExtID of we use the extended mode we can have more variation of the sent IDs
	IDE   : Choose the Mode Either STDID or EXTID
	RTR   : Choose the type either Data or Request Data
	DLC   : Choose the size of the Data to be sent it is from 0 to 8
	TransmitGlobalTime : The Timeout for sending is activated or not (Usually Disabled)
 ************************************************************************************************************************/
static CAN_TX_Header_Status CAN_SetTxHeaderParamters(CAN_TxHeader* HeaderParamters)
{
	/********* Configure Header Frame ************/
	// Before sending data, Its a must to adjust the header info first
	if(IS_CAN_STDID(HeaderParamters->StdId))
	{
		TxHeader.StdId= HeaderParamters->StdId ; // ID of the sent message , helps in filtering the message from other devices
	}
	else
	{
		TxHeader_Status=CAN_TX_HEADER_NOT_READY;
	}
	//-------------------------------------------
	if(IS_CAN_EXTID(HeaderParamters->ExtId))
	{
		TxHeader.ExtId=HeaderParamters->ExtId; // Since using Standard ID --> ExtID = 0
	}
	else
	{
		TxHeader_Status=CAN_TX_HEADER_NOT_READY;
	}
	//-------------------------------------------
	if(IS_CAN_IDTYPE(HeaderParamters->IDE))
	{
		TxHeader.IDE = HeaderParamters->IDE; // Choose the Mode to be Standard ID
	}
	else
	{
		TxHeader_Status=CAN_TX_HEADER_NOT_READY;
	}
	//-------------------------------------------
	if(IS_CAN_DLC(HeaderParamters->DLC))
	{
		TxHeader.DLC = HeaderParamters->DLC; // Insert the Intial size of The Sent Data Frame
	}
	else
	{
		TxHeader_Status=CAN_TX_HEADER_NOT_READY;
	}
	//-------------------------------------------
	if(IS_CAN_RTR(HeaderParamters->RTR))
	{
		TxHeader.RTR=HeaderParamters->RTR; // Sending Data Mode not requesting Data
	}
	else
	{
		TxHeader_Status=CAN_TX_HEADER_NOT_READY;
	}
	//-------------------------------------------
	if(HeaderParamters->TransmitGlobalTime == DISABLE || HeaderParamters->TransmitGlobalTime == ENABLE)

	{
		TxHeader.TransmitGlobalTime= HeaderParamters->TransmitGlobalTime; // Disable The TransmitGlobaltime
	}
	else
	{
		TxHeader_Status=CAN_TX_HEADER_NOT_READY;
	}
	return TxHeader_Status;
}
/***********************************************************************************************************************/
/************************************************************************************************************************
 * Function Name: CAN_SetFilterConfigurations
 * Function ID[hex]: 0x5
 * Sync/Async: Async
 * Reentrancy: Renternat
 * Parameters (in): CAN_Filter* FilterParamters : Struct including the Configuration of CAN Filter
 * Parameters (inout): NAN
 * Parameters (out): NAN
 * Return value: void
 * Description:
 * This function adjust the Configuration of the filter bank to filter the received messages based on it
 * 	 FilterBank    		: Which filterbank you will use (0 to 13)
	 FilterIdHigh; 		: Higher ID Register
	 FilterIdLow   		: Lower ID Register
	 FilterMaskIdHigh   : Higher Mask ID Register if these bits are 1's then they will be compared
	 FilterMaskIdLow;   : Lower Mask ID Register if these bits are 1's then they will be compared
	 FilterFIFOAssignment; : Choose either FIFO0 or FIFO1
 	 FilterMode         : Choose the Mode of Comparing the ID (Mask or Passing on the whole Id and compare it)
	 FilterScale;       : The Size of the ID to be filtered 32 or 64
	 FilterActivation;  : State of the FilterBank (Enabled - Disabled)
 *
 *
 ************************************************************************************************************************/
static void CAN_SetFilterConfigurations(CAN_Filter* FilterParamters)
{
	/********** Configure Filter Parameters **********/
	CAN_FilterTypeDef CANFilter; // Generating an object of CAN filter to adjust its parameters
	CANFilter.FilterBank = FilterParamters->FilterBank ;   					 // Specifies the filter bank which will be initialized
	CANFilter.FilterIdHigh=	FilterParamters->FilterIdHigh;                 // Filter Incoming 22 , while sending above on 11
	CANFilter.FilterIdLow =  FilterParamters->FilterIdLow;
	CANFilter.FilterMaskIdHigh=FilterParamters->FilterMaskIdHigh;
	CANFilter.FilterMaskIdLow =FilterParamters->FilterMaskIdLow;
	CANFilter.FilterFIFOAssignment =FilterParamters->FilterFIFOAssignment;
	CANFilter.FilterMode =FilterParamters->FilterMode;
	CANFilter.FilterScale=FilterParamters->FilterScale;
	CANFilter.FilterActivation= FilterParamters->FilterActivation;   // Enable the filter
	if(	HAL_CAN_ConfigFilter(&_hcan,&CANFilter) !=HAL_OK)
	{
		Activate_LED_Indicator(MODULE_ERROR_EXIST);
	}
}
/************************************************************************************************************************
 * Function Name: Activate_LED_Indicator
 * Function ID[hex]: 0x6
 * Sync/Async: Synchronous
 * Reentrancy: Reentrant
 * Parameters (in): State of the Module that based on it light on or off the LEDS
 * Parameters (inout): NAN
 * Parameters (out): NAN
 * Return value: void
 * Description:
 *	Based on the incoming state of the module certain lEDS will light either on or OFF
 *  these states are :
 *	MODULE_ERROR_EXIST  	--> Warning Led(RED) will be On
 *  MODULE_ERROR_SOLVED,	--> Warning Led(RED) will be OFF
 *	MODULE_OK,              --> Warning Led(RED) will be OFF
 *	CAN_SENDING_ERROR,		--> Success Led(GREEN) will be OFF
 *	CAN_SENDING_SUCCEFULL,  --> Success Led(GREEN) will be Toggle
 *  CAN_RECIEVING_ERROR,    --> Success Led(GREEN) will be OFF
 *	CAN_RECIEVING_SUCCEFULL,--> Success Led(GREEN) will be Toggle
 ************************************************************************************************************************/
static void Activate_LED_Indicator(LED_Indicator_Status state)
{
	switch (state) {
	case MODULE_ERROR_EXIST:
		// Turn Red Led On
		HAL_GPIO_WritePin(WARNING_LED_PORT, WARNING_LED_PIN, LED_HIGH);
		break;
	case MODULE_ERROR_SOLVED:
		// Turn LED off Red
		HAL_GPIO_WritePin(WARNING_LED_PORT, WARNING_LED_PIN, LED_LOW);
		break;
	case CAN_SENDING_ERROR:
		// Switch LED GREEN OFF
	{
		HAL_GPIO_WritePin(SUCCESS_LED_PORT, SUCCESS_LED_PIN, LED_LOW);
		HAL_GPIO_WritePin(WARNING_LED_PORT, WARNING_LED_PIN, LED_HIGH);
//		HAL_GPIO_TogglePin(WARNING_LED_PORT, WARNING_LED_PIN);
	}
	break;
	case CAN_SENDING_SUCCEFULL:
		// Toggle LED GREEN
	{
		HAL_GPIO_WritePin(SUCCESS_LED_PORT, SUCCESS_LED_PIN, LED_HIGH);
		HAL_GPIO_WritePin(WARNING_LED_PORT, WARNING_LED_PIN, LED_LOW);
	}
	break;
	case CAN_RECIEVING_ERROR:
		// Switch Yellow  OFF
	{
		HAL_GPIO_WritePin(SUCCESS_LED_PORT, SUCCESS_LED_PIN, LED_LOW);
		HAL_GPIO_WritePin(WARNING_LED_PORT, WARNING_LED_PIN, LED_HIGH);
	}
	break;
	case CAN_RECIEVING_SUCCEFULL:
		// Toggle yellow light
	{
		HAL_GPIO_WritePin(SUCCESS_LED_PORT, SUCCESS_LED_PIN, LED_HIGH);
		HAL_GPIO_WritePin(WARNING_LED_PORT, WARNING_LED_PIN, LED_LOW);
	}
	break;
	default:
		// Handle unexpected state values (optional)
		break;
	}
}
/************************************************************************************************************************
 * Function Name: CAN_Get_Rx_Packet
 * Function ID[hex]: 0x7
 * Sync/Async: Synchronous
 * Reentrancy: Non-Reentrant
 * Parameters (in): Ptr to array of characters to store the packet Data
 * 					Expected Packet ID
 * Parameters (inout): NAN
 * Parameters (out): Ptr to array of characters to store the packet Data
 * Return value: CAN_Recieving_Status: Receiving Status
 * Description: Function to get and extract the data of expected packet ID
 * 				First it check that the ID is within the configured ID
 * 				Then it fetches the data of this packet ID if the data exists.
 ************************************************************************************************************************/
CAN_Recieving_Status CAN_Get_Rx_Packet(uint8_t* Packet_Ptr,uint8_t* packetSize,uint32_t packetId)
{
	CAN_Recieving_Status currentRecieveStatus;
	for(uint16_t IdIterator = 0;IdIterator<NUMBER_OF_RX_IDS;IdIterator++)
	{
		if(packetId == RxMessageIdList[IdIterator])
		{
			/*Clear the input buffer*/
			memset(Packet_Ptr,0,PAYLOAD_SIZE);
			/*Copy Data*/
			memcpy(Packet_Ptr,RxDataBaseBuffers[IdIterator],RxDataBaseBuffers_Size[IdIterator]);
			*packetSize = RxDataBaseBuffers_Size[IdIterator];
			memset(RxDataBaseBuffers[IdIterator],0,PAYLOAD_SIZE);	/*Clear Buffers*/
			RxDataBaseBuffers_Size[IdIterator] = 0;
			currentRecieveStatus = CAN_BUS_RECIEVED_CORRECT;
			Activate_LED_Indicator(CAN_RECIEVING_SUCCEFULL);
			break;
		}
		else
		{
			currentRecieveStatus = CAN_BUS_RECIEVED_MSG_OUTRANGE_ID;
		}
	}
	if(currentRecieveStatus != CAN_BUS_RECIEVED_CORRECT)
	{
		// After Iterating on all saved IDs if the message was not found light Warning led
		Activate_LED_Indicator(CAN_RECIEVING_ERROR);
	}
	if(*packetSize == 0)
	{
		currentRecieveStatus = CAN_BUS_RECIEVED_ERROR;
	}
	return currentRecieveStatus;
}
/************************************************************************************************************************
 * Function Name: CAN_Check_Inbox
 * Function ID[hex]: 0x8
 * Sync/Async: Synchronous
 * Reentrancy: Non-Reentrant
 * Parameters (in): Expected Packet ID
 * Parameters (inout): NONE
 * Parameters (out): NONE
 * Return value: CAN_Recieving_Status: Receiving Status
 * Description: Function to check receiving inbox of expected packet ID
 * 				First it check that the ID is within the configured ID
 * 				Then it finds if there's new data or not.
 ************************************************************************************************************************/
CAN_Recieving_Status CAN_Check_Inbox(uint32_t packetId)
{
	uint8_t IdIterator = 0;
	for(IdIterator;IdIterator<NUMBER_OF_RX_IDS;IdIterator++)
	{
		if(packetId == RxMessageIdList[IdIterator])
		{
			for(uint8_t k=0;k<PAYLOAD_SIZE;k++)
			{
				if(RxDataBaseBuffers[IdIterator][k] != '\0')
				{
					return CAN_BUS_RECIEVED_CORRECT;	/*data found*/
				}
			}
		}
	}
	return CAN_BUS_RECIEVED_MSG_EMPTY_ID;	/*Inbox Empty*/
}
