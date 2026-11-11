/*
 * CallBacks.cpp
 *
 *  Created on: Mar 31, 2025
 *      Author: erlbriton
 */

#include "CallBacks.hpp"
#include "Control.hpp"
#include "Heat.hpp"
#include "SetTimer.hpp"
#include "melody_gpio.hpp"

constexpr uint8_t RELAY_DOWN_BIT  = 0;
constexpr uint8_t RELAY_GRL_BIT   = 1;
constexpr uint8_t RELAY_RIGHT_BIT = 2;

//---------------------ADCManager-----------------------------
void ADCManager::handleADCConversionComplete(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
       // Control control;
        //#################################Для калибровки###################################################
//        buf_485[4] = adcTemp[0] / 1000;
//        buf_485[3] = adcTemp[0] / 100 % 10;
//        buf_485[2] = adcTemp[0] / 10 % 10;
//        buf_485[1] = adcTemp[0] % 10;
//        HAL_UART_Transmit_IT(&huart3, buf_485, 20);
//        HAL_Delay(1000);
        //#################################Конец калибровки###################################################
        Control::bakeTemper();
    }
}
//---------------------TimerManager-----------------------------

// Массив структур, которые связывают таймеры и их обработчики
const TimerManager::TimerMap TimerManager::timerMap[ ] = {
	{ TIM2,  &TimerManager::handleTIM2  },
	{ TIM4,  &TimerManager::handleTIM4  },
    { TIM6,  &TimerManager::handleTIM6  },
};

// Обработчик прерывания таймера
void TimerManager::handleTimerInterrupt(TIM_HandleTypeDef* htim) {
    TimerHandler handler = nullptr;
    // Поиск соответствующего обработчика для таймера
    for (const auto& entry : timerMap) {
        if (entry.instance == htim->Instance) {
            handler = entry.handler;
            break;
        }
    }
    if (handler) {
        (this->*handler)(); // Вызываем обработчик
    }
}
// Реализация обработчиков
//---------------------------------------------------------------------
void TimerManager::handleTIM2() {
	Button::isEncDone(true);
}

void TimerManager::handleTIM4() {
    // Используем явное присваивание для исключения предупреждений volatile
    SetTimer::totalTime = SetTimer::totalTime + 1; // Общее время нагрева
    SetTimer::secondCounter = SetTimer::secondCounter + 1;

    // Классическое условие вместо логического &&
    if (SetTimer::secondCounter == 60) {
        SetTimer::minCounterInc = SetTimer::minCounterInc + 1;
        SetTimer::minCounterDec = SetTimer::minCounterDec - 1;
        SetTimer::secondCounter = 0;
    }
}

void TimerManager::handleTIM6() {
    if (Control::ovenTemper >= Heat::tempMax) {
        Button::regim1Button();
    }
}
//void TimerManager::handleTIM5() {
//	//Protection::TIM5_Handler();
//}

/*Логика защиты ТЭНов:
 * 1. Принцип мониторинга тока (Слой EXTI)
Система отслеживает реальное протекание тока через три трансформатора тока (ТТ), подключенных к входам EXTI микроконтроллера.
Детекция: Каждый импульс тока (частота сети 50 Гц) вызывает прерывание, которое обновляет метку времени lastPulse[i] значением системного таймера HAL_GetTick().
Критерий "Ток есть": Если разница между текущим временем и последней меткой импульса менее 100 мс, система считает, что ТЭН в данный момент находится под нагрузкой.
* 2. Программный фильтр (Метод checkHeaters)
Для предотвращения ложных срабатываний от сетевых помех или переходных процессов используется 5-секундная задержка.
Алгоритм накопления: При обнаружении рассогласования (команды нет, а ток есть, или наоборот) система начинает отсчет времени. Если аномалия исчезает хотя бы на мгновение, таймер сбрасывается в ноль.
Принятие решения: Только непрерывная аномалия в течение 5000 мс приводит к генерации кода ошибки.
* 3. Классификация инцидентов
ОБРЫВ (Коды 11, 12, 13): Команда на включение реле подана, но ток через ТТ не зафиксирован.
Действие: Вывод предупреждения на дисплей о неисправности конкретного ТЭНа.
ЗАЛИПНИЕ / ПРОБОЙ КЛЮЧА (Коды 21, 22, 23): Программная команда на выключение реле активна, но ток продолжает течь через ТЭН. Это критическая ситуация (неконтролируемый нагрев).
* 4. Аппаратная блокировка (Силовой барьер)
В случае подтвержденного залипания (ошибка 21–23), система переходит в режим аварийного стопа:
Срабатывание расцепителя: Процессор немедленно подает сигнал на вспомогательное реле, которое активирует независимый расцепитель вводного автомата.
Обесточивание: Автомат физически разрывает цепь питания всей духовки.
Блокировка: Поскольку автомат спрятан за корпусом духовки в шкафу, повторное включение невозможно без извлечения устройства и вмешательства специалиста.
Индикация: На дисплее отображается сообщение о критической аварии и необходимости вызова мастера.
 */
//-------------------------Проверка ТЭНов на обрыв и залипание контактов реле или пробой ключа-----------------
void EXTIManager::handleEXTIInterrupt(uint16_t GPIO_Pin){
    switch (GPIO_Pin) {
        case EXTI_Right_Pin:
            handleRight();
            break;
        case EXTI_Down_Pin:
            handleDown();
            break;
        case EXTI_Grl_Pin:
            handleGrl();
            break;
        default:
            break;
    }
}
void EXTIManager::handleDown()  { lastPulse[0] = HAL_GetTick(); }
void EXTIManager::handleGrl()   { lastPulse[1] = HAL_GetTick(); }
void EXTIManager::handleRight() { lastPulse[2] = HAL_GetTick(); }

//-----Главный метод определения критических ошибок и предупреждений---------

uint8_t EXTIManager::checkHeaters(uint8_t buttonMode) {
    uint32_t now = HAL_GetTick();
    const uint32_t timeout = 5000;      // 5 секунд
    const uint32_t pulseValid = 100;    // 100 мс

    // 1. Подготовка данных о командах
    bool cmdOn[3] = {false, false, false};

    // Если режим 3 — ТЭНы работают согласно программе
    if(buttonMode == 2) {
        uint8_t relayByte = Heat::getDataTransmit();
        cmdOn[0] = (relayByte & (1 << RELAY_DOWN_BIT))  != 0;
        cmdOn[1] = (relayByte & (1 << RELAY_GRL_BIT))   != 0;
        cmdOn[2] = (relayByte & (1 << RELAY_RIGHT_BIT)) != 0;
    }
    // Если режим 0 или 1 — команды ВСЕГДА false (ток запрещен)
    // 2. Единый цикл проверки
    for (uint8_t i = 0; i < 3; i++) {
        bool hasCurrent = (now - lastPulse[i]) < pulseValid;
        // --- ПРОВЕРКА ЗАЛИПАНИЯ (Ток есть, когда его не должно быть) ---
        // Сработает и в режиме 3 (если ТЭН выключен программой),
        // и в режимах 0/1 (так как cmdOn там всегда false).
        if(!cmdOn[i] && hasCurrent){
            if (stuckTimer[i] == 0) stuckTimer[i] = now;
            if ((now - stuckTimer[i]) > timeout) return (i + 4);
        }else {
            stuckTimer[i] = 0;
        }
        // --- ПРОВЕРКА ОБРЫВА (Тока нет, хотя команда на включение есть) ---
        // В режимах 0/1 cmdOn = false, поэтому эта проверка никогда не сработает.
        // Она активна только в режиме 3 для тех ТЭНов, что включены.
        if(cmdOn[i] && !hasCurrent){
            if (openTimer[i] == 0) openTimer[i] = now;
            if ((now - openTimer[i]) > timeout) return (i + 1);
        } else {
            openTimer[i] = 0;
        }
    }

    return 0;
}
//Статус ТЭНов(реально включен или выключен) 001 - включен Down, 010 - включен Grill, 100 - включен Right
uint8_t EXTIManager::getRealStatus() {
    uint32_t now = HAL_GetTick();
    uint8_t status = 0;
    for (int i = 0; i < 3; i++) {
    if ((now - lastPulse[i]) < 100) { // 100мс - окно валидности
        status |= (1 << i); // Устанавливаем бит, если ток есть
        }
    }
    return status;
}
//---------------------Колбеки-----------------------------
ADCManager adcManager;
TimerManager timerManager;
extern EXTIManager extiManager;


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    adcManager.handleADCConversionComplete(hadc);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    timerManager.handleTimerInterrupt(htim);
}
//-----------------------------------------------------------------
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        Button::isEncDone(true);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    extiManager.handleEXTIInterrupt(GPIO_Pin);
}

