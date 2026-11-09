/*
 * Temp_sensor.c
 *
 *  Created on: Jul 20, 2022
 *      Author: Yurij
 */
#include "DS18B20.h"
#include "onewire.h"
#include "tim.h"
#include "flashdata.hpp"
#include "Control.hpp"

DS18B20 temperatureSensor;
//ModeCook modeCook;

void Sensor_init(void)
{
    DS18B20_Init(&temperatureSensor, &huart2);
    DS18B20_InitializationCommand(&temperatureSensor);
    DS18B20_ReadRom(&temperatureSensor);
    DS18B20_ReadScratchpad(&temperatureSensor);
    uint8_t settings[3];
    settings[0] = temperatureSensor.temperatureLimitHigh;
    settings[1] = temperatureSensor.temperatureLimitLow;
    settings[2] = DS18B20_12_BITS_CONFIG;
    DS18B20_InitializationCommand(&temperatureSensor);
    DS18B20_SkipRom(&temperatureSensor);
    DS18B20_WriteScratchpad(&temperatureSensor, settings);
}

void plateTemp()
{
      DS18B20_InitializationCommand(&temperatureSensor);
      DS18B20_SkipRom(&temperatureSensor);
      DS18B20_ConvertT(&temperatureSensor, DS18B20_DATA);
      DS18B20_InitializationCommand(&temperatureSensor);
      DS18B20_SkipRom(&temperatureSensor);
      DS18B20_ReadScratchpad(&temperatureSensor);
      uint8_t tempPlate = temperatureSensor.temperature * 0.77777;
      buf_485[CHECK_FIRST] = 151;// Проверочный код
      buf_485[CHECK_LAST]  = 151;// Проверочный код
	  buf_485[TEMP_SET_1]  = tempPlate % 10;//Единицы температуры
	  buf_485[TEMP_SET_2]  = tempPlate / 10;//Десятки температуры
	  buf_485[TEMP_SET_3]  = 0;//Сотни температуры
	  buf_485[MODE]        = Control::readAdc(1);//Режим
}



