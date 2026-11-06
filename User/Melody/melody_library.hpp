#ifndef MELODY_LIBRARY_HPP_
#define MELODY_LIBRARY_HPP_

#include <array>
#include <cstdint>

namespace MelodyLibrary {

    constexpr uint16_t R = 0; // Пауза

    struct Note {
        uint16_t frequency;
        uint16_t duration;
    };

    // "Подмосковные вечера" (28 нот)
    constexpr std::array<Note, 28> PodmoskovnyeVechera = {{
        {394, 600}, {440, 600}, {494, 600}, {523, 600}, {587, 600}, {587, 600}, {523, 600}, {494, 600},
        {659, 900}, {659, 900}, {587, 900}, {587, 900},
        {440, 1200}, {440, 1200}, {440, 600}, {R, 300},
        {523, 600}, {659, 600}, {784, 600}, {784, 600},
        {880, 900}, {880, 900}, {784, 600}, {698, 600},
        {659, 1200}, {659, 600}, {659, 300}, {R, 300}
    }};

} // namespace MelodyLibrary

#endif /* MELODY_LIBRARY_HPP_ */
