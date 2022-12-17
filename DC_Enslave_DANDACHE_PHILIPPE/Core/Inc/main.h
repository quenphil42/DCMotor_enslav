/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdlib.h"
#include "commandeMCC.h"
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
#define BUTTON_Pin GPIO_PIN_13
#define BUTTON_GPIO_Port GPIOC
#define BUTTON_EXTI_IRQn EXTI15_10_IRQn
#define ISO_RESET_Pin GPIO_PIN_3
#define ISO_RESET_GPIO_Port GPIOC
#define ADC_Red_Current_Sensor_Pin GPIO_PIN_0
#define ADC_Red_Current_Sensor_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_5
#define LED_GPIO_Port GPIOA
#define T_SWDIO_Pin GPIO_PIN_13
#define T_SWDIO_GPIO_Port GPIOA
#define T_SWCLK_Pin GPIO_PIN_14
#define T_SWCLK_GPIO_Port GPIOA
#define ENC_VOIEA_Pin GPIO_PIN_15
#define ENC_VOIEA_GPIO_Port GPIOA
#define ENC_VOIEB_Pin GPIO_PIN_3
#define ENC_VOIEB_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#define ARR_MAX_VALUE 1024
#define ADC_MAX_VALUE 4096

#define ALPHA_MIN 0		//consigne de commande alpha entre 0 et 100
#define ALPHA_MAX 100

// PI Courant
#define KP_ALPHA 0.1	//0.10
#define KI_ALPHA 0.8	//0.8

#define ALPHA_OUT_MAX_VALUE 0.99
#define ALPHA_OUT_MIN_VALUE 0.01
#define ALPHA_MEM_I 0.5
#define ALPHA_MEM_P 0.0
#define TIM1_PERIOD 0.0000625

// PI Vitesse
#define KP_CURRENT 0.1		//matlab0.72
#define KI_CURRENT 2.0		//matlab15.94

#define CURRENT_OUT_MAX_VALUE	2.0
#define CURRENT_MEM_I 0.0
#define CURRENT_MEM_P 0.0

#define TIM3_PERIOD 1.0
#define TIM4_PERIOD 0.0064

#define TICK2SPEED_TIM4 0.000000001472 // 60.0 / encodeurDefinition /TIM4_PERIOD // 60/4096/TimerDelay    avec TimerDelay(en sec)
#define TICK2SPEED_TIM3 0.00000023 //60.0 / encodeurDefinition / TIM3_PERIOD //(60.0/4096.0)*TIM3_PERIOD

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
