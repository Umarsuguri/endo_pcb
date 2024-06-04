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
#define cam_UART huart3
#define main_UART huart1
#define BTN1_Pin GPIO_PIN_0
#define BTN1_GPIO_Port GPIOA
#define BTN2_Pin GPIO_PIN_1
#define BTN2_GPIO_Port GPIOA
#define BTN3_Pin GPIO_PIN_2
#define BTN3_GPIO_Port GPIOA
#define M2R_Pin GPIO_PIN_3
#define M2R_GPIO_Port GPIOA
#define M2F_Pin GPIO_PIN_4
#define M2F_GPIO_Port GPIOA
#define BTN5_Pin GPIO_PIN_6
#define BTN5_GPIO_Port GPIOA
#define BTN6_Pin GPIO_PIN_7
#define BTN6_GPIO_Port GPIOA
#define M1R_Pin GPIO_PIN_0
#define M1R_GPIO_Port GPIOB
#define M1F_Pin GPIO_PIN_1
#define M1F_GPIO_Port GPIOB
#define BTN7_Pin GPIO_PIN_8
#define BTN7_GPIO_Port GPIOA
#define BTN8_Pin GPIO_PIN_11
#define BTN8_GPIO_Port GPIOA
#define BTN9_Pin GPIO_PIN_12
#define BTN9_GPIO_Port GPIOA
#define END1_Pin GPIO_PIN_15
#define END1_GPIO_Port GPIOA
#define ENC1_Pin GPIO_PIN_3
#define ENC1_GPIO_Port GPIOB
#define ENC1_EXTI_IRQn EXTI3_IRQn
#define END2_Pin GPIO_PIN_4
#define END2_GPIO_Port GPIOB
#define ENC2_Pin GPIO_PIN_5
#define ENC2_GPIO_Port GPIOB
#define ENC2_EXTI_IRQn EXTI9_5_IRQn
#define BTN4_Pin GPIO_PIN_6
#define BTN4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
