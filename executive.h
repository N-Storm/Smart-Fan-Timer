#ifndef	EXECUTIVE_H_
#define	EXECUTIVE_H_

#include <avr/io.h>
#include "commons.h"



/******************************************************************************************
 * Порты FAN1 и FAN2 */

#define FAN_1_DDR		DDRB
#define FAN_1_PORT		PORTB
#define FAN_1			1
#define FAN_1_ON()		CLEAR_BIT(FAN_1_PORT,FAN_1)	// активный уровень - низкий. СЛЕДИТЬ!
#define FAN_1_OFF()		SET_BIT(FAN_1_PORT,FAN_1)

#define FAN_2_DDR		DDRB
#define FAN_2_PORT		PORTB
#define FAN_2			2
#define FAN_2_ON()		CLEAR_BIT(FAN_2_PORT,FAN_2)	// активный уровень - низкий. СЛЕДИТЬ!
#define FAN_2_OFF()		SET_BIT(FAN_2_PORT,FAN_2)

	
/******************************************************************************************
 * Объявления функций */

void EXECUTIVE_Init();
void EXECUTIVE();



#endif // EXECUTIVE_H_
