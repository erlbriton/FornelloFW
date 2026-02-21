/*
 * Heat.hpp
 *
 *  Created on: Jan 25, 2024
 *      Author: erlbriton
 */

#ifndef HEAT_HEAT_HPP_
#define HEAT_HEAT_HPP_

#include "Fram.hpp"
#include "tim.h"
#include "adc.h"
#include "Button.hpp"
#include <array>
#include <cstdint>
#include <unordered_map>

#define Hi false//Начало нагрева
#define Low true//Конец нагрева

class Heat {
private:
	static bool checkProtectionTriggers(vu8);
	static void checkAndPlaySound();//Проигрывание мелодии
	//Массив кодов, подающихся непосредственно на тэны и внутренний кулер
	static constexpr vu8 modeTable[11 ][2] = {
			{0,     0},//Off
			{1,     3},//Pre
			{9,     9},//Dry
			{0,     0},//Set
			{2,     2},//Grill
			{4,     5},//Brow
		    {9,    11},//Fan
	        {10,   11},//Ff
		    {1,     3},//Ord
		    {10,   10},//Cook
		    {0,     0}//Light
};
	inline static vu8 dataTransmit;
public:
	static inline bool soundPre;//Флаг подачи звука предварительного нагрева
	static inline vu8 dataMode;	//Ключ для массива пар, который определяет данные для передачи на 595(работае также просто как локальная переменная)
	static inline uint8_t dataModeOld = 0;//Режим(см. выше) прошлой итерации. Нужен для сравнения с текущим режимом и принятия
	static const uint32_t maxTotalTime = 43200;	//Общее время работы до 120 градусов
	static const uint16_t minTotalTime = 10800;	//Общее время работы более  120 градусов
	static  void ajustHeat595(vu8 numberRegimCook);
	static void TransmitToTENs(vu8);
	static vu8 HysteresisTemp(); //Гистерезис температур в зависимости от режима
	inline static void all_off(){ //Просто делаем сброс HC595
		GPIOA->BSRR |= GPIO_PIN_8 << 16U;   //MR Off
		GPIOC->BSRR |= GPIO_PIN_6; //Latch on
		GPIOC->BSRR |= GPIO_PIN_6 << 16U; //Latch off
		GPIOA->BSRR |= GPIO_PIN_8; //MR On
	}
	static void setOutCooler(); //Управление внешним кулером---------------------
	static void soundMatch(); //Подача звука при предварительном нагреве
//---------------------Делаем inline метод--------------------------------------
	static inline bool spOn() { //Подача звука при переключении режима и нажатии кнопки
//		TIM7->ARR = 9; // Немного изменяю на ходу
//		TIM7->PSC = 1680; //частоту звука
//		TIM7->EGR = TIM_EGR_UG; //просто для тренировки
//		HAL_TIM_Base_Start_IT(&htim7);
//		HAL_TIM_Base_Start_IT(&htim12);
		return true;
	}
//-------------------------------------------------------------------------------
	static const uint16_t tempPreOver = 10;//Гистерезис температуры режима "Pre"
	static const uint16_t tempOtherOver = 3;//Гистерезис температуры остальных режимов
	static const uint16_t tempMax = 240;//максимальная температура
};

#endif /* HEAT_HEAT_HPP_ */
