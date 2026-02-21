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
	if(!number_Iterator_Increment){
	    minCounterInc = 0;
	    number_Iterator_Increment = true;
	}
	hours = minCounterInc / 60;   //Часы
	minutes = minCounterInc % 60;   //минуты
	Fram::elementFram(2, minutes); //Записываем текущее время - минуты,
	Fram::elementFram(3, hours);   //...часы...
	Fram::fram_rd_massive();
	Fram::elementFram(6, minCounterInc >> 8);   // Старший байт
	Fram::elementFram(7, (minCounterInc & 0xFF));   //Младший байт
	Fram::elementFram(4, Button::dirTime); //Направление счета времени
	Fram::fram_wr_massive(); //
	Fram::fram_rd_massive();
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
	if (!number_Iterator_Decrement) {
	    minCounterDec = Button::timerCntEncoder;
	    number_Iterator_Decrement = true;
	}
	bool hasTime = minCounterDec > 0;	//Если время еще не вышло считаем дальше
	hasTime && ( // Используем логическое умножение для условного выполнения блока
			(hours = minCounterDec / 60),   //Часы
			(minutes = minCounterDec % 60),   //минуты
			Fram::elementFram(2, minutes), //Записываем текущее время - минуты,
			Fram::elementFram(3, hours),   //...часы...
			Fram::fram_rd_massive(), Fram::elementFram(6, minCounterDec >> 8), // Старший байт
			Fram::elementFram(7, (minCounterDec & 0xFF)),   //Младший байт
			Fram::elementFram(4, Button::dirTime), //Направление счета времени
			//**************************  Выводим в USART  ********************************************
			(buf_485[1] = minutes % 10),   //Единицы минут
			(buf_485[2] = minutes / 10),   //Десятки минут
			(buf_485[3] = hours % 10),  //Единицы часов
			(buf_485[4] = hours / 10)   //Десятки часов
			);
	!hasTime && (Button::regim1Button(), // Если времени закончилось — выключаем нагрев
	buf_485[1] = 0, HAL_UART_Transmit_IT(&huart3, buf_485, 20));
}
