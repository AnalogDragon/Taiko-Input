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
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "string.h"


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

//#define IS_4CH_MODE

#ifndef IS_4CH_MODE
extern uint8_t SET_1;
extern uint8_t SET_2;
extern uint8_t SET_3;
extern uint8_t SET_4;
#endif

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

extern IWDG_HandleTypeDef hiwdg;

extern TIM_HandleTypeDef htim14;

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;


uint8_t CloseIrqAndSave(void);
void CloseIrqAndActive(uint8_t data);


#ifdef IS_4CH_MODE

#define OUT2_Pin GPIO_PIN_14
#define OUT2_GPIO_Port GPIOC
#define OUT3_Pin GPIO_PIN_15
#define OUT3_GPIO_Port GPIOC
#define IN1_Pin GPIO_PIN_0
#define IN1_GPIO_Port GPIOA
#define IN2_Pin GPIO_PIN_1
#define IN2_GPIO_Port GPIOA
#define IN3_Pin GPIO_PIN_2
#define IN3_GPIO_Port GPIOA
#define IN4_Pin GPIO_PIN_3
#define IN4_GPIO_Port GPIOA
#define IN5_Pin GPIO_PIN_4
#define IN5_GPIO_Port GPIOA
#define IN6_Pin GPIO_PIN_5
#define IN6_GPIO_Port GPIOA
#define IN7_Pin GPIO_PIN_6
#define IN7_GPIO_Port GPIOA
#define IN8_Pin GPIO_PIN_7
#define IN8_GPIO_Port GPIOA
#define LED_STA_Pin GPIO_PIN_8
#define LED_STA_GPIO_Port GPIOA
#define OUT1_Pin GPIO_PIN_11
#define OUT1_GPIO_Port GPIOA
#define OUT4_Pin GPIO_PIN_12
#define OUT4_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA

#else


/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OUT1_Pin GPIO_PIN_9
#define OUT1_GPIO_Port GPIOB
#define SET3_Pin GPIO_PIN_14
#define SET3_GPIO_Port GPIOC
#define SET4_Pin GPIO_PIN_15
#define SET4_GPIO_Port GPIOC
#define IN1_Pin GPIO_PIN_0
#define IN1_GPIO_Port GPIOA
#define IN2_Pin GPIO_PIN_1
#define IN2_GPIO_Port GPIOA
#define IN3_Pin GPIO_PIN_2
#define IN3_GPIO_Port GPIOA
#define IN4_Pin GPIO_PIN_3
#define IN4_GPIO_Port GPIOA
#define IN5_Pin GPIO_PIN_4
#define IN5_GPIO_Port GPIOA
#define IN6_Pin GPIO_PIN_5
#define IN6_GPIO_Port GPIOA
#define IN7_Pin GPIO_PIN_6
#define IN7_GPIO_Port GPIOA
#define IN8_Pin GPIO_PIN_7
#define IN8_GPIO_Port GPIOA
#define SET2_Pin GPIO_PIN_0
#define SET2_GPIO_Port GPIOB
#define SET1_Pin GPIO_PIN_1
#define SET1_GPIO_Port GPIOB
#define OUT8_Pin GPIO_PIN_2
#define OUT8_GPIO_Port GPIOB
#define LED_STA_Pin GPIO_PIN_8
#define LED_STA_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define OUT7_Pin GPIO_PIN_3
#define OUT7_GPIO_Port GPIOB
#define OUT6_Pin GPIO_PIN_4
#define OUT6_GPIO_Port GPIOB
#define OUT5_Pin GPIO_PIN_5
#define OUT5_GPIO_Port GPIOB
#define OUT4_Pin GPIO_PIN_6
#define OUT4_GPIO_Port GPIOB
#define OUT3_Pin GPIO_PIN_7
#define OUT3_GPIO_Port GPIOB
#define OUT2_Pin GPIO_PIN_8
#define OUT2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

#endif

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
