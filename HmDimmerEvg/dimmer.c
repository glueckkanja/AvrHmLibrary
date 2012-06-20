/*
 * dimmer.c
 *
 * Created: 08.06.2012 17:01:07
 *  Author: fstorm
 */ 

#include "board_defines.h"
#include "debug.h"
#include "dimmer.h"
#include <avr/sfr_defs.h>

int8_t dim_level_percent;

void dimmer_init()
{
	DIM_REL_PORT &= ~_BV(DIM_REL_BIT);
	DIM_REL_DDR |= _BV(DIM_REL_BIT);

	DIM_PWM_DDR &= ~_BV(DIM_PWM_BIT);				// direction input during setup
	DIM_PWM_PORT &= ~_BV(DIM_PWM_BIT);				// disable pullup
	OCR1A = 199;									// top = 199
	TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS12);	// prescaler 1024 => 8.000.000 / 1024 / 200 = 157 Hz???
	TCCR1A = _BV(COM1B1) | _BV(WGM11) | _BV(WGM10);	// clear OC1B on compare match (OCR1B), set at bottom, top = OCR1A
	dimmer_set(0);									// set initial state
}

void dimmer_set(int8_t percent)
{
	if (percent < 0)
		percent = 0;
	if (percent > 100)
		percent = 100;
		
	dim_level_percent = percent;
	debug_dump_integer(percent);
	
	if (percent > 0)
	{
		OCR1B = percent * 2;
		DIM_PWM_DDR |= _BV(DIM_PWM_BIT);	// direction output
		DIM_REL_PORT |= _BV(DIM_REL_BIT);
	}
	else
	{
		DIM_REL_PORT &= ~_BV(DIM_REL_BIT);
		DIM_PWM_DDR &= ~_BV(DIM_PWM_BIT);	// direction input to stop pwm when off
	}
}
