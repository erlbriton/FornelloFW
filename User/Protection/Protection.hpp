/*
 * Protection.hpp
 *
 *  Created on: Dec 7, 2025
 *      Author: erlbriton
 */

#ifndef PROTECTION_PROTECTION_HPP_
#define PROTECTION_PROTECTION_HPP_

#include "stm32f4xx.h"

// --- Константы для Инерционной Логики ---

// 1. Количество полупериодов (фронтов), необходимых для подтверждения наличия тока (минимум 200 мс устойчивого сигнала).
#define REQUIRED_PULSE_COUNT 1500

// 2. Количество тиков TIM5, необходимых для подтверждения отсутствия тока (200 тиков * 10 мс/тик = 2000 мс / 2 секунды).
// Предполагается, что TIM5 настроен на 10 мс в файле TIM.c.
#define TIMEOUT_TICKS 200

// --- Состояние Сигнала (Тока) ---
typedef enum {
    SIGNAL_ABSENT = 0, //Ток отсутствует
    SIGNAL_PENDING = 2,//Идет подтверждение наличия тока
    SIGNAL_PRESENT = 1 //Ток присутствует
} SignalState_t;

/**
 * @brief Класс для инерционного мониторинга наличия/отсутствия 50 Гц тока на PC15.
 */
class Protection {
public:
	Protection();
	// Методы управления
	    static void Init();
	    static void Start();
	    static void Stop();
	    static SignalState_t GetState();

	    // Обработчики прерываний (должны вызываться из stm32f4xx_it.c)
	    static void EXTI_Handler();
	    static void TIM5_Handler();

	private:
	    // --- Переменные состояния ---
	    static volatile SignalState_t current_state;
	    static volatile uint16_t Pulse_Confirm_Counter;
	    static volatile uint16_t timeout_tick_counter;

	    // --- Константа EXTI ---
	   inline static const uint32_t SIGNAL_EXTI_LINE = EXTI_LINE_15;

	public:
	   volatile inline static  bool protection_is_active;//Флаг вкл-выкл защиты

	virtual ~Protection();
};

#endif /* PROTECTION_PROTECTION_HPP_ */
