/*
 * onewire.h
 *
 *  Created on: Jul 12, 2022
 *      Author: Yurij
 */

#ifndef INC_ONEWIRE_H_
#define INC_ONEWIRE_H_

#include "main.h"
#include "usart.h"

#define ONEWIRE_BAUDRATE                                     115200
#define ONEWIRE_RESET_BAUDRATE                               9600
#define ONEWIRE_RESET_BYTE                                   0xF0
#define ONEWIRE_UART_TIMEOUT                                 10
#define ONEWIRE_BITS_NUM                                     8
typedef enum
{
  ONEWIRE_OK              = 0x00,
  ONEWIRE_ERROR           = 0x01,
} ONEWIRE_Status;

uint8_t OneWire_ProcessByte(UART_HandleTypeDef *huart, uint8_t byte);
uint8_t OneWire_ProcessBit(UART_HandleTypeDef *huart, uint8_t bit);

ONEWIRE_Status OneWire_Reset(UART_HandleTypeDef *huart);

#endif /* INC_ONEWIRE_H_ */
