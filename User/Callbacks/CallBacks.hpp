/*
 * CallBacks.hpp
 *
 *  Created on: Mar 31, 2025
 *      Author: erlbriton
 */

#ifndef CALLBACKS_HPP_
#define CALLBACKS_HPP_

#include "stm32f4xx_hal.h"
#include "FryMode.hpp"
#include "Protection.hpp"
#include <array>

class ADCManager {// Класс для обработки ADC
public:
    void handleADCConversionComplete(ADC_HandleTypeDef* hadc);
};
class TimerManager {// Класс для обработки таймеров
public:
    void handleTimerInterrupt(TIM_HandleTypeDef* htim);
private:
    void handleTIM4();
    void handleTIM9();
    void handleTIM7();
    void handleTIM12();
    void handleTIM10();
    void handleTIM6();
    void handleTIM5();
    //void handleTIM11();

    using TimerHandler = void (TimerManager::*)(); // Указатель на метод класса

    struct TimerMap {// Массив структур, которые связывают таймеры и их обработчики
        TIM_TypeDef* instance;
        TimerHandler handler;
    };

    static const TimerMap timerMap[ ];

    bool num = 0;
    vu8 numSound = 0;
    int recount[2] = {1, -1};
    //uint8_t ovenTemper = 0;
};
class EXTIManager {// Класс для обработки EXTI
public:
    void handleEXTIInterrupt(vu16 GPIO_Pin);
private:
    void handleGPIO2();
    void handleGPIO14();
    void handleGPIO15();
};
#endif /* CALLBACKS_HPP_ */
