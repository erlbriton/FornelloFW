/*
 * CallBacks.cpp
 *
 *  Created on: Mar 31, 2025
 *      Author: erlbriton
 */

#include "CallBacks.hpp"
#include "Control.hpp"
#include "Heat.hpp"
#include "SetTimer.hpp"
#include "melody_gpio.hpp"

//---------------------ADCManager-----------------------------
void ADCManager::handleADCConversionComplete(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
       // Control control;
        //#################################Для калибровки###################################################
//        buf_485[4] = adcTemp[0] / 1000;
//        buf_485[3] = adcTemp[0] / 100 % 10;
//        buf_485[2] = adcTemp[0] / 10 % 10;EXTI
//        buf_485[1] = adcTemp[0] % 10;
//        HAL_UART_Transmit_IT(&huart3, buf_485, 20);
//        HAL_Delay(1000);
        //#################################Конец калибровки###################################################
        Control::bakeTemper();
    }
}
//---------------------TimerManager-----------------------------

// Массив структур, которые связывают таймеры и их обработчики
const TimerManager::TimerMap TimerManager::timerMap[ ] = {
	{ TIM4,  &TimerManager::handleTIM4  },
    { TIM5,  &TimerManager::handleTIM5  },
    { TIM6,  &TimerManager::handleTIM6  },
    //{ TIM7,  &TimerManager::handleTIM7  },
    { TIM9,  &TimerManager::handleTIM9  },
    { TIM10, &TimerManager::handleTIM10 },
	//{ TIM11, &TimerManager::handleTIM11 },
    { TIM12, &TimerManager::handleTIM12 },
};

// Обработчик прерывания таймера
void TimerManager::handleTimerInterrupt(TIM_HandleTypeDef* htim) {
    TimerHandler handler = nullptr;
    // Поиск соответствующего обработчика для таймера
    for (const auto& entry : timerMap) {
        if (entry.instance == htim->Instance) {
            handler = entry.handler;
            break;
        }
    }
    if (handler) {
        (this->*handler)(); // Вызываем обработчик
    }
}
// Реализация обработчиков
void TimerManager::handleTIM4() {
	//HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);//Разрешаем прерывание EXTI15


	//HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);//Запрещаем прерывание EXTI15
}
void TimerManager::handleTIM9() {
//    num = 1 - num;
//    TIM7->CR1 = TIM7->CR1 + recount[num]; // Пуск/стоп TIM7
//    if (num) {
//        GPIOB->BSRR |= GPIO_PIN_15 << 16U; // Спикер выкл
//    }
//    numSound++;
//    if (numSound == 8) {
//        HAL_TIM_Base_Stop_IT(&htim7);
//        HAL_TIM_Base_Stop_IT(&htim9);
//        GPIOB->BSRR |= GPIO_PIN_15 << 16U; // Спикер выкл
//        numSound = 0;
//    }
}
//-------------------------Параметры звука переключения режимов и кнопки
//void TimerManager::handleTIM7() {
//    GPIOB->BSRR = ((GPIOB->ODR & GPIO_PIN_15) << 16U) | (~GPIOB->ODR & GPIO_PIN_15);
//}

void TimerManager::handleTIM12() {
//    num = 1 - num;
//    TIM7->CR1 = TIM7->CR1 + recount[num]; // Пуск/стоп TIM7
//    //(num) && (GPIOB->BSRR |= GPIO_PIN_15 << 16U); // Спикер выкл
//    if (num != 0) {
//        GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U; // Спикер выкл
//    }
//    numSound = numSound + 1;
////    (numSound == 2) && (HAL_TIM_Base_Stop_IT(&htim7),
////                                          HAL_TIM_Base_Stop_IT(&htim12),
////                                          GPIOB->BSRR |= GPIO_PIN_15 << 16U, // Спикер выкл
////                                          numSound = 0);
//    if (numSound == 2) {
//        HAL_TIM_Base_Stop_IT(&htim7);
//        HAL_TIM_Base_Stop_IT(&htim12);
//        GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16U; // Спикер выкл
//        numSound = 0;
//    }
}
//-----------------Время-------------------------------------------------
//void TimerManager::handleTIM10() {
//    SetTimer::totalTime += 1; // Общее время нагрева
//    SetTimer::secondCounter += 1;
//    (SetTimer::secondCounter == 60) &&
//    		( SetTimer::minCounterInc += 1,
//    		 SetTimer::minCounterDec -= 1,
//			 SetTimer::secondCounter = 0);
//}

void TimerManager::handleTIM10() {
    // Используем явное присваивание для исключения предупреждений volatile
    SetTimer::totalTime = SetTimer::totalTime + 1; // Общее время нагрева
    SetTimer::secondCounter = SetTimer::secondCounter + 1;

    // Классическое условие вместо логического &&
    if (SetTimer::secondCounter == 60) {
        SetTimer::minCounterInc = SetTimer::minCounterInc + 1;
        SetTimer::minCounterDec = SetTimer::minCounterDec - 1;
        SetTimer::secondCounter = 0;
    }
}

void TimerManager::handleTIM6() {
	(Control::ovenTemper >= Heat::tempMax) && (Button::regim1Button(), 0);
}
void TimerManager::handleTIM5() {
	Protection::TIM5_Handler();
}
//void TimerManager::handleTIM11() {
//	MelodyPlayer::handleTimerInterrupt();
//}
//---------------------EXTIManager-----------------------------
void EXTIManager::handleEXTIInterrupt(uint16_t GPIO_Pin) {
	(GPIO_Pin == GPIO_PIN_2 && (handleGPIO2(), true)) ||
	(GPIO_Pin == GPIO_PIN_14 && (handleGPIO14(), true))||
	(GPIO_Pin == GPIO_PIN_15 && (handleGPIO15(), true));
}
void EXTIManager::handleGPIO2() {
	//__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
    HAL_NVIC_DisableIRQ(EXTI2_IRQn);
    Button::isEncDone(true);
}
void EXTIManager::handleGPIO14() {
	Heat::soundPre = Control::readAdc(3) &&
	                    (Fram::framRD0byte() - Control::ovenTemper < Heat::HysteresisTemp()) &&
	                    (Fram::framRD0byte() - Control::ovenTemper > 0);
}
void EXTIManager::handleGPIO15() {
	Protection::EXTI_Handler();
}


//---------------------Колбеки-----------------------------
ADCManager adcManager;
TimerManager timerManager;
EXTIManager extiManager;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    adcManager.handleADCConversionComplete(hadc);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    timerManager.handleTimerInterrupt(htim);
}

void HAL_GPIO_EXTI_Callback(vu16 GPIO_Pin) {
    extiManager.handleEXTIInterrupt(GPIO_Pin);
}
