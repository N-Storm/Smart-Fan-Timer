#include "leds.h"
#include "commons.h"
#include "gtimer.h"


//-----------------------------------------
//   Function name : LEDs_Init
//   Returns :       нет
//   Parameters :    нет
//   Purpose :       инициализация портов вывода LED
//-----------------------------------------

void LEDs_Init() {
	
	SET_BIT(SYS_LED_DDR, SYS_LED);	// линию SysLED настроить как вывод
	SYS_LED_OFF();					// SysLED вЫключить
	return;
}



//-----------------------------------------
//	Function name :	SysLED_Blink
//	Returns :		нет
//	Parameters :	нет
//	Purpose :		Автомат SysLED
//					программное мигание SysLED'ом с целью
//					визуального контроля работы ядра
//-----------------------------------------

void SysLED_Blink() {

static u08 state = 0;

	switch(state) 	{
	
		case 0:	
			SYS_LED_ON();			// включаем SysLED
			GTimer_Start(GTIMER_SYS_LED,SYS_LED_DURATION_ON);	// запускаем таймер
			state = 1;				// меняем состояние свича
			break;

		case 1:
			SYS_LED_OFF();			// вЫключаем SysLED
			GTimer_Start(GTIMER_SYS_LED,SYS_LED_DURATION_OFF);	// запускаем таймер
			state = 0;				// меняем состояние свича
			break;

		default:	
			SYS_LED_OFF();			// вЫключаем SysLED
			GTimer_Stop(GTIMER_SYS_LED); // стопарим таймер 
			state = 0;				// ремонтируем состояние свича
			break;

	}
}



