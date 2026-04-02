/*
 * Protection.cpp
 *
 *  Created on: Dec 7, 2025
 *      Author: erlbriton
 */

#include "Protection.hpp"
#include "tim.h"

Protection::Protection() {}

EXTIManager eXTIManager;
void Protection::checkProtection(){
	const uint8_t errorCode = eXTIManager.checkHeaters();
	if (errorCode == 0) return; // Ошибок нет, выходим
	//Теперь проверяем 3 предупреждения и 3 критические ошибки
	switch (errorCode) {
	        // Группируем все обрывы
	        case 11:
	        case 12:
	        case 13:
	            this->handleOpen(errorCode);
	            break;

	        // Группируем все залипания
	        case 21:
	        case 22:
	        case 23:
	            this->handleStuck(errorCode);
	            break;

	        default:
	            break;
	    }
	}

void Protection::handleOpen(const uint8_t errorCode) {
    // Внутри можно узнать какой ТЭН: 11-Down, 12-Grl, 13-Right
    // TODO: Алгоритм при обрыве ТЭНа
}

// Реализация обработки залипания
void Protection::handleStuck(const uint8_t errorCode) {
    // Внутри можно узнать какой ТЭН: 21-Down, 22-Grl, 23-Right
    // TODO: Алгоритм при залипании (Авария)
}

Protection::~Protection() {}

