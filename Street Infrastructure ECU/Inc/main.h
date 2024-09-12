/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define System_Reset_LED_Pin GPIO_PIN_13
#define System_Reset_LED_GPIO_Port GPIOC
#define ESP8266_Init_LED_Pin GPIO_PIN_14
#define ESP8266_Init_LED_GPIO_Port GPIOC
#define Alarm_Buzzer_Pin GPIO_PIN_3
#define Alarm_Buzzer_GPIO_Port GPIOA
#define Alarm_LED_Pin GPIO_PIN_4
#define Alarm_LED_GPIO_Port GPIOA
#define Alarm_Reset_Push_Button_Pin GPIO_PIN_5
#define Alarm_Reset_Push_Button_GPIO_Port GPIOA
#define Alarm_Reset_Push_Button_EXTI_IRQn EXTI9_5_IRQn
#define ESP8266_AP_LED_Pin GPIO_PIN_13
#define ESP8266_AP_LED_GPIO_Port GPIOB
#define UART6_TX___ESP8266_RX_Pin GPIO_PIN_11
#define UART6_TX___ESP8266_RX_GPIO_Port GPIOA
#define UART6_RX___ESP8266_TX_Pin GPIO_PIN_12
#define UART6_RX___ESP8266_TX_GPIO_Port GPIOA
#define ESP8266_Wi_Fi_Connected_LED_Pin GPIO_PIN_8
#define ESP8266_Wi_Fi_Connected_LED_GPIO_Port GPIOB
#define ESP8266_Internet_LED_Pin GPIO_PIN_9
#define ESP8266_Internet_LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
