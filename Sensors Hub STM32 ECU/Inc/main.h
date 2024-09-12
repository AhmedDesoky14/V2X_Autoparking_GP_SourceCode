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
#include "stm32f1xx_hal.h"

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
#define CAN_Success_LED_Pin GPIO_PIN_14
#define CAN_Success_LED_GPIO_Port GPIOC
#define Shock_Sensor_Pin GPIO_PIN_1
#define Shock_Sensor_GPIO_Port GPIOA
#define Shock_Sensor_EXTI_IRQn EXTI1_IRQn
#define IMU_LED_Pin GPIO_PIN_5
#define IMU_LED_GPIO_Port GPIOA
#define CAN_Fail_LED_Pin GPIO_PIN_1
#define CAN_Fail_LED_GPIO_Port GPIOB
#define GY521_SCL_Pin GPIO_PIN_10
#define GY521_SCL_GPIO_Port GPIOB
#define GY521_SDA_Pin GPIO_PIN_11
#define GY521_SDA_GPIO_Port GPIOB
#define Front_Light_H_Pin GPIO_PIN_12
#define Front_Light_H_GPIO_Port GPIOB
#define Front_Light_L_Pin GPIO_PIN_13
#define Front_Light_L_GPIO_Port GPIOB
#define Back_Light_H_Pin GPIO_PIN_14
#define Back_Light_H_GPIO_Port GPIOB
#define Back_Light_L_Pin GPIO_PIN_15
#define Back_Light_L_GPIO_Port GPIOB
#define Right_Light_Pin GPIO_PIN_8
#define Right_Light_GPIO_Port GPIOA
#define Left_Light_Pin GPIO_PIN_9
#define Left_Light_GPIO_Port GPIOA
#define Awaiting_Light_Pin GPIO_PIN_10
#define Awaiting_Light_GPIO_Port GPIOA
#define Inter_Light_3_Pin GPIO_PIN_4
#define Inter_Light_3_GPIO_Port GPIOB
#define Inter_Light_2_Pin GPIO_PIN_5
#define Inter_Light_2_GPIO_Port GPIOB
#define Inter_Light_1_Pin GPIO_PIN_6
#define Inter_Light_1_GPIO_Port GPIOB
#define GY271_SCL_Pin GPIO_PIN_8
#define GY271_SCL_GPIO_Port GPIOB
#define GY271_SDA_Pin GPIO_PIN_9
#define GY271_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
