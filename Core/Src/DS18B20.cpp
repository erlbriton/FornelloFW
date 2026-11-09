/*
 * DS18B20.cpp
 *
 *  Created on: 9 нояб. 2026 г.
 *      Author: erlbriton
 */

#include "DS18B20.hpp"

DS18B20::DS18B20(OneWire& oneWire) : m_oneWire(oneWire) {}

void DS18B20::init() {
    m_isConnected = false;
    m_isInitialized = true;
}

uint8_t DS18B20::calculateChecksum(const uint8_t* data, uint8_t length) {
    uint8_t checksum = 0;
    while (length--) {
        uint8_t currentByte = *data++;
        for (uint8_t i = 8; i; i--) {
            uint8_t temp = (checksum ^ currentByte) & 0x01;
            checksum >>= 1;
            if (temp) {
                checksum ^= 0x8C;
            }
            currentByte >>= 1;
        }
    }
    return checksum;
}

DS18B20::Status DS18B20::executeCommand(uint8_t commandCode, uint8_t rxBytes, uint8_t txBytes, uint8_t* data) {
    if (!m_isConnected) return Status::Error;

    m_oneWire.processByte(commandCode);

    if (rxBytes != 0) {
        for (uint8_t i = 0; i < rxBytes; i++) {
            data[i] = m_oneWire.processByte(0xFF);
        }
        uint8_t checkSum = calculateChecksum(data, rxBytes - 1);
        if (checkSum != data[rxBytes - 1]) {
            return Status::Error;
        }
    } else {
        for (uint8_t i = 0; i < txBytes; i++) {
            m_oneWire.processByte(data[i]);
        }
    }
    return Status::OK;
}

void DS18B20::waitForConversionFinished() {
    uint8_t data = m_oneWire.processBit(1);
    while(data != 0xFF) {
        data = m_oneWire.processBit(1);
    }
}

DS18B20::Status DS18B20::initializationCommand() {
    if (!m_isInitialized) return Status::Error;

    if (m_oneWire.reset() == OneWire::Status::OK) {
        m_isConnected = true;
        return Status::OK;
    } else {
        m_isConnected = false;
        return Status::Error;
    }
}

DS18B20::Status DS18B20::readRom() {
    std::array<uint8_t, 8> rxData{};
    Status result = executeCommand(0x33, 8, 0, rxData.data());
    if (result != Status::OK) return result;

    for (uint8_t i = 0; i < 6; i++) {
        m_serialNumber[i] = rxData[1 + i];
    }
    return Status::OK;
}

DS18B20::Status DS18B20::skipRom() {
    uint8_t dummy;
    return executeCommand(0xCC, 0, 0, &dummy);
}

DS18B20::Status DS18B20::convertT(WaitCondition wait) {
    uint8_t dummy;
    Status result = executeCommand(0x44, 0, 0, &dummy);

    if (result != Status::OK) return result;

    if (wait == WaitCondition::Data) {
        waitForConversionFinished();
    } else if (wait == WaitCondition::Delay) {
        uint32_t delayMs = 750;
        switch (m_configRegister) {
            case 0x1F: delayMs = 94; break;
            case 0x3F: delayMs = 188; break;
            case 0x5F: delayMs = 375; break;
            case 0x7F: delayMs = 750; break;
        }
        HAL_Delay(delayMs);
    }
    return Status::OK;
}

DS18B20::Status DS18B20::readScratchpad() {
    std::array<uint8_t, 9> rxData{};
    Status result = executeCommand(0xBE, 9, 0, rxData.data());
    if (result != Status::OK) return result;

    m_tempLimitHigh = rxData[2];
    m_tempLimitLow  = rxData[3];
    m_configRegister = rxData[4];

    uint16_t tRegValue = (rxData[1] << 8) | rxData[0];
    bool isNegative = (tRegValue & 0xF800) != 0;

    if (isNegative) {
        tRegValue = (0xFFFF - tRegValue + 1);
    }

    switch (m_configRegister) {
        case 0x1F: tRegValue &= 0x7F8; break;
        case 0x3F: tRegValue &= 0x7FC; break;
        case 0x5F: tRegValue &= 0x7FE; break;
        case 0x7F: tRegValue &= 0x7FF; break;
        default:   tRegValue &= 0x7FF; break;
    }

    m_temperature = static_cast<float>(tRegValue) * 0.0625f;
    if (isNegative) m_temperature *= -1.0f;

    return Status::OK;
}

DS18B20::Status DS18B20::writeScratchpad(const std::array<uint8_t, 3>& data) {
    Status result = executeCommand(0x4E, 0, 3, const_cast<uint8_t*>(data.data()));
    if (result != Status::OK) return result;

    m_tempLimitHigh = data[0];
    m_tempLimitLow  = data[1];
    m_configRegister = data[2];
    return Status::OK;
}
