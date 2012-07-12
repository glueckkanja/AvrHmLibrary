/*
 * HmDimmerEvg.c
 *
 * Created: 08.06.2012 09:30:41
 *  Author: fstorm
 */ 


#include "board_defines.h"

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include "dimmer.h"
#include <hm_main.h>

#include "debug.h"

int main(void)
{
	// watchdog is still enabled after a software reset and needs to be cleared immediately
	MCUSR = 0;
	wdt_disable();
	
	clock_prescale_set(clock_div_1);
	
	uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(57600, F_CPU));

	buttons_leds_init();
	dimmer_init();
	hm_init();
	sei();
	
	while(1)
    {
		hm_task();
    }
}
	