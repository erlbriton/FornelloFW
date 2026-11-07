#ifndef MELODY_GPIO_HPP_
#define MELODY_GPIO_HPP_

#include <cstdint>
#include <array>
#include "main.h"
#include "tim.h"
#include "melody_library.hpp"

class MelodyPlayer {
public:
    struct DmaNote {
        uint16_t arr;  // Частота
        uint16_t rcr;  // Длительность
        uint16_t ccr;  // Скважность (Тембр)
        uint16_t pad;  // Выравнивание
    };

    static void init();
    static void play();
    static void stop();
    static bool isPlaying();
    static void onDmaComplete();

    template<size_t N>///////////////////////////////////////////////////////////////////////////////////////
    static constexpr auto compileMelody(const std::array<MelodyLibrary::Note, N>& melody) {
        struct Result {
            std::array<DmaNote, N * 4 + 4> data; // +4 ячейки про запас
            size_t count;
        };
        Result res{};
        size_t dmaIndex = 0;

        constexpr uint32_t TICK_FREQ = 1000000;
        constexpr float TEMPO_SCALE = 1.0f;
        constexpr float ARTICULATION = 0.7f;
        constexpr float DUTY_CYCLE = 0.3f;

        for (size_t i = 0; i < N; ++i) {
            uint16_t freq = melody[i].frequency;
            if (freq == 0) freq = 1000;

            uint32_t arr = (TICK_FREQ / freq) - 1;
            if (arr > 65535) arr = 65535;
            uint16_t ccr = (melody[i].frequency == 0) ? 0 : static_cast<uint16_t>(arr * DUTY_CYCLE);

            uint32_t scaledDuration = static_cast<uint32_t>(melody[i].duration * TEMPO_SCALE * ARTICULATION);
            uint32_t totalPeriods = (scaledDuration * freq) / 1000;
            if (totalPeriods == 0) totalPeriods = 1;

            while (totalPeriods > 0 && dmaIndex < (N * 4)) {
                uint16_t rcr = (totalPeriods > 256) ? 255 : (totalPeriods - 1);
                res.data[dmaIndex++] = { static_cast<uint16_t>(arr), rcr, ccr, 0 };
                totalPeriods -= (rcr + 1);
            }
        }

        // 🎯 ДОБАВЛЯЕМ ФИНАЛЬНУЮ НОТУ ТИШИНЫ (защита от гудения)
        // 1000 Гц (ARR=999), RCR=255 (длинная), CCR=0 (тишина)
        res.data[dmaIndex++] = { 999, 9, 0, 0 };

        res.count = dmaIndex;
        return res;
    }

private:
    static bool playing;
};

#endif /* MELODY_GPIO_HPP_ */
