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

    // "Подмосковные вечера" — точные частоты
    // D4=294, F4=349, A4=440, G4=392, E4=330
    constexpr std::array<Note, 23> PodmoskovnyeVechera = {{
    		// "Не слыш-ны в са-ду да-же шо-ро-хи"
    		        {294, 500}, // Ре (долгая)
    		        {349, 250}, // Фа (короткая)
    		        {440, 500}, // Ля (долгая)
    		        {349, 250}, // Фа (короткая)
    		        {392, 500}, // Соль (долгая)
    		        {349, 250}, // Фа (короткая)
    		        {330, 500}, // Ми (долгая)
    		        {440, 500}, // Ля (долгая)
    		        {440, 250}, // Ля (короткая)
    		        {392, 250}, // Соль (короткая)
    		        {294, 1000}, // Ре (половинная, финальная)
    }};

} // namespace MelodyLibrary

#endif /* MELODY_LIBRARY_HPP_ */
