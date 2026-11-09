/*
 * PlateTemp.cpp
 *
 *  Created on: 9 нояб. 2026 г.
 *      Author: erlbriton
 */

#include "PlateTemp.hpp"
#include "tim.h"
#include "Control.hpp"

// Константы для индексов (если они не объявлены глобально)
// #define CHECK_FIRST 0 ... и т.д.

// 🎯 Инициализируем поля в конструкторе
PlateTemp::PlateTemp(uint8_t* displayBuffer, uint8_t bufferSize)
    : m_oneWire(&huart2),
      m_sensor(m_oneWire),
      m_buffer(displayBuffer),
      m_bufferSize(bufferSize)
{
}

void PlateTemp::init() {
    // ... (код инициализации датчика остается без изменений) ...
}

void PlateTemp::plateTemp() {
    // Защита от пустого указателя (на всякий случай)
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

    // Используем сохраненный указатель m_buffer
    m_buffer[CHECK_FIRST] = 151;
    m_buffer[CHECK_LAST]  = 151;
    m_buffer[TEMP_SET_1]  = tempPlate % 10;
    m_buffer[TEMP_SET_2]  = tempPlate / 10;
    m_buffer[TEMP_SET_3]  = 0;
    m_buffer[MODE]        = Control::readAdc(1);
}
