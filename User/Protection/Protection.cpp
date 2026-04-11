/*
 * Protection.cpp
 *
 *  Created on: Dec 7, 2025
 *      Author: erlbriton
 */

#include "Protection.hpp"
#include "tim.h"

Protection::Protection() {}

EXTIManager extiManager;
void Protection::checkProtection(){
	uint8_t errorCode = extiManager.checkHeaters(Button::scanButton());
	if (errorCode == 0){
		buf_485[19] = 0;
		HAL_GPIO_WritePin(Off_GPIO_Port, Off_Pin, GPIO_PIN_SET);
		return; // Ошибок нет, выходим
	}
	//Теперь проверяем 3 предупреждения и 3 критические ошибки
	switch (errorCode) {
	        // Группируем все обрывы
	        case 11://Down
	        case 12://Grill
	        case 13://Right
	            this->handleError(errorCode, true);
	            break;
	        // Группируем все залипания
	        case 21://Down
	        case 22://Grill
	        case 23://Right
	            this->handleError(errorCode, false);
	            break;

	        default:
	            break;
	    }
	}
void Protection::handleError(const uint8_t errorCode, bool isWarning) {
	buf_485[19] = errorCode;//Передаем в дисплей код ошибки
	HAL_GPIO_WritePin(Off_GPIO_Port, Off_Pin, (GPIO_PinState)isWarning);//Включаем расцепитель
	Button::regim1Button();
}

Protection::~Protection() {}

