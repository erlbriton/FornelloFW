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
//#define downIn HAL_GPIO_ReadPin(down_In_GPIO_Port, down_In_Pin)//Читаем состояние...
//#define rightIn HAL_GPIO_ReadPin(Right_in_GPIO_Port, Right_in_Pin)//...выходов...
//#define grillIn HAL_GPIO_ReadPin(grill_In_GPIO_Port, grill_In_Pin)//HC595

typedef  uint8_t vu8;
typedef  uint16_t vu16;
typedef  uint32_t vu32;

//#ifdef __cplusplus
//extern "C" {
//#endif

extern vu8 buf_485[ ]; //Массив для посылки в дисплей========================================================

//#ifdef __cplusplus
//}
//#endif

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
