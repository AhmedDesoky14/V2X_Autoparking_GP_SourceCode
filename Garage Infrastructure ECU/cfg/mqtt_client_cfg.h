/************************************************************************************************************************
 * 	Module: MQTT Client
 * 	File Name: mqtt_client_cfg.h
 *  Authors: Ahmed Desoky
 *	Date: 11/3/2024
 *	*********************************************************************************************************************
 *	Description: Module implemented based on Paho MQTT Packet C Library, which is used to serialize MQTT Packets
 *				 This module is based on STM HAL Drivers, ESP8266 Driver and Paho MQTT Packet C Library
 *				 which is a low level library used to serialize and deserialize MQTT Packets
 *				 It deals with MQTT Brokers and topics to publish and subscribe messages
 *				 This module by default supports only QoS=0 Transactions and Non-Retained messages
 *				 This module only support receiving messages in JSON Format
 *				 This module set the keep alive parameter to 6 minutes and every 5 minutes ping to the MQTT broker
 *				 This module is only working when TCP server of ESP8266 is off
 ***********************************************************************************************************************/
#ifndef MQTT_CLIENT_CFG_H_
#define MQTT_CLIENT_CFG_H_
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
/*Current Version 1.2.4*/
#define MQTT_CLIENT_CFG_SW_MAJOR_VERSION           (1U)
#define MQTT_CLIENT_CFG_SW_MINOR_VERSION           (2U)
#define MQTT_CLIENT_CFG_SW_PATCH_VERSION           (4U)
/************************************************************************************************************************
 *                            			     PRE-BUILD CONFIGURATIONS
 ***********************************************************************************************************************/
#define MQTT_Rx_Buffer_Size				500U	/*Rx Buffer Size that store messages from subscribed topics*/
												/*Must be greater that or equal esp8266 Rx buffers*/
#define MQTT_Rx_Last_Messages_Number	3U		/*Number of the last messages to keep, the larger the better*/
#define SEND_BUFFER_SIZE				300U	/*Packet Send Buffer Size*/
#define TOPIC_LENGTH					40U		/*Topic String Length*/
/************************************************************************************************************************
 *                            	       STM HAL CUBEX Configurations Notes
 ***********************************************************************************************************************/
/* 1- Go to ESP8266 configurations Notes and check them
 * 2- Enable Timer4, make sure of that, and enable its global interrupt and make sure in NVIC that timer4
 *    has lower priority than Systick Timer
 * 3- Adjust the prescaler value so that timer4 clock becomes 1.25KHz
 * 4- Add the period value = 37499
 * 5- configure the Pre-Build configurations for your application
 * 6- Copy these lines in HAL_UARTEx_RxEventCallback function, after copying it from stm32f1xx_hal_uart.c file to your code
 *			if (huart->Instance == USART1,2,3,4,5)
			{
				MQTT_Subscribe_Receive_Callback();
			}
 * 7- Copy these lines in HAL_TIM_PeriodElapsedCallback function, after copying it from stm32f1xx_hal_uart.c file to your code
			if(htim == &htim4)
			{
				MQTT_Subscribe_Ping_Callback();
			}
 * 8- Include "mqtt_client.h"
 * 9- NJoy! :D
 */
#endif /*FILE_CFG_H_*/
