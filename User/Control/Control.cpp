/*
 * Control.cpp
 *
 *  Created on: Jan 16, 2024
 *      Author: erlbriton
 */
#include "Control.hpp"
#include "Heat.hpp"
#include <numeric>
#include "SetTimer.hpp"
#include "Fram.hpp"

/* Сопротивление терморезистора в зависимости от температуры
   25 - 1.148 om - 3663
   49 - 1.214 om - 3550
   74 - 1.29  om - 3423
   150- 1.542 om - 3106
   201- 1.749 om - 2875*/
uint32_t Control::bakeTemper() {//Измеряем температуру в духовке
	tempADC = adcTemp[0];
	tVref = adcTemp[1];
	kVref = ((float) VREF / (float) tVref);
	ovenTemper = (((tempADC - 592.5) / 11.83) * kVref); //Текущая температура в духовке//////////////////////
	if (ovenTemper < 15.0f) {//На время наладки
	    ovenTemper = 15.0f;
	}
	buf_485[CHECK_FIRST] = 151;
	buf_485[CHECK_LAST] = 151;
	buf_485[TEMP_1] = ovenTemper % 10; //Единицы
	buf_485[TEMP_2] = (ovenTemper / 10) % 10; //Десятки
	buf_485[TEMP_3] = ovenTemper / 100; //Сотни
	return ovenTemper;
}
//Измеряем АЦП канала "РЕЖИМ" 3 раза или сколько нужно раз(одного явно мало)
uint8_t Control::readAdc(uint8_t numSamples) {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < numSamples; i++) {
        HAL_ADC_Start(&hadc2);
        uint32_t startTime = HAL_GetTick(); // Ожидание завершения с таймаутом
        while (!(ADC2->SR & ADC_SR_EOC)) {
            if ((HAL_GetTick() - startTime) > 10) { // 10 мс таймаут
                return 0; // Ошибка
            }
        }
        uint16_t adcRaw = ADC2->DR; // Чтение результата
        sum += adcRaw; // Суммирование
    }
    uint16_t averageAdc = sum / numSamples; // Вычисление среднего
    // Проверка границ (std::array имеет метод .size() — используем его!)
    if (averageAdc >= rawADC.size()) {
        return 0; // Ошибка
    }
    // 🎯 А ВОТ ТЕПЕРЬ МЫ МОЖЕМ ПОЛУЧИТЬ РЕЖИМ
    uint8_t currentMode = rawADC[averageAdc];

    // 🎯 И ТЕПЕРЬ ПРОВЕРЯЕМ, НУЖНО ЛИ ПИСАТЬ ВО FRAM
    static uint8_t oldMode = 0xFF; // Храним прошлый режим
    if (currentMode != oldMode) {
        Fram::elementFram(Fram::TARGET_MODE, currentMode);
        oldMode = currentMode;
    }
    return currentMode; // Возвращаем номер режима
}

//---------------------------Восстановление информации после сброса или отключения------------------------
void Control::backInfo() {
	Fram fram;
	uint8_t targetTemp = fram.framRdOneAddress(Fram::TARGET_TEMP);
	TIM2->CNT = targetTemp;//Fram::framRdOneAddress(Fram::TARGET_TEMP);
	uint8_t modeBack = fram.framRdOneAddress(Fram::TARGET_MODE); //Восстанавливаем номер режима приготовления
	Button::dirTime = fram.framRdOneAddress(Fram::DIRECTION_TIME); //Направление счета времени
	SetTimer::minCounterDec = (fram.framRdOneAddress(Fram::H_BYTE_MINUTS_COUNTER) << 8) | fram.framRdOneAddress(Fram::L_BYTE_MINUTS_COUNTER);//Соединяем старший и младший байты счетчика минут
	SetTimer::minCounterInc = SetTimer::minCounterDec;//Счетчик инкремента и декремента времени здесь равны
	buf_485[0] = buf_485[FRAME_WATCH] = 151; //Проверочный код посылки
//#################################Для калибровки###################################################
//	buf_485[1] = framRdWr.massive_rd[2] % 10; //Единицы минут
//	buf_485[2] = framRdWr.massive_rd[2] / 10; //Десятки минут
//	buf_485[3] = framRdWr.massive_rd[3] % 10; //Единицы часов
//	buf_485[4] = framRdWr.massive_rd[3] / 10; //Десятки часов
//#################################Для калибровки###################################################
	buf_485[TEMP_SET_1] = targetTemp % 10; //Единицы...
	buf_485[TEMP_SET_2] = targetTemp % 100 / 10; //...десятки ........
	buf_485[TEMP_SET_3] = targetTemp / 100; //...сотни заданной температуры
	buf_485[MODE] = modeBack; //Режим приготовления
	Button::setButtonRegim(2);//Восстанавливаем 3-й режим кнопки
	SetTimer::number_Iterator_Decrement = true;//Восстанавливаем второй проход инкремента времени
	SetTimer::number_Iterator_Increment = true;//Восстанавливаем второй проход декремента времени
	FryModeLambda::modeCookOld = modeBack; //Восстанавливаем информацию, что проход по установке режима уже сделан(ПРОВЕРИТЬ)
	GPIOA->BSRR = GPIO_PIN_12;//Свет
	GPIOC->BSRR = GPIO_PIN_7 << 16U; // HC595 вкл
	Heat::ajustHeat595(modeBack);//Запускаем продолжение приготовления.
}

