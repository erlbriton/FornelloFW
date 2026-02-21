/*
 * Control.hpp
 *
 *  Created on: Jan 16, 2024
 *      Author: erlbriton
 */

#ifndef CONTROL_CONTROL_HPP_
#define CONTROL_CONTROL_HPP_
#include "flashdata.hpp"
#include "adc.h"
#include "main.h"
#include "FryMode.hpp"
#include <array>
#include <boost/preprocessor/repetition/repeat.hpp>//Здесь использована сторонняя библиотека  "boost" для короткой записи массива

#define REPEAT(z, n, value) value,//Дефайн для короткой записи массива

class Control {
private:
	static inline const uint16_t VREF = 1500;//Постоянный коффициент референса
	static inline uint32_t tempADC = 0;//Сырые данные АЦП температуры в шкафу
	static inline uint32_t tVref = 0;//Сырые данные АЦП референса
	static inline float kVref = 0;//Поправочный коффициент референса
	static inline uint32_t adcRaw = 0; //Сырые данные ADC задатчика режима

public:
	inline static vu16 ovenTemper = 0;
	inline static vu32 adcTemp[2] = {0, };
	static vu32 bakeTemper();
	static vu16 readAdc(vu8); //Читаем задатчик режима
	static void backInfo(void);
	bool isEncDone() const;//Геттер EncDone
	void isEncDone(bool newEncDone);//Сеттер EncDone
private:
	static constexpr std::array<vu8, 4096> rawADC = {
			    BOOST_PP_REPEAT(239, REPEAT, 1)
			    BOOST_PP_REPEAT(250, REPEAT, 2)
				BOOST_PP_REPEAT(250, REPEAT, 2)
				BOOST_PP_REPEAT(56,  REPEAT, 2)
			    BOOST_PP_REPEAT(250, REPEAT, 3)
				BOOST_PP_REPEAT(250, REPEAT, 3)
				BOOST_PP_REPEAT(99,  REPEAT, 3)
				BOOST_PP_REPEAT(250, REPEAT, 4)
				BOOST_PP_REPEAT(250, REPEAT, 4)
				BOOST_PP_REPEAT(11,  REPEAT, 4)
				BOOST_PP_REPEAT(250, REPEAT, 5)
				BOOST_PP_REPEAT(200, REPEAT, 5)
				BOOST_PP_REPEAT(250, REPEAT, 6)
				BOOST_PP_REPEAT(118, REPEAT, 6)
				BOOST_PP_REPEAT(250, REPEAT, 7)
				BOOST_PP_REPEAT(68,  REPEAT, 7)
				BOOST_PP_REPEAT(250, REPEAT, 8)
				BOOST_PP_REPEAT(69,  REPEAT, 8)
				BOOST_PP_REPEAT(250, REPEAT, 9)
				BOOST_PP_REPEAT(18,  REPEAT, 9)
				BOOST_PP_REPEAT(250, REPEAT, 10)
				BOOST_PP_REPEAT(43,  REPEAT, 10)
				BOOST_PP_REPEAT(175, REPEAT, 0)

	};
};
#endif /* CONTROL_CONTROL_HPP_ */
