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

#define safeTemperature 90

//------------------------------Главный метод--------------------------------------------------
void
Heat::ajustHeat595(vu8 numberRegimCook) {
	Button::encCount(); //Установка температуры в режиме приготовления
	vu8 differenceTemper = Fram::framRD0byte() - Control::ovenTemper; //Разность между установленной и текущей температурой
//Определяем номер члена столбца(второй мерности) массива modeTable. То есть включать 1 ТЭН или 2.
	bool hiLowMode = static_cast<bool>(differenceTemper > HysteresisTemp());
	dataTransmit = modeTable[numberRegimCook][hiLowMode]; //
	if (!Heat::checkProtectionTriggers(dataTransmit)) { //Если защиты не сработали
		TransmitToTENs(dataTransmit); //Посылаем на ТЭНЫ
	}
	if ((Heat::soundPre == false) && (Fram::elementFram(1) == 1) && ((Fram::framRD0byte() - Control::ovenTemper) <= HysteresisTemp())) {
		Heat::soundPre = true;
		//checkAndPlaySound();
	}
	SetTimer::TimeCook(Button::dirTime);
}

//----------------------------Передаем на ТЭНЫ через SPI и HC595---------------------------
void
Heat::TransmitToTENs(vu8 dataTransmit) {
//Сравниваем свежую посылку(dataMode[0]) с уже включенными выходами HC595,
//если равны - блокируем вывод Latch HC595. Таким образом включенные ТЭНы остаются без изменения
//	uint8_t difference = dataModeOld - dataTransmit; //Разность между набором включенных выходов и новой  посылкой
	GPIO_PinState diff = (GPIO_PinState) 1; //difference; //true - разность есть, false - разности нет
	LL_SPI_TransmitData8(SPI3, dataTransmit);	//Передаем на тэны через HC595
	while (!LL_SPI_IsActiveFlag_TXE(SPI3)) {
	}	//Ждем освобождения буфера передачи
	HAL_GPIO_WritePin(Latch, diff);	//Latch. Если diff = true - передаем посылку, если diff = false - посылка не передаестся
	HAL_GPIO_WritePin(Latch, GPIO_PIN_RESET);	//Latch
	//------ Включаем или выключаем пламя на дисплее в зависимости от включенных тенов ----------------------------
	vu8 statusTENs = extiManager.getRealStatus();
	 if(status == 7){//Если все 3 ТЭНа включены
	    buf_485[19] = 24;//Код ошибки
	    Button::regim1Button();
	 }
	buf_485[13] =    statusTENs & (1 << 0); //Пламя внизу
	buf_485[15] = !!(statusTENs & (1 << 1)); //Пламя вверху
	buf_485[14] = !!(statusTENs & (1 << 2)); //Пламя сбоку
	//HAL_UART_Transmit_IT(&huart3, buf_485, 20);//Передаем на дисплей
//	uint8_t rIn = rightIn << 2;          //Читаем...
//	uint8_t gIn = grillIn << 1;          //...включенные...
//	uint8_t dIn = downIn  << 0;           //...выходы HC595
//  uint8_t fIn = dataTransmit &= 0b1000;//Обнуляем посылку в HC595
//  dataModeOld = rIn + gIn + dIn + fIn;//Старая посылка
}

vu8
Heat::getDataTransmit() {
	return dataTransmit;
}	//Читаем состояние программной посылки на включение-выключение ТЭНов
//------------------------Гистерезис температур в зависимости от режима---------------------------------------------------
vu8
Heat::HysteresisTemp() {
	return Fram::elementFram(1) == 1 ? tempPreOver : tempOtherOver;
}
//--------------------------------Управление внешним кулером---------------------
void
Heat::setOutCooler() {
	GPIOA->BSRR = (Control::ovenTemper > safeTemperature && Fram::elementFram(1) != 3) ?
	GPIO_PIN_11               // включить кулер
			:
			GPIO_PIN_11 << 16U;       // выключить кулер
}
//--------------------------------Подача звука при предварительном нагреве-----------
//void Heat::soundMatch() {
//	TIM7->ARR = 12;        // Немного изменяю на ходу
//	TIM7->PSC = 1680;      //частоту звука
//	TIM7->EGR = TIM_EGR_UG;//просто для тренировки
//	//HAL_ADC_Stop (&hadc2);/*Если не остановить ADC режима в момент подачи сигнала конца предварительного нагрева то
//	// при случайном переключении режима звук какое-то время ведет себя неадекватно*/
//	HAL_TIM_Base_Start_IT(&htim7);
//	HAL_TIM_Base_Start_IT(&htim9);
//}
//-------------------------------------------------------Проверка защит----------------------------------------------------
struct ProtectionTrigger {
	std::function<bool()> condition; //Условие, которое проверяется (возвращает true или false)
	std::function<bool()> action;       //Что делать, если условие выполнено
};
bool Heat::checkProtectionTriggers(vu8 dataTransmit) {
// @formatter:off
	const ProtectionTrigger triggers[] = {// 1. Открыта дверь и температура выше безопасной
		{ []() { return doorRd != 0 && Control::ovenTemper > safeTemperature; },
		[&]() { dataTransmit = 0;
		TransmitToTENs(dataTransmit); return true; } },

		// 2. Перегрев
		{ []() { return Control::ovenTemper > Fram::framRD0byte(); },// Перегрев
		[&]() { dataTransmit &= 0b1000; TransmitToTENs(dataTransmit); return true; } },

		// 3. Проверка всех включенных тэнов
		{ []() { return false && false && false; },
		[]()  { Button::regim1Button(); GPIOA->BSRR |= GPIO_PIN_8 << 16U; return true; } },

		// 4. Проверка прошедшего времени при 70 < t <= 120 град (12 часов)
		{ []() { return (Fram::framRD0byte() > 70 && Fram::framRD0byte() <= 120) && SetTimer::totalTime > maxTotalTime; },
		[]() { Button::regim1Button(); return true; } },

		// 5. Проверка прошедшего времени при t > 120 град (3 часа)
		{ []() { return Fram::framRD0byte() > 120 && SetTimer::totalTime > minTotalTime; },
		[]()  { Button::regim1Button(); return true; } }
	};
// @formatter:on

	for (const auto &trigger : triggers) { // Проходимся по массиву циклом
		if (trigger.condition() && trigger.action()) {
			return true;
		}
	}
	return false;
}
//------------------------------Подаем сигнал предварительного нагрева-------------------------------
void
Heat::checkAndPlaySound() {
	// 4. Если все условия сошлись, запускаем мелодию
	GPIOC->BSRR = GPIO_PIN_7;	//HC595 выкл
	HAL_TIM_Base_Stop_IT(&htim5);
	HAL_TIM_Base_Stop_IT(&htim6);
	HAL_TIM_Base_Stop(&htim4);
	USART1->CR1 &= ~USART_CR1_UE; // Выключить USART1
	USART2->CR1 &= ~USART_CR1_UE; // Выключить USART2
	USART3->CR1 &= ~USART_CR1_UE; // Выключить USART3
	//             MelodyPlayer::playPodmoskovnye();
	// 5. Устанавливаем флаг, чтобы исключить повторный запуск
	// Heat::soundPre = true;
//                HAL_TIM_Base_Start(&htim3);
//                HAL_TIM_Base_Start_IT(&htim6);
//                HAL_TIM_Base_Start(&htim10);
//                GPIOC->BSRR = GPIO_PIN_7 << 16U;//HC595 вкл
}
