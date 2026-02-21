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

DS18B20 temperatureSensor;

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
      vu8 tempPlate = temperatureSensor.temperature * 0.77777;
      buf_485[0] = buf_485[19] = 151;
	  buf_485[8] = tempPlate % 10;//Единицы
	  buf_485[9] = tempPlate / 10;//Десятки
	  buf_485[10] = 0;//Сотни
	  //buf_485[11] = modeCook;////////////////////////////////////////////////////////////////////////////
}



