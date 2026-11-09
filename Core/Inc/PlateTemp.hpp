/*
 * PlateTemp.hpp
 *
 *  Created on: 9 нояб. 2026 г.
 *      Author: erlbriton
 */

#ifndef PLATE_TEMP_HPP_
#define PLATE_TEMP_HPP_

#include "DS18B20.hpp"
#include <cstdint>

class PlateTemp {
public:
    // 🎯 Конструктор теперь принимает указатель на буфер и его размер
    PlateTemp(uint8_t* displayBuffer, uint8_t bufferSize);

    void init();
    void plateTemp(); // Метод снова без параметров

private:
    OneWire m_oneWire;
    DS18B20 m_sensor;

    // 🎯 Храним указатель и размер внутри класса
    uint8_t* m_buffer;
    uint8_t  m_bufferSize;
};

#endif // PLATE_TEMP_HPP_
