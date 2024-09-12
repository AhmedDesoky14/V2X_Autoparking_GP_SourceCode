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
#ifndef CAN_CFG_H_
#define CAN_CFG_H_
/************************************************************************************************************************
 *                     				    VENDOR & SOFTWARE VERSION CONTROL
 ***********************************************************************************************************************/
/*Current Version 2.0.0*/
#define CAN_BUS_CFG_SW_MAJOR_VERSION           (2U)
#define CAN_BUS_CFG_SW_MINOR_VERSION           (0U)
#define CAN_BUS_CFG_SW_PATCH_VERSION           (0U)
/************************************************************************************************************************
 *                            			     PRE-BUILD CONFIGURATIONS
 ***********************************************************************************************************************/
#define NUMBER_OF_RX_IDS 5
#define SUCCESS_LED_PORT GPIOC
#define WARNING_LED_PORT GPIOB
#define SUCCESS_LED_PIN GPIO_PIN_14
#define WARNING_LED_PIN GPIO_PIN_1

#endif /*CAN_CFG_H_*/
