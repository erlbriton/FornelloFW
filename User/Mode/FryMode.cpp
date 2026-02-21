/*
 * FryMode.cpp
 *
 *  Created on: Jan 8, 2024
 *      Author: erlbriton
 */

#include "FryMode.hpp"
#include "Heat.hpp"

void FryModeLambda::modeCom(vu16 averageAdc, vu8 tempFirstMode) {//Метод, запускаемый при переключении режима
	firstRegim(tempFirstMode);//Устанавливаем первоначальный режим
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);//Запускаем энкодер в работу
	buf_485[11] =  Fram::elementFram(1);//Номер режима
}
//-----------------------------------Начальный режим---------------------------------------
void FryModeLambda::firstRegim(vu8 tempFirst) {
	Heat::spOn();//Писк
	Button::flagOneButton = false;//Флаг первого режима кнопки
	TIM2->CNT = 0;
	uint8_t indexesToClear[ ] = {1, 2, 3, 4, 12, 13, 14, 15, 18};//Стираем рамку вокруг часов, точки в часах, все пламя
	for (uint8_t i = 0; i < sizeof(indexesToClear)/sizeof(indexesToClear[0]); ++i) {
	    buf_485[indexesToClear[ i ] ] = 0;
	}
	buf_485[0] = buf_485[19] = 151; //Проверочный код посылки
	buf_485[8] = tempFirst % 100 % 10; //Единицы)
	buf_485[9] = tempFirst % 100 / 10; //Десятки
	buf_485[10] = tempFirst / 100;     //Сотни
	Button::regim1Button(); //Включаем 1-й режим button
	//----------Сбрасываем все выходы у HC595---------------------
	GPIOA->BSRR |= GPIO_PIN_8 << 16U;	//MR off
	GPIOC->BSRR |= GPIO_PIN_6; //Latch on
	GPIOC->BSRR |= GPIO_PIN_6 << 16U; //Latch off
	GPIOA->BSRR |= GPIO_PIN_8; //MR on
	//-----------------------------------------------------------
	Fram::elementFram(5, 0);//Сброса во время работы не было
	Fram::elementFram(0, tempFirst);//Записываем во фрам начальную температуру
	TIM2->CNT = tempFirst; //Записываем начальную температуру в энкодер
	Heat::soundPre = false;
}
//--------------------------/Записываем номер включенного режима---------------------------------------------
void FryModeLambda::modeWrite(vu8 modeCookAveADC) {
	settedMode = Fram::elementFram(1);
	buf_485[0] = 151;
	buf_485[19] = 151;
	buf_485[11] = settedMode;
}
//-----------------------------------------Методы режимов(массив лямбда функций)----------------------------------------------
const std::array<std::function<void(FryModeLambda*)>, 6> FryModeLambda::modeCookLambdaArray =
		//--------------------------------------------0  -  Off--------------------------------------------------------------------------
		{ [ ](FryModeLambda* fryModeLambda) {
			Heat::spOn();//Звук
			Heat::all_off(); // Выключаем все тены
			GPIOA->BSRR |= GPIO_PIN_12 << 16U; // Свет выкл
			buf_485[11] = settedMode;//Off
		},
//----------------------------------------------------1 - Pre----------------------------------------------------------------------------
		[ ](FryModeLambda* fryModeLambda) {
			fryModeLambda->firstRegim(firstTemp);
			HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);
			HAL_TIM_Base_Stop_IT(&htim4);
			buf_485[11] = settedMode; // Pre
		},
		//---------------------------------------------2 - Dry----------------------------------------------------------------------------------
		[ ](FryModeLambda* fryModeLambda) {
			fryModeLambda->modeCom(settedMode, fryModeLambda->firstTempDry);
			HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);
		},
		//----------------------------------------------3 - Set----------------------------------------------------------------------------------
		[ ](FryModeLambda* fryModeLambda) {//Set
			Heat::all_off(); //Выключаем все
			//HAL_GPIO_WritePin(fanOut, GPIO_PIN_RESET);
			GPIOA->BSRR |= GPIO_PIN_11 << 16U;
			HAL_TIM_Encoder_Stop(&htim2, TIM_CHANNEL_1);
			HAL_TIM_Base_Stop_IT(&htim4);
			plateTemp();
		},
		//-------------------------4 - Grill---Brow---Fan---Ff---Ord---Cook-----------------------------------------------
		[ ](FryModeLambda *fryModeLambda) {
			fryModeLambda->modeCom(settedMode, fryModeLambda->firstTemp);
			HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);
			buf_485[11] = settedMode;
		},
        //--------------------------------------------------5 - Light------------------------------------------------------------------
		[ ](FryModeLambda *fryModeLambda) {
			Heat::all_off(); //Выключаем все
			fryModeLambda->firstRegim(fryModeLambda->firstTemp);
			HAL_TIM_Base_Stop_IT(&htim4);
		} };
//--------------------------- Метод для вызова лямбда-функции по индексу----------------------------------
vu8 FryModeLambda::ModeSetLambda(vu8 modeCookNew) {
	if (modeCookNew == modeCookOld && modeCookNew != 3) return modeCookNew; /* Ранний выход, если ничего не изменилось и
    повтор и если режим "Set" иначе температура платы будет измеряться только один раз при включении режима*/
	static constexpr vu8 modeMapping[11] = {0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 5};//Массив лямбд режимов приготовления
	vu8 indexSet = (modeCookNew < 11) ? modeMapping[modeCookNew] : 0;//Выбираем лямбду соответствующую режиму
	buf_485[11] = modeCookNew;
	ModeSetLambda(modeCookLambdaArray[indexSet], modeCookNew); // Передаем лямбду по индексу
	modeCookOld = modeCookNew;
	return modeCookNew;//Номер режима приготовления
}
//Вызываем лямбду, переданную как параметр
void FryModeLambda::ModeSetLambda(std::function<void(FryModeLambda*)> lambda, vu8 modeCookNew) {
	lambda(this); // Передаем указатель на текущий объект
	modeWrite(modeCookNew);
}
