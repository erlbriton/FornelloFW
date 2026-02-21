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
	inline static vu32 totalTime = 0;//Общее время. Для контроля максимального времени нагрева
	inline static vu32 secondCounter = 0;//Секундный счетчик
	inline static vu32 minCounterInc = 0;//Инкрементный счетчик минут
	inline static vu32 minCounterDec = 0;//Декрементный счетчик минут
	inline static vu32 hours = 0;
	inline static vu32 minutes = 0;
	static inline vu32 hoursSet = 0; //Установленные часы
	static inline vu32 minutesSet = 0; //Установленные минуты
	static inline vu32 memTime = 0; //Запоминаем время, выставленное энкодером

	static vu16 setTime() {//Задаем время приготовления
		hoursSet = TIM2->CNT / 60;     //Часы
		minutesSet = TIM2->CNT % 60;     //минуты
		buf_485[0] = 151;
		buf_485[1] = minutesSet % 10;
		buf_485[2] = minutesSet  / 10;
		buf_485[3] = hoursSet % 10;
		buf_485[4] = hoursSet / 10;
		buf_485[19] = 151;
		HAL_UART_Transmit_IT(&huart3, buf_485, 20);//Передаем на дисплей
		return memTime = TIM2->CNT;
	}
static void TimeCook(bool dirTime){
	dirTime ? DecrementCookTime() : IncrementCookTime();//Выбираем направление счета
}
static void  IncrementCookTime();
static void DecrementCookTime();

};
#endif /* SETTIME_SETTIMER_HPP_ */
