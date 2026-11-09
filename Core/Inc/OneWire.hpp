/*
 * OneWire.hpp
 *
 *  Created on: 9 нояб. 2026 г.
 *      Author: erlbriton
 */

#ifndef ONEWIRE_HPP_
#define ONEWIRE_HPP_

#include "main.h"
#include "usart.h"
#include <cstdint>

class OneWire {
public:
    enum class Status { OK, Error };

    explicit OneWire(UART_HandleTypeDef* huart);

    uint8_t processByte(uint8_t byte);
    uint8_t processBit(uint8_t bit);
    Status reset();

private:
    UART_HandleTypeDef* m_huart;

    static constexpr uint32_t BAUDRATE = 115200;
    static constexpr uint32_t RESET_BAUDRATE = 9600;
    static constexpr uint8_t RESET_BYTE = 0xF0;
    static constexpr uint32_t UART_TIMEOUT = 10;

    void setBaudrate(uint32_t baudrate);
};

#endif // ONEWIRE_HPP_
