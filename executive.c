#include "executive.h"
#include "commons.h"
#include "adc.h"
#include "buttons.h"
#include "gtimer.h"
#include "leds.h"

//-----------------------------------------
//   Function name : EXECUTIVE_Init
//   Returns :       нет
//   Parameters :    нет
//   Purpose :       инициализация портов ввода/вывода
//-----------------------------------------

void EXECUTIVE_Init() {
	
	SET_BIT(FAN_1_DDR,FAN_1);		// линию FAN_1 настроить как вывод
	FAN_1_OFF();					// FAN_1 вЫключить

	SET_BIT(FAN_2_DDR,FAN_2);		// линию FAN_2 настроить как вывод
	FAN_2_OFF();					// FAN_2 вЫключить

	return;
}


//-----------------------------------------
//   Function name : EXECUTIVE()
//   Returns :       нет
//   Parameters :    GTIMER_RUN, BUTTONS_Get_Key(), 
//   Purpose :       главный исполнительный модуль
//-----------------------------------------

void EXECUTIVE() {

u08 key = BUTTONS_Get_Key();		// получаем из буфера код нажатой кнопки

static u08 selector_state = 0;		// переменная конечного автомата перебора вариантов
									// кнопкой SELECTOR

	if (GTimer_Exp(GTIMER_RUN)) {	// если таймер GTIMER_RUN истёк
	
		FAN_1_OFF();				// FAN_1 вЫключить
		FAN_2_OFF();				// FAN_2 вЫключить
	
		if (key == KEY_RUN)	{		// если нажата кнопка "RUN"

			// получаем установленное "Время" работы вентиляторов
			u08 lap_time = ADC_Average_Filter_Result(LAP_TIME);

			// стартуем на установленное "Время" таймер LAP_TIME
			GTimer_Start(LAP_TIME,lap_time);

			FAN_1_ON();				// FAN_1 включаем
			FAN_2_ON();				// FAN_2 включаем
			
		}
	}
	
	else {							// если таймер GTIMER_RUN ещё не истёк и
									// нажата какая-то кнопка, то
		
		switch (key) {
			
			case KEY_NULL:			// никакая кнопка не нажата - ничего не делаем
			
				break;
			
			case KEY_RUN:			// при работающем таймере GTIMER_RUN каждое нажатие
									// кнопки RUN рестартует GTIMER_RUN на новое время,
									// установленное потенциометром "Время"
				
				// получаем новое установленное "Время" работы вентиляторов
				u08 lap_time = ADC_Average_Filter_Result(LAP_TIME);

				// принудительно останавливаем таймер LAP_TIME
				GTimer_Stop(LAP_TIME);

				// стартуем таймер LAP_TIME на новое установленное "Время"
				GTimer_Start(LAP_TIME,lap_time);

				break;
			
			case KEY_STOP:			// если нажата кнопка STOP, то достаточно приказать,
									// что таймер "истёк". Остальное сделает стандартная
									// процедура "остановки по истечению времени".

				// принудительно останавливаем таймер LAP_TIME
				GTimer_Stop(LAP_TIME);

				break;
			
			case KEY_SELECTOR:	
				
				// при каждом нажатии кнопки SELECTOR по кольцу переключаемся
				// на следующий набор работы группы вентиляторов
				selector_state = selector_state + 1;
				
				if (selector_state > 2) { 
					selector_state = 0;
				}
				
				switch(selector_state) 	{
	
					case 0:

						FAN_1_ON();				// FAN_1 включаем
						FAN_2_ON();				// FAN_2 включаем
						break;

					case 1:

						FAN_1_ON();				// FAN_1 включаем
						FAN_2_OFF();			// FAN_2 вЫключаем
						break;

					case 2:

						FAN_1_OFF();			// FAN_1 вЫключаем
						FAN_2_ON();				// FAN_2 включаем
						break;
						
					default:

						selector_state = 0;		// ремонт состояния свича
						break;
				}

				break;
			
			default:	
				
				key = KEY_NULL;					// ремонт состояния свича
				break;
		}
	}
	return;
}


