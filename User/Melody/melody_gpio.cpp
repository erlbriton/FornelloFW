#include "melody_gpio.hpp"

extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_tim1_up;

// Компилируем массив (он уже содержит 3 повтора и паузы)
static constexpr auto DmaMelody = MelodyPlayer::compileMelody(MelodyLibrary::PodmoskovnyeVechera);

bool MelodyPlayer::playing = false;

void MelodyPlayer::init() {
    __HAL_TIM_SET_PRESCALER(&htim1, 167);

    // DBL = 1 (2 трансфера по 32 бита)
    TIM1->DCR = (0x0B << TIM_DCR_DBA_Pos) | (1 << TIM_DCR_DBL_Pos);

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
}

void MelodyPlayer::play() {
    if (playing) return;
    playing = true;

    __HAL_TIM_SET_COUNTER(&htim1, 0);

    // Загружаем первую ноту вручную
    __HAL_TIM_SET_AUTORELOAD(&htim1, DmaMelody.data[0].arr);
    htim1.Instance->RCR = DmaMelody.data[0].rcr;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, DmaMelody.data[0].ccr);

    // Запускаем DMA со второй ноты (умножаем на 2, так как структура 16-битная)
    HAL_DMA_Start_IT(&hdma_tim1_up,
                     (uint32_t)&DmaMelody.data[1],
                     (uint32_t)&TIM1->DMAR,
                     (DmaMelody.count - 1) * 2);

    __HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_UPDATE);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

void MelodyPlayer::stop() {
    __HAL_TIM_DISABLE_DMA(&htim1, TIM_DMA_UPDATE);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_Base_Stop(&htim1);
    playing = false;
}

bool MelodyPlayer::isPlaying() {
    return playing;
}

void MelodyPlayer::onDmaComplete() {
    stop();
}

extern "C" void HAL_DMA_XferCpltCallback(DMA_HandleTypeDef *hdma) {
    if (hdma->Instance == DMA2_Stream5) {
        MelodyPlayer::onDmaComplete();
    }
}
