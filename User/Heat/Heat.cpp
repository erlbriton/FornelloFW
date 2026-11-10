/*
 * Heat.cpp
 *
 *  Created on: Jan 25, 2024
 *      Author: erlbriton
 */

#include "Heat.hpp"
#include "Button.hpp"
#include "Fram.hpp"
#include "Control.hpp"
#include "SetTimer.hpp"
#include "melody_gpio.hpp"

#define safeTemperature 90//Температура включения внешнего кулера


//------------------------------Главный метод--------------------------------------------------
void
Heat::ajustHeat595(uint8_t numberRegimCook) {
	Button::encCount(); //Установка температуры в режиме приготовления
	int8_t differenceTemper = Fram::framRdOneAddress(Fram::TARGET_TEMP) - Control::ovenTemper; //Разность между установленной и текущей температурой////////////////////////////
//Определяем номер члена столбца(второй мерности) массива modeTable. То есть включать 1 ТЭН или 2.
	bool hiLowMode = static_cast<bool>(differenceTemper > HysteresisTemp());
	dataTransmit = modeTable[numberRegimCook][hiLowMode]; //
	if (!Heat::checkProtectionTriggers(dataTransmit)) { //Если защиты не сработали
		TransmitToTENs(dataTransmit); //Посылаем на ТЭНЫ
	}
	if ((!Heat::getSoundPre()) && (Fram::framRdOneAddress(Fram::TARGET_MODE) == 1) && ((Fram::framRdOneAddress(Fram::TARGET_TEMP) - Control::ovenTemper) <= HysteresisTemp())) {/////////////////
		Heat::setSoundPre(true);
		MelodyPlayer::play();// Включаем  "Подмосковные вечера"
	}
	SetTimer::TimeCook(Button::dirTime);
}

//----------------------------Передаем на ТЭНЫ через SPI и HC595---------------------------

void Heat::TransmitToTENs(uint8_t dataTransmit) {
	    // 1. Сравниваем свежую посылку со старым состоянием
	    // XOR даст 0, если байты одинаковые, и >0, если есть отличия
	    uint8_t difference = dataModeOld ^ dataTransmit;
	    GPIO_PinState diff = (difference != 0) ? GPIO_PIN_SET : GPIO_PIN_RESET;

	    // 2. Отправляем данные по SPI
	    LL_SPI_TransmitData8(SPI3, dataTransmit);

	    // Ждем, пока данные полностью не уйдут на ножки (SCK/MOSI)
	    while (!LL_SPI_IsActiveFlag_TXE(SPI3)) {}
	    while (LL_SPI_IsActiveFlag_BSY(SPI3)) {} // КРИТИЧЕСКИ ВАЖНО!

	    // 3. Дергаем Latch (защелкиваем данные в HC595)
	    HAL_GPIO_WritePin(Latch, diff);
	    __NOP();
	    __NOP();
	    // Небольшая задержка может потребоваться, если HC595 слишком быстрый,
	    // но на 168 МГц STM32 импульса от двух вызовов WritePin обычно достаточно.
	    HAL_GPIO_WritePin(Latch, GPIO_PIN_RESET);

	    // 4. Сохраняем текущее состояние для следующего сравнения
	    dataModeOld = dataTransmit;

	    // 5. Включаем или выключаем пламя на дисплее
	    uint8_t statusTENs = extiManager.getRealStatus();

	    // Более читаемый вариант работы с битами
	    buf_485[FIRE_DOWN] = (statusTENs & (1 << 0)) ? 1 : 0; // Пламя внизу
	    buf_485[FIRE_UP] = (statusTENs & (1 << 1)) ? 1 : 0; // Пламя вверху
	    buf_485[FIRE_SIDE] = (statusTENs & (1 << 2)) ? 1 : 0; // Пламя сбоку
}


uint8_t Heat::getDataTransmit() {
	return dataTransmit;
}	//Читаем состояние программной посылки на включение-выключение ТЭНов
//------------------------Гистерезис температур в зависимости от режима-----------

uint8_t Heat::HysteresisTemp() {
	if (Fram::framRdOneAddress(Fram::TARGET_MODE) == 1) {//////////////////////////////////////////////////
	        return tempPreOver;
	    }
	    return tempOtherOver;
}
//--------------------------------Управление внешним кулером---------------------
void Heat::setOutCooler() {//    константа        если режим не "set"
    if (Control::ovenTemper > safeTemperature && Fram::framRdOneAddress(Fram::TARGET_MODE) != 3) {///////////////////////////////////
        HAL_GPIO_WritePin(FanOut_GPIO_Port, FanOut_Pin, GPIO_PIN_SET);    // кулер ON
    } else {
        HAL_GPIO_WritePin(FanOut_GPIO_Port, FanOut_Pin, GPIO_PIN_RESET);  // кулер OFF
    }
}
//--------------------------------Подача звука при предварительном нагреве-----------
void Heat::soundMatch() {
	if (!MelodyPlayer::isPlaying()) {
		    MelodyPlayer::play();
		}

}
//-------------------------------------------------------Проверка защит--------------
bool Heat::checkProtectionTriggers(uint8_t& dataTransmit) { // Передача по ссылке!

    // 1. Считываем уставку ОДИН раз за вызов функции.
    // Это защитит от двойного чтения, но данные всё равно будут "свежими" (Just-in-Time)
    uint8_t currentTargetTemp = Fram::framRdOneAddress(Fram::TARGET_TEMP);/////////////////////////////////////////////////

    // Правило 1: Открыта дверь и температура выше безопасной
    if (doorRd != 0 && Control::ovenTemper > safeTemperature) {
        dataTransmit = 0;
        TransmitToTENs(dataTransmit);
        return true;
    }

    // Правило 2: Перегрев
    if (Control::ovenTemper > currentTargetTemp) {
        dataTransmit &= 0b1000;
        TransmitToTENs(dataTransmit);
        return true;
    }

    // Правило 4: Проверка времени при 70 < t <= 120
    if (currentTargetTemp > 70 && currentTargetTemp <= 120 && SetTimer::totalTime > maxTotalTime) {
        Button::regim1Button();
        return true;
    }

    // Правило 5: Проверка времени при t > 120
    if (currentTargetTemp > 120 && SetTimer::totalTime > minTotalTime) {
        Button::regim1Button();
        return true;
    }

    return false; // Защита не сработала
}

void Heat::setSoundPre(bool setSoundPre){
		soundPre = setSoundPre;
	}

bool Heat::getSoundPre(){
	return soundPre;
}

    void Heat::setDataModeOld(uint8_t setdataModeOld){//Сеттер для dataModeOld
    	dataModeOld = setdataModeOld;
    }
    uint8_t Heat::getDataModeOld(){//Геттер для dataModeOld
    	return dataModeOld;
    }


