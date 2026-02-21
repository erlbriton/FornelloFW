/*
 * FryMode.hpp
 *
 *  Created on: Jan 8, 2024
 *      Author: erlbriton
 */

#ifndef MODE_FRYMODE_HPP_
#define MODE_FRYMODE_HPP_
#include "main.h"
#include "flashdata.hpp"
#include "tim.h"
#include "DS18B20.h"
#include "Control.hpp"
#include "Heat.hpp"
#include "usart.h"
#include <functional>
#include <array>
#include <cstdint>

//extern const std::array<uint8_t, 4096> rawADC;

//class Heat;
//class FryModePoly;

class FryModeLambda {
public:
  inline static uint8_t modeCookOld = 0; //Для Запоминания номера режима, чтобы не повторятся каждую итерацию, кроме set,
  inline static vu8 settedMode = 0;
  	//иначе температура платы будет измеряться только один раз при включении режима.
  void modeCom(uint16_t, uint8_t);
  	void firstRegim(vu8);
  	void modeWrite(vu8);//Записываем номер включенного режима
  	static const uint16_t firstTemp = 180;	//начальная температура кроме сушки
  	static const uint16_t firstTempDry = 70;//начальная температура для сушки(dry)
  //-----------------------------------------Новый метод-----------------------------------------------//
	vu8 ModeSetLambda(uint8_t index); // Метод для вызова лямбда-функции по индексу
	void ModeSetLambda(std::function<void(FryModeLambda*)> lambda, vu8);// Метод для вызова лямбда-функции, переданной как параметр

private:
	static const std::array<std::function<void(FryModeLambda*)>, 6> modeCookLambdaArray;// Массив лямбда-функций
};
#endif /* MODE_FRYMODE_HPP_ */
