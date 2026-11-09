/*
 * PlateTemp.cpp
 *
 *  Created on: 9 нояб. 2026 г.
 *      Author: erlbriton
 */

#include "PlateTemp.hpp"
#include "tim.h"
#include "Control.hpp"

// Константы индексов (адаптируйте под ваш проект, если они в другом месте)
#define CHECK_FIRST 0
#define CHECK_LAST  20
#define TEMP_SET_1  5
#define TEMP_SET_2  6
#define TEMP_SET_3  7
#define MODE        11

PlateTemp::PlateTemp(uint8_t* displayBuffer, uint8_t bufferSize)
    : m_oneWire(&huart2),
      m_sensor(m_oneWire),
      m_buffer(displayBuffer),
      m_bufferSize(bufferSize)
{
}

void PlateTemp::init() {
    // 🎯 ПОЛНЫЙ КОД ИНИЦИАЛИЗАЦИИ (как было в Sensor_init)
    m_sensor.init();
    m_sensor.initializationCommand();
    m_sensor.readRom();
    m_sensor.readScratchpad();

    std::array<uint8_t, 3> settings = {
        m_sensor.getTemperatureLimitHigh(),
        m_sensor.getTemperatureLimitLow(),
        0x7F // 12-bit
    };

    m_sensor.initializationCommand();
    m_sensor.skipRom();
    m_sensor.writeScratchpad(settings);
}

void PlateTemp::plateTemp() {
    // Защита от пустого указателя
    if (m_buffer == nullptr || m_bufferSize < 12) {
        return;
    }

    m_sensor.initializationCommand();
    m_sensor.skipRom();
    m_sensor.convertT(DS18B20::WaitCondition::Data);
    m_sensor.initializationCommand();
    m_sensor.skipRom();
    m_sensor.readScratchpad();

    uint8_t tempPlate = static_cast<uint8_t>(m_sensor.getTemperature() * 0.77777f);

    // Пишем в сохраненный указатель
    m_buffer[CHECK_FIRST] = 151;
    m_buffer[CHECK_LAST]  = 151;
    m_buffer[TEMP_SET_1]  = tempPlate % 10;
    m_buffer[TEMP_SET_2]  = tempPlate / 10;
    m_buffer[TEMP_SET_3]  = 0;
    m_buffer[MODE]        = Control::readAdc(1);
}
