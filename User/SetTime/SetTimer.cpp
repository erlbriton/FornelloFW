/*
 * SetTimer.cpp
 *
 *  Created on: Apr 7, 2025
 *      Author: erlbriton
 */

#include <SetTimer.hpp>
#include "Button.hpp"
//----------------------------------------------------------Increment времени----------------------------------
void SetTimer::IncrementCookTime(){
	Fram fram;
	Button button;
	if(!number_Iterator_Increment){
	    minCounterInc = 0;
	    number_Iterator_Increment = true;
	}
	hours = minCounterInc / 60;   //Часы
	minutes = minCounterInc % 60;   //минуты
	fram.elementFram(fram.CURRENT_MINUTS, minutes); //Записываем текущее время - минуты,
	fram.elementFram(fram.CURRENT_HOURS, hours);   //...часы...
	//Fram::fram_rd_massive();
	fram.elementFram(fram.H_BYTE_MINUTS_COUNTER, minCounterInc >> 8);   // Старший байт
	fram.elementFram(fram.L_BYTE_MINUTS_COUNTER, (minCounterInc & 0xFF));   //Младший байт
	fram.elementFram(fram.DIRECTION_TIME, button.dirTime); //Направление счета времени
	fram.fram_wr_massive(); //
	//**************************  Выводим в USART  ********************************************
	//#################################Для калибровки датчика температуры убираем##########################
	buf_485[1] = minutes % 10;   //Единицы минут
	buf_485[2] = minutes / 10;   //Десятки минут
	buf_485[3] = hours % 10;  //Единицы часов
	buf_485[4] = hours / 10;  //Десятки часов
	//#################################Для калибровки###################################################
}
//--------------------------------------------------------Decrement времени---------------------------------------------
void SetTimer::DecrementCookTime() {
	// Первый вход — устанавливаем флаг первого прохода и начальное значение времени
	Fram fram;
	Button button;
	if (!number_Iterator_Decrement) {
	    minCounterDec = Button::timerCntEncoder;
	    number_Iterator_Decrement = true;
	}
	bool hasTime = minCounterDec > 0;	//Если время еще не вышло считаем дальше
	if (hasTime) {
	    // 1. Расчет времени
	    hours = minCounterDec / 60;
	    minutes = minCounterDec % 60;

	    // 2. Сохранение во FRAM
	    fram.elementFram(fram.CURRENT_MINUTS, minutes);          // Записываем текущее время - минуты
	    fram.elementFram(fram.CURRENT_HOURS, hours);            // ...часы...
	    //Fram::fram_rd_massive();

	    // Запись общего счетчика (два байта)
	    fram.elementFram(fram.H_BYTE_MINUTS_COUNTER, minCounterDec >> 8);    // Старший байт
	    fram.elementFram(fram.L_BYTE_MINUTS_COUNTER, (minCounterDec & 0xFF)); // Младший байт

	    fram.elementFram(fram.DIRECTION_TIME, button.dirTime);  // Направление счета времени

	    // 3. Вывод в буфер USART (разложение на разряды)
	    buf_485[1] = minutes % 10;   // Единицы минут
	    buf_485[2] = minutes / 10;   // Десятки минут
	    buf_485[3] = hours % 10;     // Единицы часов
	    buf_485[4] = hours / 10;     // Десятки часов
	} else {
	    // Если время закончилось (!hasTime)
	    Button::regim1Button();      // Выключаем нагрев
	    buf_485[1] = 0;              // Обнуляем индикацию
	}
}
