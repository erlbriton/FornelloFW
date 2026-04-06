/*
 * Protection.cpp
 *
 *  Created on: Dec 7, 2025
 *      Author: erlbriton
 */

#include "Protection.hpp"
#include "tim.h"

Protection::Protection() {}

//extern EXTIManager eXTIManager;
EXTIManager extiManager;
void Protection::checkProtection(){
	volatile uint8_t errorCode = extiManager.checkHeaters(Button::scanButton());
	if (errorCode == 0) return; // Ошибок нет, выходим
	//Теперь проверяем 3 предупреждения и 3 критические ошибки
	switch (errorCode) {
	        // Группируем все обрывы
	        case 11://Down
	        case 12://Grill
	        case 13://Right
	            this->handleOpen(errorCode);
	            break;

	        // Группируем все залипания
	        case 21://Down
	        case 22://Grill
	        case 23://Right
	            this->handleStuck(errorCode);
	            break;

	        default:
	            break;
	    }
	}
volatile uint8_t global_errorCode; // Глобально
void Protection::handleOpen(const uint8_t errorCode) {
    // Внутри можно узнать какой ТЭН: 11-Down, 12-Grl, 13-Right
    // TODO: Алгоритм при обрыве ТЭНа
	global_errorCode = errorCode;
}

// Реализация обработки залипания
void Protection::handleStuck(const uint8_t errorCode) {
    // Внутри можно узнать какой ТЭН: 21-Down, 22-Grl, 23-Right
    // TODO: Алгоритм при залипании (Авария)
	global_errorCode = errorCode; // Это компилятор не удалит
}

Protection::~Protection() {}

