#ifndef MELODY_GPIO_HPP
#define MELODY_GPIO_HPP

#include "main.h"
#include "tim.h"

// Структура для хранения параметров ноты
struct Note {
    uint16_t frequency; // Частота в Гц (0 для паузы)
    uint16_t duration;  // Длительность в мс
};

class MelodyPlayer {
private:
    static uint32_t noteTimeout;     // Время окончания текущей ноты
    static uint16_t currentNoteIdx;  // Индекс воспроизводимой ноты

public:
    static void setFrequency(uint16_t frequency);
    static void processNextNoteAsync();
    static void playPodmoskovnye();
};

// Объявление массива нот (сама мелодия будет в .cpp)
extern const Note podmoskovnye[];

#ifdef __cplusplus
extern "C" {
#endif
    void TIM7_Music_Handler(void);
#ifdef __cplusplus
}
#endif

#endif
