/************************************************************************************************************************
 * 	Module: esp8266
 * 	File Name: esp8266_cfg.h
 *  Authors: Ahmed Desoky
 *	Date: 14/2/2024
 *	*********************************************************************************************************************
 *	Description: ESP8266 Driver Pre-build configurations file
 *				 ESP8266 Driver on STM32F103C8T6 Micro-controller.
 *				 Micro-controller Drivers are STM32 HAL Drivers.
 *				 This Driver is designed to support generally any application it's needed to be used in
 *				 You can skip looking in source file functions if you intend you use it only
 *				 This Driver uses the dedicated UART peripheral resources, and no other device or instance can share it
 ***********************************************************************************************************************/
#ifndef ESP8266_CFG_H_
#define ESP8266_CFG_H_
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
/*Current Version 2.4.4*/
#define ESP8266_CFG_SW_MAJOR_VERSION           (2U)
#define ESP8266_CFG_SW_MINOR_VERSION           (4U)
#define ESP8266_CFG_SW_PATCH_VERSION           (4U)
/************************************************************************************************************************
 *                            			     PRE-BUILD CONFIGURATIONS
 ***********************************************************************************************************************/
/*LEDs are connected in positive logic*/
#define INITIALIZED_CONNECTED_LED_PORT		GPIOB
#define INITIALIZED_CONNECTED_LED_PIN		GPIO_PIN_5
#define ACCESS_POINT_LED_PORT				GPIOB
#define ACCESS_POINT_LED_PIN				GPIO_PIN_13
#define CONNECTED_TO_WIFI_LED_PORT			GPIOB
#define CONNECTED_TO_WIFI_LED_PIN			GPIO_PIN_8
#define INTERNET_CONNECTED_LED_PORT			GPIOB
#define INTERNET_CONNECTED_LED_PIN			GPIO_PIN_9
#define COMMAND_TRIALS_NUMBER				3U	/*Number of times to re-send the command to try to guarantee its execution*/
#define PING_ITERATIONS						5U	/*Number of trials of ping while checking Internet connection*/
#define ESP8266_Rx_Buffers_Sizes			500	/*All Receiving Buffers sizes*/
#define COMMANDS_TIMEOUT_SAFETY_FACTOR		1.5	/*Safety factor for timeout uncertainty to make sure of command execution*/
#define ESP_TAKE_BREATH_ms					10	/*Time to take before any transmission operation of the ESP*/
#define ESP8266_MAX_TCP_CONNECTIONS			1U	/*It's mentioned in ESP8266 AT commands data sheet that
													it accepts up to 4 TCP connections while in TCP Server ON mode,
													it's added to cfg file to change it according to you application*/
/************************************************************************************************************************
 *                            	       STM HAL CUBEX Configurations Notes
 ***********************************************************************************************************************/
/*	1- Guarantee that ESP8266 is connected to 3.3V supply with enough current supplied, ESP8266 needs 300mA
 *	2- Open STM CubeMX, Enable any UART peripheral, enable its DMA reception with high priority chosen and its interrupt
 *	   is not necessary to be ON
 *	3- Enable UART Global Interrupt
 *	4- Copy these lines in HAL_UARTEx_RxEventCallback function, after copying it from stm32f1xx_hal_uart.c file to your code
 *			if (huart->Instance == USART1,2,3,4,5)
			{
				ESP8266_Receive_TCP_Packets_Callback();
			}
 *	5- To learn how to use each function read their descriptions carefully
 *	6- Open Cfg and PBCfg files to configure your option according to your application, NEVER use AP IP Address = 192.168.1.X
 *	7- Include esp8266.h
 *	8- Any other module that reads data from esp8266 using read function must pass a buffer greater than or equal to
 *	   ESP8266 configured buffer size
 *	9- NJoy! :D
 */
#endif /*ESP8266_CFG_H_*/
