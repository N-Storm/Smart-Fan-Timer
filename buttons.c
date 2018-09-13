#include "buttons.h"
#include "commons.h"
#include "gtimer.h"


static u08 pressedKey = 0;



/**************************************************************************
*   Function name : BUTTONS_Init
*   Returns :       нет
*   Parameters :    нет
*   Purpose :       инициализация кнопочных портов как входов
****************************************************************************/
void BUTTONS_Init() {
	CLEAR_BIT(BUTTONS_DDR,PIN_RUN);			// порты кнопок как входы
	CLEAR_BIT(BUTTONS_DDR,PIN_STOP);		// порты кнопок как входы
	CLEAR_BIT(BUTTONS_DDR,PIN_SELECTOR);	// порты кнопок как входы
	
	SET_BIT(BUTTONS_PORT,PIN_RUN);			// ВКЛ внутреннюю подтяжку
	SET_BIT(BUTTONS_PORT,PIN_STOP);			// ВКЛ внутреннюю подтяжку
	SET_BIT(BUTTONS_PORT,PIN_SELECTOR);		// ВКЛ внутреннюю подтяжку
}


/**************************************************************************
*   Function name : BUTTONS_Debrief
*   Returns :       нет
*   Parameters :    нет
*   Purpose :       опрашивает кнопки. если кнопка нажата в течение
*					нескольких последовательных опросов,
*                   ее номер записывается в буфер
****************************************************************************/
void BUTTONS_Debrief() {

GTimer_Start(GTIMER_BUTTONS, BUTTON_POLLING_PERIOD); // запускаем таймер

static u08 comp = 0;

u08 key = 0;

	// последовательный опрос кнопок (выводов МК)
	if (BIT_IS_CLEAR(BUTTONS_PIN, PIN_RUN)) key = KEY_RUN;
	else	if (BIT_IS_CLEAR(BUTTONS_PIN, PIN_STOP)) key = KEY_STOP;
			else	if (BIT_IS_CLEAR(BUTTONS_PIN, PIN_SELECTOR)) key = KEY_SELECTOR;
					else key = KEY_NULL;

	// если во временной переменной что-то есть
	if (key) {
  
    // и если кнопка реально удерживается, а не искрит, то
	// записать ее номер в буфер 
		if (comp > (BUTTON_POLLING_THRESHOLD-1)) {
			comp = BUTTON_POLLING_THRESHOLD+10;
			pressedKey = key;
			return;
		}
		else if (comp < (BUTTON_POLLING_THRESHOLD+5)) comp++;
	}
	else comp=0;
}

/**************************************************************************
*   Function name : BUTTONS_Get_Key
*   Returns :       номер нажатой кнопки
*   Parameters :    нет
*   Purpose :       возвращает содержимое кнопочного буфера
*                   при этом буфер очищается
****************************************************************************/
u08 BUTTONS_Get_Key() {
		u08 key = pressedKey;
		pressedKey = KEY_NULL;
		return key;
}


/**************************************************************************
*   Function name : BUTTONS_Set_Key
*   Returns :       нет
*   Parameters :    номер кнопки
*   Purpose :       записывает в кнопочный буфер значение
*                   требуется иногда для имитации нажатия кнопок
****************************************************************************/
void BUTTONS_Set_Key(u08 key) {
		pressedKey = key;
}

