/*
 * PlateTemp.hpp
 *
 *  Created on: 9 нояб. 2026 г.
 *      Author: erlbriton
 */

#ifndef PLATE_TEMP_HPP_
#define PLATE_TEMP_HPP_

#include "DS18B20.hpp"

class PlateTemp {
public:
    PlateTemp();
    void init();       // Аналог Sensor_init()
    void plateTemp();  // Аналог plateTemp()

private:
    OneWire m_oneWire;
    DS18B20 m_sensor;
};

#endif // PLATE_TEMP_HPP_
