#include "gtimer.h"
#include "commons.h"


#define TIMER_STOPPED	0	// Таймер остановлен
#define TIMER_RUNNING	1	// Таймер работает
#define TIMER_PAUSED	2	// Таймер на паузе


volatile u08 SysTick = 0;		// Инициализация флага SysTick

static volatile u32 SysTime;	// системное время

static volatile u08 GTStates[GTIMER_MAX_IDs];	// текущие состояния каждого таймера (все
												// инициализируются как TIMER_STOPPED)

static volatile u32 GTDelay[GTIMER_MAX_IDs];	// массив задержек для каждого таймера
												// GTDelay[GTimerID] = SysTime + delay;
												// все инициализируются как "истёкшие"



//-----------------------------------------
//	Function name :	GTimer_Init()
//	Returns :		нет
//	Parameters :	GTIMER_MAX_IDs - Количество используемых таймеров (максимум 255)
//					(определяется в gtimer.h)
//	Purpose :		Нулевая инициализация всех рабочих таймеров
//-----------------------------------------

void GTimer_Init() {

	SysTime = 0;							// обнуляем системное время

	for(u08 i=0; i<GTIMER_MAX_IDs; i++) {	// стартуем все (определённые в GTimerID)
		GTimer_Start(i, 0);					// таймеры с временем 0 (ноль) систик
	}


}



//-----------------------------------------
//	Function name :	SysTime_Handler()
//	Returns :		нет
//	Parameters :	нет
//	Purpose :		Обработчик системного времени
//-----------------------------------------

void SysTime_Handler() {
	SysTick = 0;			// опускаем флаг систика

	DISABLE_INTERRUPT();
		SysTime++;			// сдвигаем стрелку системных часов
	RESTORE_INTERRUPT();
		
	// если устройство не RUN (не в работе), а системное время SysTime превысило
	// половину размера своей переменной,
	if ((GTStates[GTIMER_RUN] == TIMER_STOPPED) && ((u32MAX/2) < SysTime)) {
		DISABLE_INTERRUPT();
		GTimer_Init();		// то обнуляем системное время и
							// реинициализируем все рабочие таймеры
		RESTORE_INTERRUPT();
	}
}


//-----------------------------------------
//	Function name :	GTimer_Start(u08 GTimerID,u32 delay)
//	Returns :		нет
//	Parameters :	u08 GTimerID - идентификатор таймера (определяется в maintimer.h)
//					u32 delay - устанавливаемое время отсчёта таймера в миллисекундах
//	Purpose :		Запуск указанного таймера на указанную длительность
//-----------------------------------------

void GTimer_Start(u08 GTimerID,u32 delay) {
	GTStates[GTimerID] = TIMER_RUNNING;
	DISABLE_INTERRUPT();
		GTDelay[GTimerID] = (delay/SYS_TICK_PERIOD) + SysTime;
	RESTORE_INTERRUPT();
}


//-----------------------------------------
//	Function name :	GTimer_Stop(u08 GTimerID)
//	Returns :		нет
//	Parameters :	u08 GTimerID - идентификатор таймера (определяется в maintimer.h)
//	Purpose :		Принудительная остановка указанного таймера с обнулением длительности;
//					По смыслу - принудительное "истечение времени" таймера
//-----------------------------------------

void GTimer_Stop(u08 GTimerID)	{				
	GTStates[GTimerID] = TIMER_STOPPED;
	GTDelay[GTimerID] = 0;
}


//-----------------------------------------
//	Function name :	GTimer_Exp(u08 GTimerID)
//	Returns :		YES (таймер истёк) или NO (таймер ещё не истёк)
//	Parameters :	u08 GTimerID - идентификатор таймера (определяется в maintimer.h)
//	Purpose :		Проверка истечения бегущего таймера
//-----------------------------------------

u08 GTimer_Exp(u08 GTimerID) {
	if (GTStates[GTimerID] == TIMER_STOPPED) {
		return YES;
	}
	else	if (SysTime < GTDelay[GTimerID]) {
				return NO;
			}
			else {
				GTStates[GTimerID] = TIMER_STOPPED;
				return YES;
			}
	return NO;
}


//-----------------------------------------
//	Function name :	GTimer_Pause(u08 GTimerID)
//	Returns :		нет
//	Parameters :	u08 GTimerID - идентификатор таймера (определяется в maintimer.h)
//	Purpose :		Пауза бегущего таймера с запоминанием остатка времени
//-----------------------------------------

void GTimer_Pause(u08 GTimerID) {
	if(GTStates[GTimerID] == TIMER_RUNNING) {
		GTStates[GTimerID] = TIMER_PAUSED;
		GTDelay[GTimerID] = GTDelay[GTimerID] - SysTime;
	}
}


//-----------------------------------------
//	Function name :	GTimer_Release(u08 GTimerID)
//	Returns :		нет
//	Parameters :	u08 GTimerID - идентификатор таймера (определяется в maintimer.h)
//	Purpose :		Продолжение работы таймера по остатку времени
//-----------------------------------------

void GTimer_Release(u08 GTimerID)  {
	if(GTStates[GTimerID] == TIMER_PAUSED) {
		GTStates[GTimerID] = TIMER_RUNNING;
		GTDelay[GTimerID] = GTDelay[GTimerID] + SysTime;
	}
}


//-----------------------------------------
//	Function name :	GTimer_Get_Remainder(u08 GTimerID)
//	Returns :		Остаток времени таймера
//	Parameters :	u08 GTimerID - идентификатор таймера (определяется в maintimer.h)
//	Purpose :		Получение остатка времени таймера
//-----------------------------------------

u32 GTimer_Get_Remainder(u08 GTimerID)	{
	return (GTDelay[GTimerID] > SysTime ? (GTDelay[GTimerID] - SysTime) : 0);
}
