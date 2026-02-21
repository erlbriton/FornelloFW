/*
 * Protection.cpp
 *
 *  Created on: Dec 7, 2025
 *      Author: erlbriton
 */

#include "Protection.hpp"
#include "tim.h"

Protection::Protection() {}

// --- Определение статических переменных класса Protection ---
volatile SignalState_t Protection::current_state = SIGNAL_ABSENT;
volatile uint16_t Protection::Pulse_Confirm_Counter = 0;
volatile uint16_t Protection::timeout_tick_counter = 0;
volatile bool protection_is_active = false;

// --- Константа для регистрового доступа ---
//const uint32_t Protection::SIGNAL_EXTI_LINE = EXTI_LINE_15;

/**
 * @brief Инициализация переменных и подготовка.
 */
void Protection::Init() {
    // Сброс всех переменных
    current_state = SIGNAL_ABSENT;
    Pulse_Confirm_Counter = 0;
    timeout_tick_counter = 0;
}
/**
 * @brief Активирует мониторинг. Вызывать, когда ТЭНы программно отключены.
 */
void Protection::Start() {
	EXTI->PR = EXTI_PR_PR15; // Сброс флага перед началом работы
    // 1. Сброс состояния и счетчиков
    current_state = SIGNAL_ABSENT;
    Pulse_Confirm_Counter = 0;
    timeout_tick_counter = 0;

    // 2. Разрешить прерывание EXTI15 (РАЗРЕШЕНИЕ МАСКИ)
    EXTI->IMR |= EXTI_IMR_IM15;
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    // 3. Запустить TIM
    //TIM5->CR1 |= TIM_CR1_CEN;
    HAL_TIM_Base_Start_IT(&htim5);
    Protection::protection_is_active = true;
}

/**
 * @brief Останавливает мониторинг. Вызывать, когда ТЭНы программно включены.
 */
void Protection::Stop() {
    // 1. Остановить TIM5
    //TIM5->CR1 &= ~TIM_CR1_CEN;
    HAL_TIM_Base_Stop_IT(&htim5);
    GPIOB->BSRR = GPIO_PIN_9;

    // 2. Запретить прерывание EXTI15 (ЗАПРЕТ МАСКИ)
    EXTI->IMR &= ~(1U << 15);
    //HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

    // 3. Сбросить состояние
    current_state = SIGNAL_ABSENT;
    Pulse_Confirm_Counter = 0;
    timeout_tick_counter = 0;
    Protection::protection_is_active = false;
}

/**
 * @brief Обработчик внешнего прерывания EXTI15_10_IRQn.
 * @note **Цикл Контроля Наличия Тока (Быстрая реакция)**
 */
void Protection::EXTI_Handler() {
    // Проверяем, что флаг EXTI15 установлен
    //if (EXTI->PR & SIGNAL_EXTI_LINE) {

        // --- Сброс Таймаута ---
        // Ток есть, сбрасываем счетчик тишины.
        timeout_tick_counter = 0;

        // --- Логика Подтверждения Наличия ---
        if (current_state != SIGNAL_PRESENT) {

            // 1. Считаем импульсы до REQUIRED_PULSE_COUNT (20)
            if (Pulse_Confirm_Counter < REQUIRED_PULSE_COUNT) {
            	//Pulse_Confirm_Counter++;
            	Pulse_Confirm_Counter = Pulse_Confirm_Counter + 1;
            }

            // 2. Если 20 импульсов получено, подтверждаем наличие тока.
            if (Pulse_Confirm_Counter >= REQUIRED_PULSE_COUNT) {
                current_state = SIGNAL_PRESENT;
            } else {
                current_state = SIGNAL_PENDING;
            }
        }

        // Очистка флага прерывания EXTI
        EXTI->PR = SIGNAL_EXTI_LINE;
        EXTI->PR = EXTI_PR_PR15;
}

/**
 * @brief Обработчик прерывания TIM5.
 * @note **Цикл Контроля Отсутствия Тока (Инерционный таймаут 2 секунды)**
 */
void Protection::TIM5_Handler() {
    // Проверка флага обновления TIM5
    if (TIM5->SR & TIM_SR_UIF) {

        // Очистка флага
        TIM5->SR &= ~TIM_SR_UIF;

        // --- Логика Таймаута ---

        // 1. Увеличение счетчика тишины (если он еще не достиг 200)
        if (timeout_tick_counter < TIMEOUT_TICKS) {
        	timeout_tick_counter = timeout_tick_counter +1;
        }

        // 2. Проверка Истечения Таймаута (200 тиков = 2 секунды)
        if (timeout_tick_counter >= TIMEOUT_TICKS) {

            // Установка состояния: Ток отсутствует
            if (current_state != SIGNAL_ABSENT) {
                current_state = SIGNAL_ABSENT;
            }

            // Сброс счетчика подтверждения и удержание счетчика таймаута на максимуме
            Pulse_Confirm_Counter = 0;
            timeout_tick_counter = TIMEOUT_TICKS;
        }
    }
}

/**
 * @brief Возвращает текущее состояние наличия сигнала 50 Гц.
 */
SignalState_t Protection::GetState() {
    return current_state;
}

Protection::~Protection() {}

