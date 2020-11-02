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
// ajout Free-RTOS v4.0
// v2.0 ajout yMOTOR

/*	H bridge  ---- MC33886 ----
 *           IN1   IN2   D1   /D2
 * Forward    H     L    0    pwm
 * Reverse    L     H    0    pwm
 * FreeLow    L     L    0    pwm
 * FreeH      H     H    0    pwm
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

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
#define yCubeMX "RTOS v6.4"
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define MotDin1_Pin GPIO_PIN_0
#define MotDin1_GPIO_Port GPIOH
#define MotDin2_Pin GPIO_PIN_1
#define MotDin2_GPIO_Port GPIOH
#define VRx_Pin GPIO_PIN_2
#define VRx_GPIO_Port GPIOC
#define VRy_Pin GPIO_PIN_3
#define VRy_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define SWxy_Pin GPIO_PIN_2
#define SWxy_GPIO_Port GPIOB
#define SWxy_EXTI_IRQn EXTI2_IRQn
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define MotDpwm_Pin GPIO_PIN_7
#define MotDpwm_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define yPROG "*** yRobot ***"
#define yVER  "v3.5a"
#define yDATE "xx-Oct-2020"
//#define yDBG_tk 1        //debug display on terminal (ou commenter ligne)


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
