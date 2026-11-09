/*
 * OneWire.cpp
 *
 *  Created on: 9 нояб. 2026 г.
 *      Author: erlbriton
 */


#include "OneWire.hpp"

OneWire::OneWire(UART_HandleTypeDef* huart) : m_huart(huart) {}

void OneWire::setBaudrate(uint32_t baudrate) {
    uint32_t pclk = 0;
    m_huart->Init.BaudRate = baudrate;
#if defined(USART6) && defined(UART9) && defined(UART10)
    if ((m_huart->Instance == USART1) || (m_huart->Instance == USART6) ||
        (m_huart->Instance == UART9)  || (m_huart->Instance == UART10)) {
        pclk = HAL_RCC_GetPCLK2Freq();
    }
#elif defined(USART6)
    if ((m_huart->Instance == USART1) || (m_huart->Instance == USART6)) {
        pclk = HAL_RCC_GetPCLK2Freq();
    }
#else
    if (m_huart->Instance == USART1) {
        pclk = HAL_RCC_GetPCLK2Freq();
    }
#endif
    else {
        pclk = HAL_RCC_GetPCLK1Freq();
    }

    if (m_huart->Init.OverSampling == UART_OVERSAMPLING_8) {
        m_huart->Instance->BRR = UART_BRR_SAMPLING8(pclk, m_huart->Init.BaudRate);
    } else {
        m_huart->Instance->BRR = UART_BRR_SAMPLING16(pclk, m_huart->Init.BaudRate);
    }
}

uint8_t OneWire::processBit(uint8_t bit) {
    uint8_t txData = (bit == 0) ? 0x00 : 0xFF;
    uint8_t rxData = 0x00;
    HAL_UART_Transmit(m_huart, &txData, 1, UART_TIMEOUT);
    HAL_UART_Receive(m_huart, &rxData, 1, UART_TIMEOUT);
    return rxData;
}

uint8_t OneWire::processByte(uint8_t byte) {
    uint8_t rxByte = 0x00;
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t txBit = (byte >> i) & 0x01;
        uint8_t rxBit = 0;
        uint8_t tempRxData = processBit(txBit);
        if (tempRxData == 0xFF) {
            rxBit = 1;
        }
        rxByte |= (rxBit << i);
    }
    return rxByte;
}

OneWire::Status OneWire::reset() {
    processByte(0x43);
    Status status = Status::OK;
    uint8_t txByte = RESET_BYTE;
    uint8_t rxByte = 0x00;

    setBaudrate(RESET_BAUDRATE);
    HAL_UART_Transmit(m_huart, &txByte, 1, UART_TIMEOUT);
    HAL_UART_Receive(m_huart, &rxByte, 1, UART_TIMEOUT);
    setBaudrate(BAUDRATE);

    if (rxByte == txByte) {
        status = Status::Error;
    }
    return status;
}
