/*
 * DS18B20.hpp
 *
 *  Created on: 9 нояб. 2026 г.
 *      Author: erlbriton
 */

#ifndef DS18B20_HPP_
#define DS18B20_HPP_

#include "OneWire.hpp"
#include <array>

class DS18B20 {
public:
    enum class Status { OK, Error };
    enum class WaitCondition { None, Data, Delay };

    explicit DS18B20(OneWire& oneWire);

    void init();
    Status initializationCommand(); // Сделано публичным для вызова из plateTemp()
    Status readRom();
    Status skipRom();
    Status convertT(WaitCondition wait);
    Status readScratchpad();
    Status writeScratchpad(const std::array<uint8_t, 3>& data);

    float getTemperature() const { return m_temperature; }
    uint8_t getTemperatureLimitHigh() const { return m_tempLimitHigh; }
    uint8_t getTemperatureLimitLow() const { return m_tempLimitLow; }
    uint8_t getConfigRegister() const { return m_configRegister; }

private:
    OneWire& m_oneWire;
    bool m_isConnected = false;
    bool m_isInitialized = false;

    std::array<uint8_t, 6> m_serialNumber{};
    uint8_t m_tempLimitLow = 0;
    uint8_t m_tempLimitHigh = 0;
    uint8_t m_configRegister = 0;
    float m_temperature = 0.0f;

    Status executeCommand(uint8_t commandCode, uint8_t rxBytes, uint8_t txBytes, uint8_t* data);
    uint8_t calculateChecksum(const uint8_t* data, uint8_t length);
    void waitForConversionFinished();
};

#endif // DS18B20_HPP_
