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

class Protection {
public:
	Protection();
	void checkProtection();//Проверка срабатывания защит ТЭНов
	private:
	    // Обработка любого обрыва (11, 12, 13)
	    void handleOpen(const uint8_t errorCode);
	    // Обработка любого залипания (21, 22, 23)
	    void handleStuck(const uint8_t errorCode);

	public:
	virtual ~Protection();
};

#endif /* PROTECTION_PROTECTION_HPP_ */
