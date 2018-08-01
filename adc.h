#ifndef ADC_H_
#define ADC_H_

#include "commons.h"
#include <stdint.h>

/****************************************************************************************************
ВНИМАНИЕ! Программный блок АЦП разрабатывался в расчёте "на вырост"

Движок потенциометра "Время" (установка времени таймера) висит на входе ADC7.

По умолчанию АЦП несколько раз в секунду опрашивает движок. После каждого опроса значение
"фильтруется" и сохраняется, и потом происходит новый опрос движка.

Функция ADC_Controller() в главном цикле в соответствии с ADC_State коммутирует вход АЦП и
запускает оцифровку.

В прерывании АЦП регистр ADCH сохраняется в соответствующий ADC_Value[ADC_State] для включенного
канала и поднимается флаг ADC_Latch = 1 "оцифровка сделана".

При следующем запуске ADC_Controller() в суперцикле подключается очередной вход и запускается
преобразование АЦП, а в прерывании АЦП мы получаем ADC_Value и включаем следующий
ADC_State... и так далее до тошноты.

Вся логика АЦП реализована в adc.c и adc.h
****************************************************************************************************/



/****************************************************************************************************
 * Переменные и макросы АЦП */
 
#define ADC_INPUTS  1	// количество используемых входов АЦП; их следует описать по пинам МК в
						// ADC_Port[] и по ID входов АЦП ADC_State
	
const u08 ADC_Port[ADC_INPUTS] = { 7 };	// порты входов АЦП (движок потенциометра "Время" на ADC7)
 
/* ID входов АЦП ADC_State */
// должны идти без пропусков от 0 до 8
#define LapTime		0	// опрос движка потенциометра "Время" (установка времени RUN)


static u08 ADC_State = LapTime;		// состояние и вход АЦП


#define ADC_POLLING_PERIOD			40	// в миллисекундах период опроса входа АЦП; использовать
										// только кратное десяткам миллисекунд
									
#define ADC_TIME_CONSTANT_OF_LPF	1	// постоянная времени БИХ-фильтра, в секундах;

#define ADC_SAMPLING_FREQUENCY	(1000/ADC_POLLING_PERIOD/ADC_INPUTS) // частота дискретизации АЦП

#define ADC_K_OF_LPF	(ADC_SAMPLING_FREQUENCY*ADC_TIME_CONSTANT_OF_LPF) // коэффициент  БИХ-фильтра


#if ADC_K_OF_LPF < 3 
	#define ADC_K_EXPONENT  1	// приблизительный показатель степени по основанию 2 коэффициента
#elif ADC_K_OF_LPF < 5			// "К"  БИХ-ФНЧ для того, чтобы деление заменить сдвигом
	#define ADC_K_EXPONENT  2
#elif ADC_K_OF_LPF < 9
	#define ADC_K_EXPONENT  3
#elif ADC_K_OF_LPF < 17
	#define ADC_K_EXPONENT  4
#elif ADC_K_OF_LPF < 33
	#define ADC_K_EXPONENT  5
#elif ADC_K_OF_LPF < 65
	#define ADC_K_EXPONENT  6
#elif ADC_K_OF_LPF < 129
	#define ADC_K_EXPONENT  7
#else
	#define ADC_K_EXPONENT  8
#endif

volatile u08 ADC_Value[ADC_INPUTS];	// массив, в который АЦП сбрасывает результат по каждому
									// входу отдельно; у нас пока только один вход "Время"/LapTime;
									// при инициализации LapTime ставится в максимум

static u16 y[ADC_INPUTS];		// вспомогательный массив, хранящий "коэффициенты
								// усреднения" для ADC_Average_Filter_...

volatile u08 ADC_Latch = 0;		// защёлка обновления значений АЦП - разрешает
								// перемены единожды после преобразования.
								// устанавливается ("1") в прерывании АЦП,
								// сбрасывается ("0") после окончания обработки
								// оцифрованного значения


#define ADC_Start()		SetBit(ADCSRA, ADSC)	// старт преобразования АЦП

#define ADC_SetInput(_input) do { ADMUX &= 0xF0; ADMUX |= ((_input) & 0x0F); } while(0)	//выбор входа АЦП

									
/******************************************************************************************
 * Объявления функций */

void ADC_Init(void);
void ADC_Controller(void);
void ADC_Average_Filter_Init(void);
void ADC_Average_Filter_Update(void);
void ADC_Average_Filter_Result(void);

#endif /* ADC_H_ */