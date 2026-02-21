/*
 * Fram.hpp
 *
 *  Created on: Dec 30, 2023
 *      Author: erlbriton
 *
/ * @Как с этим работать:
  * - Сначала пины портов микроконтроллера, которые являются управляющими
  * сигналами FRAM. Далее в init.c конфигурируются порты микроконтроллера.
  * - В файле fram.h редактируются макросы для сигналов управления для данной
  * аппаратной реализации (если необходимо).
  * - В функии инициализации устройства вызывается функция check_fram(), которая
  * проверяет работу ключа параметров и целостность данных, на нем записаных.
  * - Если вызвана функция записи во собственную flash память устройства,
  * вызвать функцию fram_wr_massive для записи в основной и резервный сектора
  * ключа параметров измененные данные из flash памяти (чтобы на ключе
  * параметров всегда были актуальные данные)
  * - Так же нижеописанные функции можно вызывать по необходимости (например,
  * если flash-память повреждена - записать в неё уставки из ключа параметром,
  * предварительно считав её с помощью fram_rd_massive)
  ******************************************************************************
  */
#ifndef FRAM_FRAM_HPP_
#define FRAM_FRAM_HPP_

#include "main.h"
#include "flashdata.hpp"

// команды для работы с FRAM
#define FREAD 0x03      //Read Status Register 0 - 255
#define FWRITE 0x02     //Write Status Register
#define FREAD_bkp 0x0B  //Read Status Register 256- 511
#define FWRITE_bkp 0x0A  //Write Status Register

#define WREN 0x06       //Set Write Enable Latch
#define WRDI 0x04       //Write Disable
#define RDSR 0x05       //Read Status Register
#define WRSR 0x01       //Write Status Register

/* Ниже представлены  определения и макросы для сигналов управления FRAM
 ******************************************************************************
 * Fram_SIO - линия данных
 * Fram_SCK - линия тактирования
 * Fram_CS - Chip select
 *
 * Fram_SIO_IN - состояние линии данных
 * MACROS_on - выставить сигнал в 1
 * MACROS_off - выставить сигнал в 0
 *
 * Макросы прописываются индивидуально для каждого приложенияGPIOB
 * (зависит от того, какой микроконтроллер используется,
 * на какие пины каких портов посажены сигналы управления, смотри схему)
 ******************************************************************************
 */
#define Fram_SIO GPIO_PIN_12//Fram_data        33 CPU
#define Fram_SCK GPIO_PIN_13//Fram_Clock       34 CPU
#define Fram_CS  GPIO_PIN_0//Fram_CS           26 CPU

#define Fram_SIO_IN GPIOB->IDR & Fram_SIO
#define Fram_SIO_on GPIOB->BSRR = GPIO_PIN_12//GPIO_PIN_12 SET
#define Fram_SCK_on GPIOB->BSRR = GPIO_PIN_13//GPIO_PIN_13 SET
#define Fram_CS_on  GPIOB->BSRR = GPIO_PIN_0//GPIO_PIN_0 SET

#define Fram_SIO_off GPIOB->BSRR = GPIO_PIN_12 << 16U//GPIO_PIN_13 RESET
#define Fram_SCK_off GPIOB->BSRR = GPIO_PIN_13 << 16U//GPIO_PIN_12 RESET
#define Fram_CS_off  GPIOB->BSRR = GPIO_PIN_0 << 16U//GPIO_PIN_0 RESET
//другие определения
#define delay 30 //задержка 250нс = 5
#define delay_sck 30 //задержка для клока

class Fram {
	private:
	inline static vu8 massive_wr[8];//Массив для записи во FRAM память
	inline static vu8 massive_rd[8];//Массив для считывания из FRAM памяти
public:
	 //------------------------------------Tактирование одного бита------------------------------
	static void fram_pulse(vu8 Delay) {
		uint8_t i;
		for (i = Delay; i; i--) {}
		Fram_SCK_on; //on
		for (i = (Delay + Delay); i; i--) {}
		Fram_SCK_off; //off
		for (i = Delay; i; i--) {}
	}
	//-------------------Передача одного байта, данные в функцию отправляются сразу-----------------------------
	static void framByte(vu8 Data_byt) {
	    for (uint8_t mask = 0x80; mask != 0; mask >>= 1) {
	        (Data_byt & mask) ? Fram_SIO_on : Fram_SIO_off;
	        fram_pulse(delay_sck);
	    }
	}
	 //------------------Чтение одного байта----------------
	static vu8 framByte(void) {
	    uint8_t Data = 0;
	    for (uint8_t mask = 0x80; mask != 0; mask >>= 1) {
	        if (Fram_SIO_IN) Data |= mask;
	        fram_pulse(delay_sck);
	    }
	    return Data;
	}
	//---------------------------CS Up---------------------------------------------------------------------
	static void Fram_CS_Up(void) {
		uint8_t i = 0;
		for (i = delay; i; i--) {}
		Fram_CS_on;
		Fram_SCK_on; //on
		Fram_SIO_on;
		for (i = delay; i; i--) {}
	}
	//-------------------------------------------CS Down----------------------------------------------------------
	static void Fram_CS_Down(void) {
		uint8_t i = 0;
		for (i = delay; i; i--) {}
		for (i = delay; i; i--) {}
		Fram_SIO_on;
		Fram_SCK_off;
		for (i = delay; i; i--) {}
		Fram_CS_off;
		for (i = delay; i; i--) {}
}
	static void Fram_sio_inOut(vu8 inOut){
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		if (inOut == 0) {
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; //На выход открытый коллектор
		} else {
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //На вход
		}
		//ноги на фрам память,            FRAM_sio менять на ходу
		GPIO_InitStruct.Pin = Fram_SIO;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
	static vu8 elementFram(vu8 index) { //Читаем
		fram_rd_massive();
		return massive_rd[index];
	}
	static bool elementFram(vu8 index, vu8 value){//Записываем в один адрес
		massive_wr[index] = value;
		fram_wr_massive();
		return true;
	}
	static bool fram_wr_massive(){//Запись массива во фрам
		  uint8_t i =0;
		  Fram_CS_Up();//cs вверх
		  Fram_CS_Down();
		  framByte(WRSR);
		  framByte(0x00);
		  Fram_CS_Up();
		  Fram_CS_Down();
		  framByte(WREN);
		  Fram_CS_Up();
		  Fram_CS_Down();
		  framByte(FWRITE);
		  framByte(0);
		  for(i= 0 ;i< 8 ;i++)  //а теперь собственно пишем массив, n число байт
		  {
			  framByte(massive_wr[i]);
		  }
		  Fram_CS_Up();
		  return true;
		}
	static vu8 fram_rd_massive() {//Чтение в массив из фрам
		vu8 i = 0;
		Fram_CS_Down();
		for (i = delay; i; i--) {}
		 framByte(FREAD);
		framByte(0);
	  for(i=delay;i;i--) {}
	  for(i=delay;i;i--) {}
	  Fram_sio_inOut(1);//ногу порта на вход настроили
	  for(i=delay;i;i--){}
	  for(i=0;i<8;i++){//читаем массив данных
	    massive_rd[i] = framByte();}
	  for(i=delay;i;i--){}
	  Fram_CS_Up();
	 Fram_sio_inOut(0);//ногу порта на выход настроили
	  return massive_rd[i];
	}
	static vu8 framRD0byte() {//Чтение нулевого байта из фрам
		vu8 byte0Fram;
		vu8 i = 0;
		 Fram_CS_Down();
		for (i = delay; i; i--) {}
		 framByte(FREAD);
		 framByte(0);
		for (i = delay; i; i--) {}
		for (i = delay; i; i--) {}
		Fram_sio_inOut(1);  //ногу порта на вход настроили
		byte0Fram = framByte(); //читаем нулевой байт данных
		for (i = delay; i; i--) {}
		 Fram_CS_Up();
		 Fram_sio_inOut(0); //ногу порта на выход настроили
		return byte0Fram;
	}
};

#endif /* FRAM_FRAM_HPP_ */
