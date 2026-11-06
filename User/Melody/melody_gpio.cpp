#include "melody_gpio.hpp"

extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_tim1_up;

// Компилируем мелодию на этапе сборки
static constexpr auto DmaMelody = MelodyPlayer::compileMelody(MelodyLibrary::PodmoskovnyeVechera);

bool MelodyPlayer::playing = false;

void MelodyPlayer::init() {
    // 1. Устанавливаем предделитель: 168 МГц / 168 = 1 МГц (1 тик = 1 мкс)
    __HAL_TIM_SET_PRESCALER(&htim1, 167);

    // 2. Настройка Burst-режима DMA
    // DBA = 0x0B (смещение регистра ARR)
    // DBL = 1 (2 трансфера по 32 бита: ARR+RCR и CCR1+CCR2)
    TIM1->DCR = (0x0B << TIM_DCR_DBA_Pos) | (1 << TIM_DCR_DBL_Pos);

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0); // Тишина при старте
}

void MelodyPlayer::play() {
    if (playing) return;
    playing = true;

    __HAL_TIM_SET_COUNTER(&htim1, 0);

    // 1. Вручную загружаем ПЕРВУЮ ноту, чтобы звук пошел мгновенно
    __HAL_TIM_SET_AUTORELOAD(&htim1, DmaMelody.data[0].arr);
    htim1.Instance->RCR = DmaMelody.data[0].rcr; // RCR пишем напрямую
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, DmaMelody.data[0].ccr);

    // 2. Запускаем DMA, начиная СО ВТОРОЙ ноты (data[1])
    // Передаем (количество нот - 1) * 2 слов
    HAL_DMA_Start_IT(&hdma_tim1_up,
                     (uint32_t)&DmaMelody.data[1], // Адрес второй ноты
                     (uint32_t)&TIM1->DMAR,
                     (DmaMelody.count - 1) * 2);   // Количество оставшихся слов

    // 3. Разрешаем таймеру дергать DMA
    __HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_UPDATE);

    // 4. Включаем ШИМ
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

void MelodyPlayer::stop() {
    __HAL_TIM_DISABLE_DMA(&htim1, TIM_DMA_UPDATE);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_DMA_Abort_IT(&hdma_tim1_up);
    playing = false;
}

bool MelodyPlayer::isPlaying() {
    return playing;
}

void MelodyPlayer::onDmaComplete() {
    stop();

    // --- ЗДЕСЬ ВАШ КОД ИЗ СТАРОГО ФАЙЛА (таймеры 5/6, USART, HC595) ---
    /*
    htim5.Instance->SR &= ~0x0001;
    htim6.Instance->SR &= ~0x0001;
    HAL_TIM_Base_Start_IT(&htim5);
    HAL_TIM_Base_Start_IT(&htim6);

    USART1->CR1 |= USART_CR1_UE;
    USART1->SR &= ~0x003F;
    (void)USART1->DR;
    GPIOC->BSRR = GPIO_PIN_7 << 16U;
    */
}

extern "C" void HAL_DMA_XferCpltCallback(DMA_HandleTypeDef *hdma) {
    // Проверьте в CubeMX, что для TIM1_UP выделен именно DMA2 Stream 5
    if (hdma->Instance == DMA2_Stream5) {
        MelodyPlayer::onDmaComplete();
    }
}
