//***************************************************************************
//
//  Author(s):	Pashgan (исходный код опроса кнопок) http://chipenable.ru/index.php/programming-avr/item/38-uchebnyy-kurs-opros-knopok.html
//				Vlady (адаптация под проект)
//
//***************************************************************************

#ifndef	BUTTONS_H_
#define	BUTTONS_H_

#include <avr/io.h>
#include "commons.h"


#define BUTTON_POLLING_PERIOD		10	// период опроса кнопок в миллисекундах
#define BUTTON_POLLING_THRESHOLD	5	// количество опросов кнопки до "срабатывания"


/******************************************************************************************
 * Порт подключения кнопок */

#define BUTTONS_PORT	PORTD
#define BUTTONS_PIN		PIND
#define BUTTONS_DDR		DDRD

/******************************************************************************************
 * Номера выводов порта подключения кнопок */

#define PIN_RUN	 		4
#define PIN_STOP 		5
#define PIN_SELECTOR	6

/******************************************************************************************
 * Коды кнопок */

#define KEY_NULL		0
#define KEY_RUN			1
#define KEY_STOP		2
#define KEY_SELECTOR	3


/******************************************************************************************
 * Объявления функций */

void BUTTONS_Init();
void BUTTONS_Debrief();
u08 BUTTONS_Get_Key();
void BUTTONS_Set_Key(u08 key);

#endif //BUTTONS_H_
