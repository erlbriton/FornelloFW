/*
 * PlateTemp.cpp
 *
 *  Created on: 9 нояб. 2026 г.
 *      Author: erlbriton
 */

#include "PlateTemp.hpp"
#include "tim.h"
#include "Control.hpp" // Для Control::readAdc()

// Внешняя переменная буфера (глобальная в main)
extern uint8_t buf_485[21];

// Индексы для buf_485 (адаптируйте под ваши реальные значения из main)
// Если у вас они в enum или #define, используйте их
#define CHECK_FIRST 0
#define CHECK_LAST  20
#define TEMP_SET_1  5
#define TEMP_SET_2  6
#define TEMP_SET_3  7
#define MODE        11

PlateTemp::PlateTemp() : m_oneWire(&huart2), m_sensor(m_oneWire) {}

void PlateTemp::init() {
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
    m_sensor.initializationCommand();
    m_sensor.skipRom();
    m_sensor.convertT(DS18B20::WaitCondition::Data);
    m_sensor.initializationCommand();
    m_sensor.skipRom();
    m_sensor.readScratchpad();

    uint8_t tempPlate = static_cast<uint8_t>(m_sensor.getTemperature() * 0.77777f);

    buf_485[CHECK_FIRST] = 151; // Проверочный код
    buf_485[CHECK_LAST]  = 151; // Проверочный код
    buf_485[TEMP_SET_1]  = tempPlate % 10; // Единицы
    buf_485[TEMP_SET_2]  = tempPlate / 10; // Десятки
    buf_485[TEMP_SET_3]  = 0;              // Сотни
    buf_485[MODE]        = Control::readAdc(1); // Режим
}
