/*
 * buttons_leds.c
 *
 * Created: 20.06.2012 15:53:39
 *  Author: fstorm
 */ 

#include "board_defines.h"
#include <avr/wdt.h>

#include "buttons_leds.h"
#include "external/AvrHmLibrary/hm_main.h"


volatile led_state_t led1_state = led_off;
volatile uint8_t led1_count_down = 0;
volatile uint8_t b3_long_count = 0;


void buttons_leds_init()
{
	LED0_INIT();
	LED1_INIT();
	
    LED0_ON();
}

void buttons_leds_timer()
{
	switch (led1_state) {
		case led_off:
			LED1_OFF();
			break;
			
		case led_on:
			LED1_ON();
			break;
			
		case led_blink_slow:
			if (!(hm_timer_ticks % HM_TIMER_TICKS_FROM_MS(300)))
				LED1_TOGGLE();
			break;
			
		case led_blink_fast:
			if (!(hm_timer_ticks % HM_TIMER_TICKS_FROM_MS(150)))
				LED1_TOGGLE();
			break;
	}
	
	if (led1_count_down)
	{
		if (led1_state != led_off)
		{
			led1_count_down--;
			if (!led1_count_down)
				led1_state = led_off;
		}
		else
			led1_count_down = 0;		
	}
}

void b3_short()
{
	if (b3_long_count)
	{
		b3_long_count = 0;
		led1_state = led_off;
	}		
	else
		hm_do_start_pairing = true;
}

void b3_long()
{
	b3_long_count++;
	
	if (b3_long_count == 1)
		led1_state = led_blink_slow;
	if (b3_long_count >= 2)
		led1_state = led_blink_fast;
}

void b3_long_end()
{
	if (b3_long_count >= 2)
	{
		hm_do_reset = true;
		b3_long_count = 0;
	}	
}

void reset_device()
{
	wdt_enable(WDTO_250MS);
	while (1);
}