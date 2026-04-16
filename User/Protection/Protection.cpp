/*
 * Protection.cpp
 *
 * Created on: Dec 7, 2025
 * Author: erlbriton
 */

#include "Protection.hpp"
#include "tim.h"

Protection::Protection() : tripleTenTimer(0) {} // Инициализация в конструкторе

EXTIManager extiManager;

void Protection::checkProtection(){
    // Получаем текущее время один раз для всех расчетов в этой итерации
    uint32_t now = HAL_GetTick();

    // --- Логика для ошибки №7 (3 ТЭНа одновременно) с выдержкой 5 секунд ---
    if (extiManager.getRealStatus() == 0x07) {
        if (tripleTenTimer == 0) {
            tripleTenTimer = now; // Начинаем отсчет
        }

        if ((now - tripleTenTimer) > 5000) { // Если горят более 5 секунд
            this->handleError(7, false);     // Ошибка 7, критическая
            return;
        }
    } else {
        tripleTenTimer = 0; // Сброс таймера
    }

    // --- Стандартная проверка остальных ошибок через EXTIManager ---
    uint8_t errorCode = extiManager.checkHeaters(Button::scanButton());

    if (errorCode == 0){
        if (tripleTenTimer == 0) {
            buf_485[19] = 0;
            HAL_GPIO_WritePin(Off_GPIO_Port, Off_Pin, GPIO_PIN_SET);
        }
        return;
    }

    switch (errorCode) {
        case 1: // Down
        case 2: // Grill
        case 3: // Right
            this->handleError(errorCode, true);
            break;

        case 4: // Down
        case 5: // Grill
        case 6: // Right
            this->handleError(errorCode, false);
            break;

        default:
            break;
    }
}

void Protection::handleError(const uint8_t errorCode, bool isWarning) {
    buf_485[19] = errorCode;

    // Аппаратный сигнал на расцепитель/реле
    HAL_GPIO_WritePin(Off_GPIO_Port, Off_Pin, (GPIO_PinState)isWarning);

    if(errorCode > 3){
        Button::regim1Button(); // Сброс программного режима при аварии
    }
}

Protection::~Protection() {}
