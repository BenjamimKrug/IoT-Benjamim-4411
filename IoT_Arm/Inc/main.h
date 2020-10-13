/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define V2_Pin GPIO_PIN_0
#define V2_GPIO_Port GPIOC
#define V3_Pin GPIO_PIN_1
#define V3_GPIO_Port GPIOC
#define ESW_Pin GPIO_PIN_0
#define ESW_GPIO_Port GPIOA
#define H1_Pin GPIO_PIN_1
#define H1_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define H2_Pin GPIO_PIN_4
#define H2_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define ALARM_Pin GPIO_PIN_6
#define ALARM_GPIO_Port GPIOA
#define B4_Pin GPIO_PIN_7
#define B4_GPIO_Port GPIOA
#define V1_Pin GPIO_PIN_0
#define V1_GPIO_Port GPIOB
#define J2_Pin GPIO_PIN_2
#define J2_GPIO_Port GPIOB
#define C1_Pin GPIO_PIN_10
#define C1_GPIO_Port GPIOB
#define B5_Pin GPIO_PIN_7
#define B5_GPIO_Port GPIOC
#define B3_Pin GPIO_PIN_8
#define B3_GPIO_Port GPIOA
#define B6_Pin GPIO_PIN_9
#define B6_GPIO_Port GPIOA
#define H3_Pin GPIO_PIN_10
#define H3_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define J1_Pin GPIO_PIN_15
#define J1_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define B8_Pin GPIO_PIN_4
#define B8_GPIO_Port GPIOB
#define B7_Pin GPIO_PIN_5
#define B7_GPIO_Port GPIOB
#define CLK_Pin GPIO_PIN_6
#define CLK_GPIO_Port GPIOB
#define DT_Pin GPIO_PIN_7
#define DT_GPIO_Port GPIOB
#define B1B8_Pin GPIO_PIN_8
#define B1B8_GPIO_Port GPIOB
#define B2_Pin GPIO_PIN_9
#define B2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
