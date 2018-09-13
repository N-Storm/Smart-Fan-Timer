#include "commons.h"
#include "adc.h"
#include "gtimer.h"

const u08 ADC_Port[ADC_INPUTS] = { 7 };	// порты входов АЦП (движок потенциометра "Время" на ADC7)

volatile u08 ADC_State = LAP_TIME;	// состояние и вход АЦП; инициируем в состоянии LAP_TIME

volatile u08 ADC_Value[ADC_INPUTS];	// массив, в который АЦП сбрасывает результат по каждому
									// входу отдельно; у нас пока только один вход "Время"/LAP_TIME;

static u16 K_ancillary[ADC_INPUTS];	// вспомогательный массив, хранящий "коэффициенты
									// усреднения" для ADC_Average_Filter_...

volatile u08 ADC_Latch = 0;		// защёлка обновления значений АЦП - разрешает
								// перемены единожды после преобразования.
								// устанавливается ("1") в прерывании АЦП,
								// сбрасывается ("0") после окончания обработки
								// оцифрованного значения


 
/**************************************************************************
*   Function name : ADC_Init
*   Returns :       нет
*   Parameters :    нет
*   Purpose :       Инициализация АЦП
****************************************************************************/

void ADC_Init() {
	
	SET_BIT(ADCSRA, ADEN);		//включаем АЦП
	CLEAR_BIT(ADCSRA, ADSC);	//старт преобразования пока не включаем
	CLEAR_BIT(ADCSRA, ADATE);	//отключаем постоянное преобразование
	SET_BIT(ADCSRA, ADIF);		//снимаем флаг прерывания
	SET_BIT(ADCSRA, ADIE);		//разрешаем прерывания

	//делитель частоты "на 32" для тактирования АЦП на (F_CPU/32) 250кГц
	SET_BIT(ADCSRA, ADPS2);
	CLEAR_BIT(ADCSRA, ADPS1);
	SET_BIT(ADCSRA, ADPS0);

	CLEAR_BIT(ADMUX, REFS1);	//Источник Опорного Напряжения выставляем как
	SET_BIT(ADMUX, REFS0);		//"AVcc с внешним конденсатором на ножке AREF"

	SET_BIT(ADMUX, ADLAR);		//выравниваем результат преобразования по левому краю
	
	ADCSRB = 0;					//мы мудрые - мы чистим этот регистр на всякий случай
	
	DIDR0 = 0;					//оставляем все входы ADC только как цифровые (Note that ADC pins
								//ADC6 and ADC7 do not have digital input buffers, and therefore
								//do not require Digital Input Disable bits) 
	
		// КАНАЛ ПРЕОБРАЗОВАНИЯ ЗДЕСЬ НЕ УКАЗЫВАЕМ
		// ПРЕОБРАЗОВАНИЕ АЦП НЕ ЗАПУСКАЕМ

	ADC_Average_Filter_Init();	// инициируем БИХ-ФНЧ сглаживания значений АЦП

}



/**************************************************************************
*   Function name : ADC_Average_Filter_Init
*   Returns :       нет
*   Parameters :    вход АЦП (ADC_State) и стартовое значение фильтра
*   Purpose :       инициализация фильтра по каждому входу
****************************************************************************/

// цифровой БИХ-ФНЧ обладает практически такой же АЧХ, что и скользящее
// среднее по 2^N, только не требует много памяти
// http://chipenable.ru/index.php/programming-avr/162-prostoy-cifrovoy-filtr.html

void ADC_Average_Filter_Init() {

	K_ancillary[LAP_TIME] = (u08)(u08MAX << ADC_K_EXPONENT);

}


/**************************************************************************
*   Function name : ADC_Average_Filter_Update
*   Returns :       нет
*   Parameters :    принимаемые параметры - номер входа АЦП и уровень по 
*					этому входу
*   Purpose :       формирует и сохраняет оцифровку по входу, усреднённую за
*					2^N отсчётов
****************************************************************************/

// цифровой БИХ-ФНЧ обладает практически такой же АЧХ, что и скользящее
// среднее по 2^N, только не требует много памяти
// http://chipenable.ru/index.php/programming-avr/162-prostoy-cifrovoy-filtr.html

void ADC_Average_Filter_Update(u08 ch) {

//	K_ancillary[ch] += ADC_Value[ADC_State] - (u08)(K_ancillary[ch] >> ADC_K_EXPONENT);	// нечитабельно, лучше развернуть

	K_ancillary[ch] = K_ancillary[ch] + ADC_Value[ADC_State] - (u08)(K_ancillary[ch] >> ADC_K_EXPONENT);

}


/**************************************************************************
*   Function name : ADC_Average_Filter_Result
*   Returns :       возвращает усреднённое значение по входу ADC_State АЦП
*   Parameters :    состояние АЦП ADC_State
*   Purpose :       возвращает результат АЦП по входу ADC_State, усреднённый
*					за 2^ADC_K_EXPONENT отсчётов
****************************************************************************/

// цифровой БИХ-ФНЧ обладает практически такой же АЧХ, что и скользящее
// среднее по 2^N, только не требует много памяти
// http://chipenable.ru/index.php/programming-avr/162-prostoy-cifrovoy-filtr.html

u08 ADC_Average_Filter_Result(u08 ch) {

	return (u08)(K_ancillary[ch] >> ADC_K_EXPONENT);

}



/**************************************************************************
*   Function name : ADC_Controller
*   Returns :       нет
*   Parameters :    нет
*   Purpose :       коммутирует вход АЦП, запускает оцифровку
*					раскладывает значения АЦП по местам, заведует
*					переключением состояния АЦП на другой вход (в перспективе)
****************************************************************************/

void ADC_Controller() {

// если прерывание АЦП уже случилось и ещё не обработано
	if (ADC_Latch) {

		switch (ADC_State)	{
			case LAP_TIME: {
				// значение АЦП пропустить через фильтр для
				// удаления "дрожания" движка потенциометра "Время"
				ADC_Average_Filter_Update(ADC_State);
				
				/* ............... */
				// тут мы бы сменили ADC_State на следующее состояние,
				// но у нас других состояний пока нет, потому просто выходим
				break;
			}
			default:	{				// если мы как-то случайно тут оказались, то
				ADC_State = LAP_TIME;	// "ремонтируем" ADC_State
				break;					// и выходим
			}
		}
		ADC_Latch = 0;			// указываем, что прерывание АЦП обработано и
		return;					// выходим из функции, чтобы не делать помногу за раз
	}


// если прерывание АЦП было обработано ранее и
// если таймер периода опроса АЦП истёк
	else if (GTimer_Exp(GTIMER_ADC)) {

		GTimer_Start(GTIMER_ADC,ADC_POLLING_PERIOD);	// рестартуем таймер АЦП

		switch (ADC_State)	{
			case LAP_TIME:	{
				ADC_SET_INPUT(ADC_Port[ADC_State]);	// подключаем вход АЦП к соответствующей ноге и
				ADC_START();						// запускаем однократное преобразование АЦП
				break;								// и выходим
			}
			default:		{						// если мы как-то случайно тут оказались,
				ADC_State = LAP_TIME;				// то быстренько "ремонтируем" ADC_State и 
				break;								// выходим
			}
		}
	}
	return;
}


