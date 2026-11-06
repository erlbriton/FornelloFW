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
        uint16_t ccr;  // Скважность
        uint16_t pad;  // Выравнивание
    };

    static void init();
    static void play();
    static void stop();
    static bool isPlaying();
    static void onDmaComplete();

    template<size_t N>
    static constexpr auto compileMelody(const std::array<MelodyLibrary::Note, N>& melody) {
        struct Result {
            std::array<DmaNote, N * 4> data;
            size_t count;
        };
        Result res{};
        size_t dmaIndex = 0;

        // В init() мы установим PSC = 167. 168 МГц / 168 = 1 МГц.
        // Значит, один тик таймера = 1 микросекунда.
        constexpr uint32_t TICK_FREQ = 1000000;

        for (size_t i = 0; i < N; ++i) {
            uint16_t freq = melody[i].frequency;
            if (freq == 0) freq = 1000; // Частота для паузы

            uint32_t arr = (TICK_FREQ / freq) - 1;
            if (arr > 65535) arr = 65535;
            uint16_t ccr = (melody[i].frequency == 0) ? 0 : (arr / 2);

            // УМНОЖАЕМ ДЛИТЕЛЬНОСТЬ НА КОЭФФИЦИЕНТ ТЕМПА
            uint32_t scaledDuration = static_cast<uint32_t>(melody[i].duration * 0.5f);

            // Считаем количество периодов ШИМ для заданной длительности
            uint32_t totalPeriods = (scaledDuration * freq) / 1000;
            if (totalPeriods == 0) totalPeriods = 1;

            // Дробим ноту, если она не влезает в 8-битный RCR (макс 255)
            while (totalPeriods > 0 && dmaIndex < (N * 4)) {
                uint16_t rcr = (totalPeriods > 256) ? 255 : (totalPeriods - 1);
                res.data[dmaIndex++] = { static_cast<uint16_t>(arr), rcr, ccr, 0 };
                totalPeriods -= (rcr + 1);
            }
        }
        res.count = dmaIndex; // Запоминаем реальное количество нот
        return res;
    }

private:
    static bool playing;
};

#endif /* MELODY_GPIO_HPP_ */
