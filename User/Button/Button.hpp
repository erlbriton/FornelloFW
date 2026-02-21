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
	static inline vu8 encMemory;
	static inline volatile bool encDone = false;//Флаг прокрутки энкодера
	static const std::array<std::function<void(Button*)>, 3> arrButtonRegim;//Массив элементов типа "std::function<void()>" в контейнере "std::array<>"
	static inline bool flagButton = false;
	static inline bool pass1Button = false;
	static inline bool pass3Button = false;
	static inline bool flagSoundButton[3] = { false, };
	static inline bool firstCall = true;

public:
	static inline  volatile vu8 cntEncoder;
	static inline  vu8 timerCntEncoder;
	inline static bool dirTime = false;
	inline static bool flagOneButton = false;
	inline static vu8 buttonRegim = 0;
	bool executeButtonRegim(vu8 index);
	static bool executeMainRegimLogic();//Метод второго и следующих проходов
	//--------------------------------Геттеры и сеттеры---------------------------------------------
	static void isEncDone(bool newEncDone);

	bool getEncDone() const;//Читаем флаг прокрутки энкодера
	void setEncDone(bool);//Устанавливаем флаг прокрутки энкодера
	static bool setButtonRegim(vu8 newButtonRegim);
	//---------------------------------------------------------------------------------------------------
	static vu8 encCount();
	static vu8 scanButton();
	void buttonRegimOne();//Первый режим кнопки
	void buttonRegimTwo();//Втрой режим кнопки
	void buttonRegimThree();//Третий режим кнопки
	static void zeroing();
	static bool regim1Button();
};

#endif /* BUTTON_BUTTON_HPP_ */
