/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "flashdata.hpp"
#include "DS18B20.h"
#include "FryMode.hpp"
#include "Fram.hpp"
#include "Control.hpp"
#include "Heat.hpp"
#include "Button.hpp"
#include "melody_gpio.hpp"
#include "Protection.hpp"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

vu8 buf_485[20];//Массив для посылки в дисплей
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
//Добавил комментарии
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_SPI3_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Stop_IT(&htim7);
	LL_SPI_Enable(SPI3);
	//---------------------Сброс HC595--------------------------------

	GPIOC->BSRR = GPIO_PIN_6; //Latch on
	GPIOC->BSRR = GPIO_PIN_6 << 16U; //Latch off
	GPIOA->BSRR = GPIO_PIN_8; //MR on
	//---------------------------------------------------------------
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_ADC_Start_DMA(&hadc1, Control::adcTemp, 2);
	HAL_Delay(1000); //Без этой паузы не работает восстановление режима приготовления при пропадании питания
	HAL_TIM_Base_Start(&htim3);
	HAL_ADC_Start(&hadc2);
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);
	Sensor_init();
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	//HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	GPIOC->BSRR = GPIO_PIN_4;	//DIR на передачу
	GPIOA->BSRR = GPIO_PIN_12 << 16U;	//Свет выкл
	//GPIOA->BSRR= GPIO_PIN_8 << 16U;	//HC595 выкл
	//__HAL_DBGMCU_FREEZE_IWDG(); //Останавливаем WatchDog в дебаггере
	TIM2->CNT = 180;
	Protection::Init();
	vu8 fram5rd = Fram::elementFram(5); //Читаем из FRAM
	Button button;
	Control control;
	FryModeLambda fryModeLambda;
	if(fram5rd){
		Control::backInfo();//Восстанавливаем работу
	}
	else{
/*-------------TIMERS - Нужно проверить

 TIM1 - Мелодия
 TIM2 - Энкодер
 TIM3 - Прерывание для ADC1
 TIM4- Часы 1 sec
 TIM5 - Защита от длительного времени включенных тенов(эксперимент пока)
 TIM6 - Прерывание для проверки максимальной температуры (10sec)
 TIM7 -



 -------------GPIO's---------------------------------------------

 A0 - adc_modeCook| B0 - Door     | C7 - Grill   | D2 - EXTI_4
 A1 - adc_temp    | B1 - Music    | C8 - Right
 A2 - adc_t_out   | B2 - EXTI_2   | C9 - Down
 A5 - Fdata       | B3 - Enc_2    | C10- CSK
 A7 - FCLK        | B5 - MOSI     | C12- Button
 A8 - Fan_in      | B8 - CS_595   | C13- Sync
 A9 - TXD1        | B15- Sp
 A10- RXD1
 A12- Ligh
 A13- SWDIO
 A14- SWCLK
 A15- Enc1
 ---------------Заполнение массива UART для отсылки в дисплей----------------
 1 - min          = buf_485[1]
 2 - dec_min      = buf_485[2]
 3 - one_h        = buf_485[3]
 4 - dec_h        = buf_485[4]
 5 - temp1        = buf_485[5]
 6 - temp2        = buf_485[6]
 7 - temp3        = buf_485[7]
 8 - tempset1     = buf_485[8]
 9 - tempset2     = buf_485[9]
 10- tempset3     = buf_485[10]
 11- set          = buf_485[11] - режим
 12- watch        = buf_485[12] - точки в часах
 13- fire         = buf_485[13]
 14- fire_90      = buf_485[14]
 15- fire_180     = buf_485[15]
 16- tmp_plt1     = buf_485[16]
 17- tmp_plt2     = buf_485[17]
 18- mode_2       = buf_485[18] - рамка вокруг часов

 ---------------------------------Fram память---------------------------------------
	0 - Заданная температура
	1 - Номер режима
	2 - Минуты
	3 - Часы
	4 - Направление счета времени
	5 - Сброс во время работы
	6 - Старший байт счетчика минут
	7 - Младший байт счетчика минут
 ------------------------------------------------------------------------------------------*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//HAL_TIM_Base_Start_IT(&htim7);
HAL_Delay(500);
//HAL_TIM_Base_Stop_IT(&htim7);
GPIOB->BSRR = GPIO_PIN_15 << 16U; //Спикер выкл
//Button button;
//Control control;
//FryModeLambda fryModeLambda;
//MelodyPlayer::playPodmoskovnye();
EXTI->IMR &= ~EXTI_IMR_MR15;//Запрещаем прерывание EXTI15
GPIOB->BSRR = GPIO_PIN_9;//Led
GPIOB->BSRR = GPIO_PIN_15 << 16U; //Спикер выкл
//GPIOC->BSRR = GPIO_PIN_7 << 16U; // HC595 вкл
//MelodyPlayer::playPodmoskovnye();
	}

while (1) {
			uint16_t modeCookAveADC = control.readAdc(1);//Читаем задатчик режима
			fryModeLambda.ModeSetLambda(modeCookAveADC);//Задаем режим приготовления (вызов лямбды по индексу)
			Fram::elementFram(1, modeCookAveADC);
     		Button::encCount();//Задаем температуру
			Fram::elementFram(0);
			if(Fram::elementFram(1) != 0){//Если режим не Off ...
				button.scanButton();//... сканируем кнопку и ...
				button.executeButtonRegim(button.buttonRegim);//...выбираем режим кнопки
			}
			Heat::setOutCooler(); //Вкл-выкл. внешнего кулера
			buf_485[11] = modeCookAveADC;
			HAL_UART_Transmit_IT(&huart3, buf_485, 20);//Передаем на дисплей
			HAL_Delay(100);			//Без этой паузы дисплей не успевает
			//HAL_IWDG_Refresh(&hiwdg); //Обнуляем watchdog
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_5)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_8, 168, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(168000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
/* User can add his own implementation to report the HAL error return state */
__disable_irq();
while (1) {
}
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
