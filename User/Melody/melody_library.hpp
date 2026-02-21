//
#ifndef MELODY_MELODY_LIBRARY_HPP_
#define MELODY_MELODY_LIBRARY_HPP_

#include <stdint.h>
#include <stddef.h>

namespace MelodyLibrary {

    const uint16_t R = 0; // Пауза

    // Подмосковные вечера — первые две строчки мелодии
    const uint16_t PodmoskovnyeVechera[ ] = {
    		        394, 440, 494, 523, 587, 587, 523, 494,  // 1-8
    		        659, 659, 587, 587,                       // 9-12
    		        440, 440, 440, R,                         // 13-16
    		        523, 659, 784, 784,                       // 17-20
    		        880, 880, 784, 698,                       // 21-24
    		        659, 659, 659, R,                         // 25-28
    		    };

    		    // Соответствующие первые 30 длительностей
    		    const uint16_t PodmoskovnyeVecheraDurations[ ] = {
    		        600, 600, 600, 600, 600, 600, 600, 600,  // 1-8
    		        900, 900, 900, 900,                       // 9-12
    		        1200, 1200, 600, 300,                    // 13-16
    		        600, 600, 600, 600,                       // 17-20
    		        900, 900, 600, 600,                       // 21-24
    		        1200, 600, 300, 300,                      // 25-28
    };

    const size_t PodmoskovnyeVecheraLength = sizeof(PodmoskovnyeVechera) / sizeof(PodmoskovnyeVechera[0]);

}

#endif /* MELODY_MELODY_LIBRARY_HPP_ */
