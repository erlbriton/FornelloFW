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
	    // Обработка любого обрыва (11, 12, 13)
	    void handleError(const uint8_t errorCode, bool);

	public:
	virtual ~Protection();
};

#endif /* PROTECTION_PROTECTION_HPP_ */
