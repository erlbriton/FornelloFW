/*
 * Button.hpp
 *
 *  Created on: Jan 22, 2024
 *      Author: erlbriton
 */

#ifndef BUTTON_BUTTON_HPP_
#define BUTTON_BUTTON_HPP_

#include "main.h"
#include "flashdata.hpp"
#include "functional"
#include "Protection.hpp"

class Button {
private:
	static inline uint8_t encMemory;
	static inline volatile bool encDone = false;//Флаг прокрутки энкодера
	static const std::array<std::function<void(Button*)>, 3> arrButtonRegim;//Массив элементов типа "std::function<void()>" в контейнере "std::array<>"
	static inline bool flagButton = false;
	static inline bool pass1Button = false;
	static inline bool pass3Button = false;
	static inline bool flagSoundButton[3] = { false, };
	static inline bool firstCall = true;

public:
	static inline  volatile uint8_t cntEncoder;
	static inline  uint8_t timerCntEncoder;
	inline static bool dirTime = false;
	inline static bool flagOneButton = false;
	inline static uint8_t buttonRegim = 0;
	bool executeButtonRegim(uint8_t index);
	static bool executeMainRegimLogic();//Метод второго и следующих проходов
	//--------------------------------Геттеры и сеттеры---------------------------------------------
	static void isEncDone(bool newEncDone);

	bool getEncDone() const;//Читаем флаг прокрутки энкодера
	void setEncDone(bool);//Устанавливаем флаг прокрутки энкодера
	static bool setButtonRegim(uint8_t newButtonRegim);
	static bool setflagOneButton(bool state);//Установка флага flagOneButton
	//---------------------------------------------------------------------------------------------------
	static uint8_t encCount();
	static uint8_t scanButton();
	void buttonRegimOne();//Первый режим кнопки
	void buttonRegimTwo();//Втрой режим кнопки
	void buttonRegimThree();//Третий режим кнопки
	static void zeroing();
	static bool regim1Button();
};

#endif /* BUTTON_BUTTON_HPP_ */
