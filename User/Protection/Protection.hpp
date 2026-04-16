/*
 * Protection.hpp
 *
 *  Created on: Dec 7, 2025
 *      Author: erlbriton
 */

#ifndef PROTECTION_PROTECTION_HPP_
#define PROTECTION_PROTECTION_HPP_

#include "stm32f4xx.h"
#include "CallBacks.hpp"
#include "Button.hpp"

class Protection {
public:
	Protection();
	void checkProtection();//Проверка срабатывания защит ТЭНов
	private:
	    // Обработка любого обрыва (1, 2, 3)
	    void handleError(const uint8_t errorCode, bool);
	    uint32_t tripleTenTimer = 0;

	public:
	virtual ~Protection();
};

#endif /* PROTECTION_PROTECTION_HPP_ */
