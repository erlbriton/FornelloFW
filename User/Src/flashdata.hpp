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
#include <array>
#include <cstdint>

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
//-----------------Заполнение массива UART для отсылки в дисплей----------------
static constexpr uint8_t CHECK_FIRST =  0;
static constexpr uint8_t UINITS_MIN  =  1;
static constexpr uint8_t DEC_MIN     =  2;
static constexpr uint8_t UINITS_HOUR =  3;
static constexpr uint8_t DEC_HOUR    =  4;
static constexpr uint8_t TEMP_1      =  5;
static constexpr uint8_t TEMP_2      =  6;
static constexpr uint8_t TEMP_3      =  7;
static constexpr uint8_t TEMP_SET_1  =  8;
static constexpr uint8_t TEMP_SET_2  =  9;
static constexpr uint8_t TEMP_SET_3  = 10;
static constexpr uint8_t MODE        = 11;
static constexpr uint8_t WATCH_DOTS  = 12;
static constexpr uint8_t FIRE_DOWN   = 13;
static constexpr uint8_t FIRE_SIDE   = 14;
static constexpr uint8_t FIRE_UP     = 15;
static constexpr uint8_t TEMP_PLT_1  = 16;
static constexpr uint8_t TEMP_PLT_2  = 17;
static constexpr uint8_t FRAME_WATCH = 18;
static constexpr uint8_t ERROR_NUM   = 20;
static constexpr uint8_t CHECK_LAST  = 21;


#endif /* INC_FLASHDATA_H_ */
