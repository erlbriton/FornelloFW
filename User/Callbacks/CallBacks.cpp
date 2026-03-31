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
//        buf_485[2] = adcTemp[0] / 10 % 10;
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
	{ TIM2,  &TimerManager::handleTIM2  },
	{ TIM4,  &TimerManager::handleTIM4  },
    { TIM5,  &TimerManager::handleTIM5  },
    { TIM6,  &TimerManager::handleTIM6  },
    { TIM9,  &TimerManager::handleTIM9  },
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
//---------------------------------------------------------------------
void TimerManager::handleTIM2() {
	Button::isEncDone(true);
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

void TimerManager::handleTIM4() {
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
//-------------------------Проверка ТЭНов на обрыв и залипание контактов реле или пробой ключа-----------------
void EXTIManager::handleEXTIInterrupt(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
        case EXTI_Right_Pin:
            handleRight();
            break;
        case EXTI_Down_Pin:
            handleDown();
            break;
        case EXTI_Grl_Pin:
            handleGrl();
            break;
        default:
            break;
    }
}
void EXTIManager::handleRight() { lastPulse[0] = HAL_GetTick(); }
void EXTIManager::handleDown()  { lastPulse[1] = HAL_GetTick(); }
void EXTIManager::handleGrl()   { lastPulse[2] = HAL_GetTick(); }

uint8_t EXTIManager::checkHeaters() {
    uint32_t now = HAL_GetTick();
    const uint32_t timeout = 5000;      // 5 секунд
    const uint32_t pulseValid = 100;    // 100 мс

    for (uint8_t i = 0; i < 3; i++) {
        bool hasCurrent = (now - lastPulse[i]) < pulseValid;
        bool cmdOn = getRelayState(i);

        // ПРОВЕРКА ЗАЛИПАНИЯ (Команды нет, ток есть)
        if (!cmdOn && hasCurrent) {
            if (stuckTimer[i] == 0) stuckTimer[i] = now;
            if ((now - stuckTimer[i]) > timeout) return (21 + i);
        } else {
            stuckTimer[i] = 0;
        }

        // ПРОВЕРКА ОБРЫВА (Команда есть, тока нет)
        if (cmdOn && !hasCurrent) {
            if (openTimer[i] == 0) openTimer[i] = now;
            if ((now - openTimer[i]) > timeout) return (11 + i);
        } else {
            openTimer[i] = 0;
        }
    }
    return 0; // Ошибок нет
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
//-----------------------------------------------------------------
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        // Вызываем ваш метод установки флага
        Button::isEncDone(true);
    }
}

void HAL_GPIO_EXTI_Callback(vu16 GPIO_Pin) {
    extiManager.handleEXTIInterrupt(GPIO_Pin);
}
//-------------------------------Конец проверки ТЭНов------------------------------------
