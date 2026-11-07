#include "melody_gpio.hpp"

extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_tim1_up;

// Компилируем мелодию на этапе сборки (0 ОЗУ, всё во Flash)
static constexpr auto DmaMelody = MelodyPlayer::compileMelody(MelodyLibrary::PodmoskovnyeVechera);

bool MelodyPlayer::playing = false;

void MelodyPlayer::init() {
    // 1. Предделитель 168 МГц / 168 = 1 МГц (1 тик = 1 мкс)
    __HAL_TIM_SET_PRESCALER(&htim1, 167);

    // 2. Настройка Burst-режима DMA (ARR + RCR и CCR1)
    TIM1->DCR = (0x0B << TIM_DCR_DBA_Pos) | (1 << TIM_DCR_DBL_Pos);

    // Выключаем звук при инициализации
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
}

void MelodyPlayer::play() {
    if (playing) return;
    playing = true;

    // Сброс счетчика
    __HAL_TIM_SET_COUNTER(&htim1, 0);

    // Загружаем первую ноту вручную (для мгновенного старта)
    __HAL_TIM_SET_AUTORELOAD(&htim1, DmaMelody.data[0].arr);
    htim1.Instance->RCR = DmaMelody.data[0].rcr;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, DmaMelody.data[0].ccr);

    // Запускаем DMA со ВТОРОЙ ноты (data[1])
    HAL_DMA_Start_IT(&hdma_tim1_up,
                     (uint32_t)&DmaMelody.data[1],
                     (uint32_t)&TIM1->DMAR,
                     (DmaMelody.count - 1) * 2);

    // Разрешаем таймеру дергать DMA по событию Update
    __HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_UPDATE);

    // Включаем ШИМ
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

void MelodyPlayer::stop() {
    // Отключаем запросы DMA
    __HAL_TIM_DISABLE_DMA(&htim1, TIM_DMA_UPDATE);

    // Выключаем ШИМ
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

    // 🎯 ПОЛНОСТЬЮ ОСТАНАВЛИВАЕМ СЧЕТЧИК ТАЙМЕРА
    HAL_TIM_Base_Stop(&htim1);

    // Останавливаем DMA (используем обычный Abort, так как мы можем быть внутри прерывания)
    HAL_DMA_Abort(&hdma_tim1_up);

    playing = false;
}

bool MelodyPlayer::isPlaying() {
    return playing;
}

void MelodyPlayer::onDmaComplete() {
    stop();

    // --- ЗДЕСЬ ВАШ СТАРЫЙ КОД ЗАВЕРШЕНИЯ ---
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
    if (hdma->Instance == DMA2_Stream5) {
        MelodyPlayer::onDmaComplete();
    }
}
