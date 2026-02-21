/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
//extern uint32_t adcTemp[ ];
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
#define Ioright_Pin GPIO_PIN_13
#define Ioright_GPIO_Port GPIOC
#define Iodown_Pin GPIO_PIN_14
#define Iodown_GPIO_Port GPIOC
#define Iogrl_Pin GPIO_PIN_15
#define Iogrl_GPIO_Port GPIOC
#define Rcc1_Pin GPIO_PIN_0
#define Rcc1_GPIO_Port GPIOH
#define Rcc2_Pin GPIO_PIN_1
#define Rcc2_GPIO_Port GPIOH
#define ResC0_Pin GPIO_PIN_0
#define ResC0_GPIO_Port GPIOC
#define Mode_Pin GPIO_PIN_1
#define Mode_GPIO_Port GPIOC
#define Temp_Pin GPIO_PIN_2
#define Temp_GPIO_Port GPIOC
#define ResC3_Pin GPIO_PIN_3
#define ResC3_GPIO_Port GPIOC
#define ResA0_Pin GPIO_PIN_0
#define ResA0_GPIO_Port GPIOA
#define OE_Pin GPIO_PIN_1
#define OE_GPIO_Port GPIOA
#define Data_Pin GPIO_PIN_2
#define Data_GPIO_Port GPIOA
#define ResA3_Pin GPIO_PIN_3
#define ResA3_GPIO_Port GPIOA
#define ResA4_Pin GPIO_PIN_4
#define ResA4_GPIO_Port GPIOA
#define ResA5_Pin GPIO_PIN_5
#define ResA5_GPIO_Port GPIOA
#define ResA6_Pin GPIO_PIN_6
#define ResA6_GPIO_Port GPIOA
#define ResA7_Pin GPIO_PIN_7
#define ResA7_GPIO_Port GPIOA
#define Dir3_Pin GPIO_PIN_4
#define Dir3_GPIO_Port GPIOC
#define ResA8_Pin GPIO_PIN_5
#define ResA8_GPIO_Port GPIOC
#define FCS_Pin GPIO_PIN_0
#define FCS_GPIO_Port GPIOB
#define Door_Pin GPIO_PIN_1
#define Door_GPIO_Port GPIOB
#define ResB2_Pin GPIO_PIN_2
#define ResB2_GPIO_Port GPIOB
#define TX3_Pin GPIO_PIN_10
#define TX3_GPIO_Port GPIOB
#define RX3_Pin GPIO_PIN_11
#define RX3_GPIO_Port GPIOB
#define FData_Pin GPIO_PIN_12
#define FData_GPIO_Port GPIOB
#define FCLK_Pin GPIO_PIN_13
#define FCLK_GPIO_Port GPIOB
#define ResB14_Pin GPIO_PIN_14
#define ResB14_GPIO_Port GPIOB
#define ResB15_Pin GPIO_PIN_15
#define ResB15_GPIO_Port GPIOB
#define Latch_Pin GPIO_PIN_6
#define Latch_GPIO_Port GPIOC
#define FanOut_Pin GPIO_PIN_7
#define FanOut_GPIO_Port GPIOC
#define Light_Pin GPIO_PIN_8
#define Light_GPIO_Port GPIOC
#define MR_Pin GPIO_PIN_9
#define MR_GPIO_Port GPIOC
#define TX1_Pin GPIO_PIN_9
#define TX1_GPIO_Port GPIOA
#define RX1_Pin GPIO_PIN_10
#define RX1_GPIO_Port GPIOA
#define ResA11_Pin GPIO_PIN_11
#define ResA11_GPIO_Port GPIOA
#define ResA12_Pin GPIO_PIN_12
#define ResA12_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define Enc1_Pin GPIO_PIN_15
#define Enc1_GPIO_Port GPIOA
#define SCK_Pin GPIO_PIN_10
#define SCK_GPIO_Port GPIOC
#define ResC11_Pin GPIO_PIN_11
#define ResC11_GPIO_Port GPIOC
#define Off_Pin GPIO_PIN_12
#define Off_GPIO_Port GPIOC
#define ResD2_Pin GPIO_PIN_2
#define ResD2_GPIO_Port GPIOD
#define Enc2_Pin GPIO_PIN_3
#define Enc2_GPIO_Port GPIOB
#define ResB4_Pin GPIO_PIN_4
#define ResB4_GPIO_Port GPIOB
#define MOSI_Pin GPIO_PIN_5
#define MOSI_GPIO_Port GPIOB
#define ThreeTen_Pin GPIO_PIN_6
#define ThreeTen_GPIO_Port GPIOB
#define ResB7_Pin GPIO_PIN_7
#define ResB7_GPIO_Port GPIOB
#define Button_Pin GPIO_PIN_8
#define Button_GPIO_Port GPIOB
#define Led_Pin GPIO_PIN_9
#define Led_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define Latch Latch_GPIO_Port,Latch_Pin
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
