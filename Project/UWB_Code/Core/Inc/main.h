/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOA
#define KEY_Pin GPIO_PIN_1
#define KEY_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOA
#define LED3_Pin GPIO_PIN_3
#define LED3_GPIO_Port GPIOA
#define UWB_CS_Pin GPIO_PIN_4
#define UWB_CS_GPIO_Port GPIOA
#define UWB_CLK_Pin GPIO_PIN_5
#define UWB_CLK_GPIO_Port GPIOA
#define UWB_MISO_Pin GPIO_PIN_6
#define UWB_MISO_GPIO_Port GPIOA
#define UWB_MOSI_Pin GPIO_PIN_7
#define UWB_MOSI_GPIO_Port GPIOA
#define UWB_IRQ_Pin GPIO_PIN_0
#define UWB_IRQ_GPIO_Port GPIOB
#define USBD_N_Pin GPIO_PIN_11
#define USBD_N_GPIO_Port GPIOA
#define USBD_P_Pin GPIO_PIN_12
#define USBD_P_GPIO_Port GPIOA
#define UWB_EXTON_Pin GPIO_PIN_15
#define UWB_EXTON_GPIO_Port GPIOA
#define UWB_WAKEUP_Pin GPIO_PIN_3
#define UWB_WAKEUP_GPIO_Port GPIOB
#define UWB_RST_Pin GPIO_PIN_4
#define UWB_RST_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
