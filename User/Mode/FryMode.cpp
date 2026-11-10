/*
 * FryMode.cpp
 *
 *  Created on: Jan 8, 2024
 *      Author: erlbriton
 */

#include "FryMode.hpp"
#include "Heat.hpp"
#include "Fram.hpp"
#include "melody_gpio.hpp"
#include "PlateTemp.hpp"

extern PlateTemp plateTempSensor;

void FryModeLambda::modeCom(uint16_t averageAdc, uint8_t tempFirstMode) {//Метод, запускаемый при переключении режима
	Fram fram;
	firstRegim(tempFirstMode);//Устанавливаем первоначальный режим
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);//Запускаем энкодер в работу
	buf_485[MODE] =  fram.framRdOneAddress(Fram::TARGET_MODE);//Номер режима
}
//-----------------------------------Начальный режим---------------------------------------
void FryModeLambda::firstRegim(uint8_t tempFirst) {
	Fram fram;
	//Heat::spOn();//Писк
	Button::flagOneButton = false;//Флаг первого режима кнопки
	TIM2->CNT = 0;
	uint8_t indexesToClear[ ] = {1, 2, 3, 4, 12, 13, 14, 15, 18};//Стираем рамку вокруг часов, точки в часах, все пламя
	for (uint8_t i = 0; i < sizeof(indexesToClear)/sizeof(indexesToClear[0]); ++i) {
	    buf_485[indexesToClear[ i ] ] = 0;
	}
	buf_485[CHECK_FIRST] = buf_485[20] = 151; //Проверочный код посылки
	buf_485[TEMP_SET_1] = tempFirst % 100 % 10; //Единицы)
	buf_485[TEMP_SET_2] = tempFirst % 100 / 10; //Десятки
	buf_485[TEMP_SET_3] = tempFirst / 100;     //Сотни
	Button::regim1Button(); //Включаем 1-й режим button
	//----------Сбрасываем все выходы у HC595---------------------
	GPIOA->BSRR |= GPIO_PIN_8 << 16U;	//MR off
	GPIOC->BSRR |= GPIO_PIN_6; //Latch on
	GPIOC->BSRR |= GPIO_PIN_6 << 16U; //Latch off
	GPIOA->BSRR |= GPIO_PIN_8; //MR on
	//-----------------------------------------------------------
	fram.elementFram(fram.OPERATING_MODE, 0);//Сброса во время работы не было
	fram.elementFram(fram.TARGET_TEMP, tempFirst);//Записываем во фрам начальную температуру
	TIM2->CNT = tempFirst; //Записываем начальную температуру в энкодер
	Heat::soundPre = false;
}
//--------------------------/Записываем номер включенного режима---------------------------------------------
void FryModeLambda::modeWrite(uint8_t modeCookAveADC) {
	Fram fram;
	settedMode = fram.framRdOneAddress(Fram::TARGET_MODE);
	buf_485[CHECK_FIRST] = 151;
	buf_485[CHECK_LAST] = 151;
	buf_485[MODE] = settedMode;
}
//-----------------------------------------Методы режимов(массив лямбда функций)----------------------------------------------
const std::array<std::function<void(FryModeLambda*)>, 6> FryModeLambda::modeCookLambdaArray =
		//--------------------------------------------0  -  Off--------------------------------------------------------------------------
		{ [ ](FryModeLambda* fryModeLambda) {
			Heat heat;
			//Heat::spOn();//Звук
			heat.all_off(); // Выключаем все тены
			GPIOC->BSRR |= GPIO_PIN_8 << 16U; // Свет выкл
			buf_485[MODE] = settedMode;//Off
		},
//----------------------------------------------------1 - Pre----------------------------------------------------------------------------
		[ ](FryModeLambda* fryModeLambda) {
			fryModeLambda->firstRegim(firstTemp);
			HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);
			HAL_TIM_Base_Stop_IT(&htim4);
			buf_485[MODE] = settedMode; // Pre
		},
		//---------------------------------------------2 - Dry----------------------------------------------------------------------------------
		[ ](FryModeLambda* fryModeLambda) {
			fryModeLambda->modeCom(settedMode, fryModeLambda->firstTempDry);
			HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);
		},
		//----------------------------------------------3 - Set----------------------------------------------------------------------------------
		[ ](FryModeLambda* fryModeLambda) {//Set
			Heat heat;

			if(!sondFlagSetMode){
			MelodyPlayer::beep();
			sondFlagSetMode = true;
			}
			heat.all_off(); //Выключаем все
			//HAL_GPIO_WritePin(fanOut, GPIO_PIN_RESET);
			GPIOA->BSRR |= GPIO_PIN_11 << 16U;
			HAL_TIM_Encoder_Stop(&htim2, TIM_CHANNEL_1);
			HAL_TIM_Base_Stop_IT(&htim4);
			//plateTemp();
			plateTempSensor.plateTemp(); // Аналог plateTemp()
		},
		//-------------------------4 - Grill---Brow---Fan---Ff---Ord---Cook-----------------------------------------------
		[ ](FryModeLambda *fryModeLambda) {
			fryModeLambda->modeCom(settedMode, fryModeLambda->firstTemp);
			HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);
			buf_485[MODE] = settedMode;
		},
        //--------------------------------------------------5 - Light------------------------------------------------------------------
		[ ](FryModeLambda *fryModeLambda) {
			Heat heat;
			heat.all_off(); //Выключаем все
			fryModeLambda->firstRegim(fryModeLambda->firstTemp);
			HAL_TIM_Base_Stop_IT(&htim4);
		} };
//--------------------------- Метод для вызова лямбда-функции по индексу----------------------------------
uint8_t FryModeLambda::ModeSetLambda(uint8_t modeCookNew) {
	if (modeCookNew == modeCookOld && modeCookNew != 3) return modeCookNew; /* Ранний выход, если ничего не изменилось и
    повтор и если режим "Set" иначе температура платы будет измеряться только один раз при включении режима*/
	static constexpr uint8_t modeMapping[11] = {0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 5};//Массив лямбд режимов приготовления
	uint8_t indexSet = (modeCookNew < 11) ? modeMapping[modeCookNew] : 0;//Выбираем лямбду соответствующую режиму
	buf_485[MODE] = modeCookNew;
	ModeSetLambda(modeCookLambdaArray[indexSet], modeCookNew); // Передаем лямбду по индексу
	modeCookOld = modeCookNew;
	return modeCookNew;//Номер режима приготовления
}
//Вызываем лямбду, переданную как параметр
void FryModeLambda::ModeSetLambda(std::function<void(FryModeLambda*)> lambda, uint8_t modeCookNew) {
	lambda(this); // Передаем указатель на текущий объект
	modeWrite(modeCookNew);
}
