/************************************************************************************************************************
 *  Module: ultrasonic
 *  File Name: ultrasonic.c
 *  Authors: Esraa Fawzy
 *  Date: Feb 20, 2024
 ************************************************************************************************************************
 *  Description:
 *    The ultrasonic module's functionalities are implemented in the `ultrasonic.c` source file.
 *    It has functions for reading distance values, starting measurements, initializing sensors, and managing
 *    timer input capture callbacks. The file depends on the STM32 HAL library and uses static variables to
 *    hold sensor-related data. To estimate distances accurately, these features work with ultrasonic sensors.
 ************************************************************************************************************************/
/***********************************************************************************************************************
 *                                                 INCLUDES
 ***********************************************************************************************************************/
#include "ultrasonic.h"
/***********************************************************************************************************************
 *                                             GLOBAL VARIABLES
 ***********************************************************************************************************************/
static uint32_t captureValue1[ULTRASONIC_NUMBERS] = {0};
static uint32_t captureValue2[ULTRASONIC_NUMBERS] = {0};
static uint32_t difference[ULTRASONIC_NUMBERS] = {0};
static uint8_t isFirstCaptured[ULTRASONIC_NUMBERS] = {0};
static float distance[ULTRASONIC_NUMBERS] = {0};
static uint8_t ready_flag = 0;
/***********************************************************************************************************************
 *                                          PRIVATE FUNCTIONS PROTOTYPES
 ***********************************************************************************************************************/
static void delay(TIM_HandleTypeDef *htim, uint16_t time);
static void ULTRASONIC_trigger(uint8_t ultrasonic_ID);
/***********************************************************************************************************************
 *                                        FUNCTIONS DEFINITIONS
 ***********************************************************************************************************************/
/************************************************************************************************************************
 * Function Name: delay
 * Function ID[hex]: 0x01
 * Sync/Async: Synchronous
 * Reentrancy:Non-Reentrant
 * Parameters (in): time -> time to delay, index ->Index of the ultrasonic sensor
 * Parameters (inout):None
 * Parameters (out):None
 * Return value:void
 * Description:
 *  This function implements a delay to trigger ultrasonic sensors, dependent on the timer that the ultrasonic sensors are
 *  connected to.
 ************************************************************************************************************************/
static void delay(TIM_HandleTypeDef *htim, uint16_t time)
{
	__HAL_TIM_SET_COUNTER(htim, 0);
	while (__HAL_TIM_GET_COUNTER(htim) < time);
}
/************************************************************************************************************************
 * Function Name: ULTRASONIC_init
 * Function ID[hex]: 0x00
 * Sync/Async:Synchronous
 * Reentrancy:Non-Reentrant
 * Parameters (in): htim -> Timer handle,
 * 					channel -> Timer channel,
 * 					inttruptsource -> Interrupt source,
 * 					trigPort ->GPIO port for trigger,
 * 					trigPin -> GPIO pin for trigger,
 *                  index ->Index of the ultrasonic sensor
 * Parameters (inout):None
 * Parameters (out):None
 * Return value:void
 * Description:
 *   This function initializes an ultrasonic sensor with the specified parameters.
 *   htim can take values {htim1,htim2,htim3..etc},
 *   channel can take values {TIM_CHANNEL_1,TIM_CHANNEL_2,...etc},
 *   inttruptsource can take values {TIM_IT_CC1,TIM_IT_CC2,...etc},
 *   trigPort can be any port and trigPin can be any pin .
 ************************************************************************************************************************/
ULTRASONIC_STATUS ULTRASONIC_init(void)
{
	uint8_t index;
	HAL_StatusTypeDef HAL_Status;
	for(index=0;index< ULTRASONIC_NUMBERS ;++index)
	{
		HAL_Status = HAL_TIM_IC_Start_IT( ultrasonicConfigs[index].timer, ultrasonicConfigs[index].channelID);
		if(HAL_Status != HAL_OK)
		{
			return US_NOT_OK;
		}
	}
	return US_OK;
}
/************************************************************************************************************************
 * Function Name: HAL_TIM_IC_CaptureCallback
 * Function ID[hex]: 0x01
 * Sync/Async: Synchronous
 * Reentrancy: Non-Reentrant
 * Parameters (in): htim -> Timer handle,
 * Parameters (inout):None
 * Parameters (out):None
 * Return value:void
 * Description:
 *   STM HAL Callback function that executes ultrasonics callback function to do its function
 *   First 2 ultrasonics are configured to the same timer at 2 ICU channels, channel 1 and channel 2 respectively
 *   Last 2 ultrasonics are configured to the same timer at 2 ICU channels, channel 1 and channel 2 respectively
 ************************************************************************************************************************/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
#if(ULTRASONIC_NUMBERS > 0)
	if(htim->Instance == ultrasonicConfigs[0].timer->Instance)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			TIM_IC_CaptureCallback_ultrasonic(htim,0);
		}
	}
#endif
#if(ULTRASONIC_NUMBERS > 1)
	if(htim->Instance == ultrasonicConfigs[1].timer->Instance)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			TIM_IC_CaptureCallback_ultrasonic(htim,1);
		}
	}
#endif
#if(ULTRASONIC_NUMBERS > 2)
	if(htim->Instance == ultrasonicConfigs[2].timer->Instance)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			TIM_IC_CaptureCallback_ultrasonic(htim,2);
		}
	}
#endif
#if(ULTRASONIC_NUMBERS > 3)
	if(htim->Instance == ultrasonicConfigs[3].timer->Instance)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			TIM_IC_CaptureCallback_ultrasonic(htim,4);
		}
	}
#endif
}
/************************************************************************************************************************
 * Function Name: TIM_IC_CaptureCallback_ultrasonic
 * Function ID[hex]: 0x02
 * Sync/Async:Asynchronously
 * Reentrancy:Non-Reentrant
 * Parameters (in): htim -> Timer handle, index -> Index of the ultrasonic sensor
 * Parameters (inout):None
 * Parameters (out):None
 * Return value:void
 * Description:
 *   This function handles the timer input capture callback specific to ultrasonic sensors.
 ************************************************************************************************************************/
void TIM_IC_CaptureCallback_ultrasonic(TIM_HandleTypeDef *htim, uint8_t index) {
	/* Check if this is the first capture*/
	if (isFirstCaptured[index] == 0)
	{
		/* Read the first captured value from the timer*/
		captureValue1[index] = HAL_TIM_ReadCapturedValue(htim, ultrasonicConfigs[index].channelID);
		/* Mark that the first capture has been done*/
		isFirstCaptured[index] = 1;
		/* Set the capture polarity to falling edge*/
		__HAL_TIM_SET_CAPTUREPOLARITY(htim, ultrasonicConfigs[index].channelID, TIM_INPUTCHANNELPOLARITY_FALLING);
	}
	/*If this is the second capture*/
	else if (isFirstCaptured[index] == 1)
	{
		/* Read the second captured value from the timer*/
		captureValue2[index] = HAL_TIM_ReadCapturedValue(htim, ultrasonicConfigs[index].channelID);
		/* Reset the timer counter*/
		__HAL_TIM_SET_COUNTER(htim, 0);
		/*Calculate the difference between the two captured values*/
		if (captureValue2[index] > captureValue1[index])
		{
			difference[index] = captureValue2[index] - captureValue1[index];
		}
		/*Check if overflow occur*/
		else if (captureValue1[index] > captureValue2[index])
		{
			difference[index] = (0xFFFF - captureValue1[index]) + captureValue2[index];
		}
		/*Reset the isFirstCaptured flag*/
		isFirstCaptured[index] = 0;
		/*Set the capture polarity to rising edge*/
		__HAL_TIM_SET_CAPTUREPOLARITY(htim, ultrasonicConfigs[index].channelID, TIM_INPUTCHANNELPOLARITY_RISING);
		/* Disable the timer interrupt*/
		__HAL_TIM_DISABLE_IT(htim, ultrasonicConfigs[index].interruptSource);
		ready_flag = 1;	/*reading is ready*/
	}
}

/************************************************************************************************************************
 * Function Name: ULTRASONIC_trigger
 * Function ID[hex]: 0x03
 * Sync/Async:Synchronous
 * Reentrancy:Non-Reentrant
 * Parameters (in): index - Index of the ultrasonic sensor
 * Parameters (inout):None
 * Parameters (out):None
 * Return value:void
 * Description:
 *  Triggers the ultrasonic sensor associated with the specified index to initiate distance measurement by setting the
 *  trigger pin high for 10 microseconds.
 ************************************************************************************************************************/
static void ULTRASONIC_trigger(uint8_t ultrasonic_ID)
{
	HAL_GPIO_WritePin(ultrasonicConfigs[ultrasonic_ID].trigPort, ultrasonicConfigs[ultrasonic_ID].trigPin, GPIO_PIN_SET);
	delay(ultrasonicConfigs[ultrasonic_ID].timer, 10);
	HAL_GPIO_WritePin(ultrasonicConfigs[ultrasonic_ID].trigPort, ultrasonicConfigs[ultrasonic_ID].trigPin, GPIO_PIN_RESET);
	__HAL_TIM_ENABLE_IT(ultrasonicConfigs[ultrasonic_ID].timer, ultrasonicConfigs[ultrasonic_ID].interruptSource);
}
/************************************************************************************************************************
 * Function Name: ULTRASONIC_readdistance
 * Function ID[hex]: 0x04
 * Sync/Async:Synchronous
 * Reentrancy:Non-Reentrant
 * Parameters (in): index - Index of the ultrasonic sensor
 * Parameters (inout):None
 * Parameters (out):None
 * Return value: Distance measurement in centimeters
 * Description:
 *   This function triggers the specified ultrasonic sensor to perform distance measurement and calculates the
 *   distance based on the captured response.
 ************************************************************************************************************************/
ULTRASONIC_STATUS ULTRASONIC_readDistance(uint8_t ultrasonic_ID,float* dist)
{
	uint32_t SystemClockFreq = HAL_RCC_GetHCLKFreq();
	uint32_t cycles_per_ms = SystemClockFreq / 16000;
	uint32_t total_cycles = cycles_per_ms*Trigger_Timeout_ms;
	ULTRASONIC_trigger(ultrasonic_ID);
	while((total_cycles) && (!ready_flag))
	{
		total_cycles--;
		/*timeout*/
		if(total_cycles == 0)
		{
			*dist = 0;
			return US_NOT_OK;
		}
	}
	ready_flag = 0;
	if (ultrasonic_ID < ULTRASONIC_NUMBERS)
	{
		distance[ultrasonic_ID] = difference[ultrasonic_ID] * 0.034 / 2;
	}
	*dist = distance[ultrasonic_ID];
	return US_OK;
}
