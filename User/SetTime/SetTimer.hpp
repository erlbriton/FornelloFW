/*
 * SetTimer.hpp
 *
 *  Created on: Apr 7, 2025
 *      Author: erlbriton
 */

#ifndef SETTIME_SETTIMER_HPP_
#define SETTIME_SETTIMER_HPP_
#include "flashdata.hpp"
#include "Fram.hpp"
#include "FryMode.hpp"

class SetTimer {
private:
	static void first_Iterator_Increment();
	static void second_Iterator_Increment();
	static void first_Iterator_Decrement();
	static void second_Iterator_Decrement();
public:
	inline static bool number_Iterator_Increment = false;
	inline static bool number_Iterator_Decrement = false;
	inline static uint32_t totalTime = 0;//Общее время. Для контроля максимального времени нагрева
	inline static uint32_t secondCounter = 0;//Секундный счетчик
	inline static uint32_t minCounterInc = 0;//Инкрементный счетчик минут
	inline static uint32_t minCounterDec = 0;//Декрементный счетчик минут
	inline static uint32_t hours = 0;
	inline static uint32_t minutes = 0;
	static inline uint32_t hoursSet = 0; //Установленные часы
	static inline uint32_t minutesSet = 0; //Установленные минуты
	static inline uint32_t memTime = 0; //Запоминаем время, выставленное энкодером

	static uint16_t setTime() {//Задаем время приготовления
		hoursSet = TIM2->CNT / 60;     //Часы
		minutesSet = TIM2->CNT % 60;     //минуты
		buf_485[0] = 151;
		buf_485[1] = minutesSet % 10;
		buf_485[2] = minutesSet  / 10;
		buf_485[3] = hoursSet % 10;
		buf_485[4] = hoursSet / 10;
		buf_485[20] = 151;
		return memTime = TIM2->CNT;
	}
static void TimeCook(bool dirTime){
	dirTime ? DecrementCookTime() : IncrementCookTime();//Выбираем направление счета
}
static void  IncrementCookTime();
static void DecrementCookTime();

};
#endif /* SETTIME_SETTIMER_HPP_ */
