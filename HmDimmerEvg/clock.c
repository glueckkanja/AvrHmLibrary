/*
 * clock.c
 *
 * Created: 15.06.2012 17:39:36
 *  Author: fstorm
 */ 

#include "board.h"
#include "debug.h"

#include <avr/sfr_defs.h>
#include <avr/interrupt.h>

volatile uint8_t ticks_10ms = 0;
volatile uint8_t secs = 0;

//#define BUTTON_TIME_DEBOUNCE 10
//#define BUTTON_TIME_LONG 40
//volatile uint8_t button_t1_state = 0;

void clock_init()
{
	OCR0A = 78;							// ~ 100 Hz, 10 ms
	TCCR0B = _BV(CS02) | _BV(CS00);		// prescaler 1024
	TCCR0A = _BV(WGM01);				// CTC
	TIMSK0 = _BV(OCIE0A);				// enable interrupt
}

ISR(TIMER0_COMPA_vect, ISR_BLOCK)
{
	//static uint8_t button_t1_press_start = 0xff;
	
	if(++ticks_10ms >= 100)
	{
		ticks_10ms = 0;
		secs++;
	}
	
	//if (bit_is_set(DIM_T1_PIN, DIM_T1_BIT))
	//{
		//if (button_t1_press_start == 0xff)
			//button_t1_press_start = ticks_10ms;
		//else
			//if (ticks_10ms > button_t1_press_start + BUTTON_TIME_LONG || ticks_10ms < button_t1_press_start)
				//button_t1_state = 2;
	//}
	//else
		//if (button_t1_press_start != 0xff)
		//{
			//if (ticks_10ms > button_t1_press_start + BUTTON_TIME_DEBOUNCE || ticks_10ms < button_t1_press_start)
			//
		//}
			
}