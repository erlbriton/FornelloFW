/*
 * Button.cpp
 *
 *  Created on: Jan 22, 2024
 *      Author: erlbriton
 */
#include "Button.hpp"
#include "Heat.hpp"
#include "FryMode.hpp"
#include <array>
#include <cstdio>
#include "SetTimer.hpp"
#include "flashdata.hpp"
#include "Protection.hpp"
//--------------------------------------------Сканируем нажатие-------------------------------------
uint8_t Button::scanButton() {
	vu8 modeCookAveADC = Fram::elementFram(1);
	    bool isCookingMode = modeCookAveADC != 0;
	    bool isButtonPressed = gettingButton == 1 && flagButton == 0;
	    bool isButtonReleased = gettingButton == 0 && flagButton == 1;
	    buttonRegim += isCookingMode && isButtonPressed;
	    buttonRegim = (buttonRegim > 2) ? 2 : buttonRegim;
	    flagButton = (flagButton || isButtonPressed) && !isButtonReleased;
	    return buttonRegim;
}
//-----------------------------------------1-й режим-------------------------------------------------
void Button::buttonRegimOne() {
	if(flagOneButton == false){
	vu8 settedMode = Fram::elementFram(1);
	buf_485[12] = 0;//Останавливаем точки в часах
	//if (flagSoundButton[0] == 0) {
	    Heat::spOn();           // Подаем звук нажатой кнопки
	    flagSoundButton[0] = 1; // Устанавливаем флаг, что звук уже прозвучал
	    flagSoundButton[1] = 0; // Сбрасываем остальные флаги
	    flagSoundButton[2] = 0;
	//}
//-------------------------------------------------------------------------------------------------------------------------------------------------
bool isSettedMode = (settedMode == set || settedMode == light);//Если режим  set или light то isSettedMode == true
isSettedMode && (buttonRegim = 2);//Если режим "light" или  set - то buttonRegim  сразу = 2(время выставлять не нужно)
	zeroing(); //Обнуляем все необходимые флаги
	GPIOA->BSRR |= GPIO_PIN_12;//Включаем свет
	pass3Button = false; //Обнуляем флаг прохода режима 3 кнопки(для режима "PRE")
	HAL_TIM_Base_Stop_IT(&htim4);
	flagOneButton = true;
	Fram::elementFram(5, 0);//Снимаем флаг рабочего режима
	if(!Protection::protection_is_active){
	    Protection::Start();//Запускаем проверку тока в тенах
	}
}
	if (Protection::GetState() == SIGNAL_PRESENT) {//Реле залипло!
		GPIOC->BSRR = GPIO_PIN_13;//Включить реле выключения автомата
		HAL_Delay(2000);
		GPIOC->BSRR = GPIO_PIN_13<< 16U;//Выключить реле выключения автомата
		GPIOB->BSRR = GPIO_PIN_9 << 16U;
		GPIOA->BSRR = GPIO_PIN_12;//Свет
	    }
}
//-----------------------Второй режим кнопки----------------------------
void Button::buttonRegimTwo() {//Проверка выключенного режима Pre
	vu8 settedMode = Fram::elementFram(1);
	    if (settedMode == pre) {
	        buttonRegim = 2;// Если режим "pre", сразу переходим ко второму состоянию кнопки
	    } else {
	        executeMainRegimLogic();// В противном случае выполняем основную логику режима
	    }
}
bool Button::executeMainRegimLogic() {//Метод второго и следующих проходов
    if (firstCall) TIM2->CNT = 0;// Первый проход
    bool isFlagSoundButton = (flagSoundButton[1] != true);//isFlagSoundButton противоположно flagSoundButton[1]
    isFlagSoundButton && (Heat::spOn(), flagSoundButton[1] = 1, flagSoundButton[2] = 0);
   // !flagSoundButton[1] && (Heat::spOn(), flagSoundButton[1] = 1, flagSoundButton[2] = 0);
    pass3Button = false;
    encCount(); // Устанавливаем время (или не устанавливаем)
    timerCntEncoder = TIM2->CNT;
    buf_485[18] = 1;//Рамка вокруг часов при установки времени приготовления
    //HAL_UART_Transmit_IT(&huart3, buf_485, 20); // Передаем на дисплей
    SetTimer::setTime();
    firstCall = false;
    dirTime = (timerCntEncoder != 0);//Задаем направление счета времени
    return dirTime;
}
//--------------3-й режим---------------------------------------------------------
void Button::buttonRegimThree() {
    Fram::elementFram(0, cntEncoder);
    TIM2->CNT = cntEncoder; // Пишем в счетчик энкодера и во FRAM
    Fram::fram_wr_massive(); // то, что было запомнено при установке температуры
    if (!flagSoundButton[2]) {
        Heat::spOn();           // Подаем звук
        flagSoundButton[2] = 1; // Устанавливаем флаг поданного сигнала при переходе на 3-й режим кнопки
        flagSoundButton[0] = 0; // Сбрасываем флаг поданного сигнала 1-го режима кнопки
    }
    if (!pass3Button) {//Блок инициализации при первом проходе
        vu8 settedMode = Fram::elementFram(1);// Читаем режим из памяти
        HAL_TIM_Base_Start_IT(&htim4);
        uint8_t tempValue;
        if (settedMode != dry) {//Определяем начальную температуру
            tempValue = FryModeLambda::firstTemp;
        } else {
            tempValue = FryModeLambda::firstTempDry;
        }
        Fram::elementFram(0, tempValue);
        pass3Button = true;//Помечаем, что первый проход выполнен
    }
    Fram::elementFram(5, 1); // Включен режим регулирования
    buf_485[12] = 1; // Включаем точки в часах
    HAL_Delay(50);
    Fram::framRD0byte();
    flagOneButton = false;//Включаем режим 1-го прохода 1-го режима кнопки
  if (Fram::elementFram(1) != 3 && Fram::elementFram(1) != 10) {//Если режим не set  и light
      Heat::ajustHeat595(Fram::elementFram(1));//запускаем нагрев в выбранном режиме
      buf_485[18] = 0;
      Protection::Stop();//Выключаем проверку тока в тенах
      GPIOB->BSRR = GPIO_PIN_9;
      Protection::protection_is_active = false;
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------
bool Button::regim1Button(){
	setButtonRegim(0);
	return true;
}
const std::array<std::function<void(Button*)>, 3> Button::arrButtonRegim = {
    [ ](Button* btn) { btn->buttonRegimOne(); },
    [ ](Button* btn) { btn->buttonRegimTwo(); },
    [ ](Button* btn) { btn->buttonRegimThree(); }
};
//-------------------------------------Выбор режима кнопки-------------------------------------------
bool Button::executeButtonRegim(vu8 index)  {
    if (index >= 0 && index < arrButtonRegim.size()) {
        arrButtonRegim[index](this); //Передаём `this` в лямбду
    }
    return true;
}
//--------------------------------------Zeroing------------------------------------------------------
void Button::zeroing() {
	SetTimer::number_Iterator_Increment = false;
	SetTimer::number_Iterator_Decrement = false;
	firstCall = true;
	Heat::soundPre = false;
	cntEncoder = 0;
	SetTimer::hoursSet = 0;
	SetTimer::minutesSet = 0;
	SetTimer::memTime = 0;
	Heat::all_off(); //Выключение всего
	//SetTimer::number_Iterator_Increment = false;
	Button::dirTime = false;
	SetTimer::secondCounter = 0;
	SetTimer::minCounterInc = 0;
	SetTimer::minCounterDec = 0;
	SetTimer::totalTime = 0;//Общее время нагрева
	//soundPre = false; //Сбрасываем флаг поданного звука  ДОДЕЛАТЬ
	//vu8 isSettedMode = Fram::elementFram(1);
	//--------------------Выводим в дисплей-------------------------------------------
//#################################Для калибровки###################################################
//	buf_485[1] = 0; ////Единицы минут
//	buf_485[2] = 0; //Десятки минут
//	buf_485[3] = 0; //Единицы часов
//	buf_485[4] = 0; //Десятки часов
//###########################Конец  для калибровки###################################################
	buf_485[12] = 0;
	buf_485[12] = 0; //Останавливаем точки в часах
	buf_485[13] = 0; //Стираем огонь
	buf_485[14] = 0; //Стираем огонь
	buf_485[15] = 0; //Стираем огонь
}
//-------------------------------------------Задаем температуру-----------------
vu8 Button::encCount() {
    vu8 settedMode = Fram::elementFram(1); // Читаем установленный режим из Fram
    vu8 byte0Fram = 0;
    // Главное условие: работаем, если режим не равен 1
    if (buttonRegim != 1) {
        cntEncoder = TIM2->CNT; // Считываем текущее значение счетчика таймера
        // Проверяем, изменились ли данные и был ли поворот (флаг encDone)
        if (encMemory != cntEncoder && encDone == true) {
            encMemory = cntEncoder;        // Запоминаем новое значение
            Fram::elementFram(0, cntEncoder); // Сохраняем во внутренний массив
            Fram::fram_rd_massive();       // Читаем массив обратно
            // Если мы не в режиме настройки "Set", обновляем данные для дисплея
            if (settedMode != set) {
                byte0Fram = Fram::framRD0byte();
                buf_485[0] = 151;
                buf_485[19] = 151;
                // Разложение числа на разряды для индикации
                buf_485[8] = byte0Fram % 10;          // Единицы
                buf_485[9] = (byte0Fram / 10) % 10;   // Десятки
                buf_485[10] = byte0Fram / 100;        // Сотни
            }
            encDone = false;//Сбрасываем флаг прокрутки только после обработки изменения
        }
    }
    return cntEncoder;
}
//------------------------------------Геттеры и сеттеры----------------------------------------------
void Button::isEncDone(bool newEncDone){//Сеттер EncDone
	encDone = newEncDone;
 }
bool Button::setButtonRegim(vu8 newButtonRegim){//Сеттер buttonRegim
	buttonRegim = newButtonRegim;
	return true;
}


