//#include "melody_gpio.hpp"
//#include "main.h"
//
//extern TIM_HandleTypeDef htim12;
//extern TIM_HandleTypeDef htim7;
//
//uint32_t MelodyPlayer::noteTimeout = 0;
//uint16_t MelodyPlayer::currentNoteIdx = 0;
//static bool isPlaying = false;
//
//// Оставляем ваши ноты
//#define NOTE_RE   294
//#define NOTE_MI   330
//#define NOTE_FA   349
//#define NOTE_SOL  392
//#define NOTE_LA   440
//#define NOTE_CS   277 // До-диез (C#)
//
//void MelodyPlayer::setFrequency(uint16_t frequency) {
//    if (frequency == 0) {
//        __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 0);
//        HAL_TIM_PWM_Stop(&htim12, TIM_CHANNEL_2);
//        return;
//    }
//
//    // УМЕНЬШАЕМ базу, чтобы ПОВЫСИТЬ тон.
//    // Попробуем 200 000 (в 10 раз меньше, чем когда звук был просто "низким")
//    uint32_t timer_base_freq = 2000000;
//
//    // Если 2 000 000 был низким, а 10 000 000 стал ЕЩЕ ниже,
//    // попробуем пойти вниз: 500 000
//    uint32_t target_base = 500000;
//
//    uint32_t period = target_base / frequency;
//
//    if (period < 2) period = 2;
//    if (period > 65535) period = 65535;
//
//    __HAL_TIM_SET_AUTORELOAD(&htim12, (uint32_t)(period - 1));
//    __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, (uint32_t)(period / 2));
//
//    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2);
//}
//
//void MelodyPlayer::playPodmoskovnye() {
//	if (isPlaying) return; // Если уже играет, не перезапускаем
//
//	    currentNoteIdx = 0;
//	    noteTimeout = 0;
//	    isPlaying = true;
//
//	    // Включаем таймер и его прерывания
//	    HAL_TIM_Base_Start_IT(&htim7);
//}
//
//void MelodyPlayer::processNextNoteAsync() {
//    // 2. Ждем окончания длительности текущей ноты
//    if (HAL_GetTick() < noteTimeout) return;
//
//    static const uint16_t tempo = 400;
//
//    static const Note song[] = {
//        {NOTE_RE,  tempo}, {NOTE_FA,  tempo}, {NOTE_LA,  tempo}, {NOTE_FA,  tempo},
//        {NOTE_SOL, tempo}, {NOTE_SOL, tempo}, {NOTE_FA,  tempo}, {NOTE_MI,  tempo},
//        {NOTE_LA,  tempo}, {NOTE_LA,  tempo}, {NOTE_SOL, tempo}, {NOTE_SOL, tempo},
//        {NOTE_RE,  tempo}, {NOTE_RE,  tempo}, {NOTE_RE,  tempo}, {NOTE_RE,  1}
//    };
//
//    // 3. Проверяем, не закончились ли ноты (16 нот в массиве)
//    if (currentNoteIdx >= 16) {
//        setFrequency(0);      // Выключаем звук
//        isPlaying = false;    // Сбрасываем флаг состояния
//
//        // ОСТАНАВЛИВАЕМ прерывания таймера TIM7
//        HAL_TIM_Base_Stop_IT(&htim7);
//
//        htim5.Instance->SR &= ~0x0001;
//        htim6.Instance->SR &= ~0x0001;
//        htim4.Instance->SR &= ~0x0001;
//
//        // 2. Старт таймеров
//        HAL_TIM_Base_Start_IT(&htim5);
//        HAL_TIM_Base_Start_IT(&htim6);
//        HAL_TIM_Base_Start(&htim4);
//
//        // 3. Включение USART
//        USART1->CR1 |= USART_CR1_UE;
//        USART2->CR1 |= USART_CR1_UE;
//        USART3->CR1 |= USART_CR1_UE;
//
//        // 4. Очистка USART (добавь это сейчас)
//        USART1->SR &= ~0x003F; // Сброс всех флагов ошибок (PE, FE, NE, ORE, IDLE)
//        USART2->SR &= ~0x003F;
//        USART3->SR &= ~0x003F;
//        (void)USART1->DR;       // Чтение данных для сброса флага RXNE
//        (void)USART2->DR;
//        (void)USART3->DR;
//        GPIOC->BSRR = GPIO_PIN_7 << 16U; // HC595 вкл
//
//        return;
//    }
//
//    // 4. Играем текущую ноту
//    setFrequency(song[currentNoteIdx].frequency);
//
//    // 5. Устанавливаем время, когда нужно будет переключиться на следующую ноту
//    noteTimeout = HAL_GetTick() + song[currentNoteIdx].duration;
//
//    // 6. Переходим к следующему индексу
//    currentNoteIdx++;
//}
//
//extern "C" void TIM7_Music_Handler(void) {
//    MelodyPlayer::processNextNoteAsync();
//}
