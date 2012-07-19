/*
 * dimmer.c
 *
 * Created: 08.06.2012 17:01:07
 *  Author: fstorm
 */ 

#include "hm_defines_hardware.h"
#include "debug.h"
#include "dimmer.h"
#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>

int8_t dim_level_percent;

const uint8_t PROGMEM dimmer_translation_table[26] = {0, 34, 60, 81, 99, 114, 127, 139, 150, 160, 169, 177, 185, 192, 199, 206, 212, 217, 223, 228, 233, 238, 242, 247, 251, 255};
//const uint8_t PROGMEM dimmer_translation_table[26] = {0, 10, 20, 31, 41, 51, 61, 71, 82, 92, 102, 112, 122, 133, 143, 153, 163, 173, 184, 194, 204, 214, 224, 235, 245, 255};
	
void dimmer_init()
{
	DIM_REL_PORT &= ~_BV(DIM_REL_BIT);
	DIM_REL_DDR |= _BV(DIM_REL_BIT);

	DIM_PWM_DDR &= ~_BV(DIM_PWM_BIT);				// direction input during setup
	DIM_PWM_PORT &= ~_BV(DIM_PWM_BIT);				// disable pullup
	TCCR1A = _BV(COM1B1) | _BV(WGM10);				// clear OC1B on compare match (OCR1B), set at bottom
	TCCR1B = _BV(WGM12) | _BV(CS12);				// fast PWM 8-Bit, prescaler 1024
	dimmer_set(0);									// set initial state
}

void dimmer_set(int8_t percent)
{
	if (percent < 0)
		percent = 0;
	if (percent > 100)
		percent = 100;
		
	dim_level_percent = percent;
	
	if (percent > 0)
	{
		OCR1B = pgm_read_byte(&dimmer_translation_table[percent / 4]);
		DIM_PWM_DDR |= _BV(DIM_PWM_BIT);	// direction output
		DIM_REL_PORT |= _BV(DIM_REL_BIT);
	}
	else
	{
		DIM_REL_PORT &= ~_BV(DIM_REL_BIT);
		DIM_PWM_DDR &= ~_BV(DIM_PWM_BIT);	// direction input to stop pwm when off
	}
}
