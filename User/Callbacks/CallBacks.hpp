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
        void handleTIM2();
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
class EXTIManager {
public:
    // Точка входа из HAL_GPIO_EXTI_Callback
    void handleEXTIInterrupt(uint16_t GPIO_Pin);

    // Основной монитор (вызывать в while(1))
    // Возвращает 0 - ОК, 11-13 - Обрыв, 21-23 - Залипание
    uint8_t checkHeaters();

private:
    // Метки времени импульсов (обновляются в прерываниях)
    // 0: Right, 1: Down, 2: Grl
    volatile uint32_t lastPulse[3] = {0, 0, 0};

    // Таймеры для накопления 5-секундной задержки
    uint32_t stuckTimer[3] = {0, 0, 0};
    uint32_t openTimer[3]  = {0, 0, 0};

    // Внутренние методы обработки конкретных пинов
    void handleRight();
    void handleDown();
    void handleGrl();

    // Вспомогательный метод для получения состояния реле
    bool getRelayState(uint8_t index);
};
#endif /* CALLBACKS_HPP_ */
