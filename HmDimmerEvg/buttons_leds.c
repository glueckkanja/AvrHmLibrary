/*
 * buttons_leds.c
 *
 * Created: 20.06.2012 15:53:39
 *  Author: fstorm
 */ 

#include "board_defines.h"
#include <avr/wdt.h>
#include <util/delay.h>

#include <hm_main.h>
#include "buttons_leds.h"


volatile led_state_t led_state = led_off;
volatile bool do_reset_on_b3_release = false;


void buttons_leds_init()
{
	LED_INIT();
	
    LED_ON(); _delay_ms(500); LED_OFF(); _delay_ms(150); 
	LED_ON(); _delay_ms(150); LED_OFF(); _delay_ms(150); 
	LED_ON(); _delay_ms(150); LED_OFF(); _delay_ms(150); 
	LED_ON(); _delay_ms(500); LED_OFF(); 
}

void buttons_leds_timer()
{
	static uint8_t led_timer = 0;
	
	if (led_timer > 0)
		led_timer--;
		
	switch (led_state)
	{
		case led_on:
			led_timer = 0;
			LED_ON();
			break;
			
		case led_blink_slow:
		case led_blink_fast:
			if (led_timer == 0)
			{
				led_timer = led_state == led_blink_fast ? HM_TIMER_TICKS_FROM_MS(150) : HM_TIMER_TICKS_FROM_MS(300);
				LED_TOGGLE();
			}				
			break;
		
		default:
			led_timer = 0;
			LED_OFF();
	}
	
}

void b3_short()
{
	if (hm_is_waiting_for_pairing)
	{
		hm_do_stop_pairing = true;
	}		
	else 
	{
		switch (dim_level_percent)	// step trough different levels to ease setup and installation
		{
			case 0:
				dimmer_set(100);
				break;
			case 100:
				dimmer_set(50);
				break;
			case 50:
				dimmer_set(96);
				break;
			case 96:
				dimmer_set(4);
				break;
			default:
				dimmer_set(0);
		}
	}
}

void b3_long()
{
	if (!hm_is_waiting_for_pairing)
	{
		hm_do_start_pairing = true;	// first long press -> start pairing
	}		
	else
	{
		led_state = led_blink_fast;	// second long press -> do a reset on release
		do_reset_on_b3_release = true;
	}		
}

void b3_long_end()
{
	if (do_reset_on_b3_release)
	{
		do_reset_on_b3_release = false;
		hm_do_reset = true;
	}	
}

void reset_device()
{
	wdt_enable(WDTO_250MS);
	while (1);
}