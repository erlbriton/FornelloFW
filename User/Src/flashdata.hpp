/*
 * Flashdata.h
 *
 *  Created on: Feb 8, 2023
 *      Author: Yurij
 */
#ifndef INC_FLASHDATA_HPP_
#define INC_FLASHDATA_HPP_

#include "main.h"
#include <stdbool.h>

#define gettingButton (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8))//Состояние кнопки
#define doorRd HAL_GPIO_ReadPin(Door_GPIO_Port, Door_Pin)//Состояние двери


extern uint8_t buf_485[ ]; //Массив для посылки в дисплей========================================================

enum {
	off,
	pre,
	dry,
	set,
	grill,
	brow,
	fan,
	forseFan,
	ord,
	cook,
	light
};

#endif /* INC_FLASHDATA_H_ */
