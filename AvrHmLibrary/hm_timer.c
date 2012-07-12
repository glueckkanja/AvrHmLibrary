/*
 * hm_timer.c
 *
 * Created: 15.06.2012 17:39:36
 *  Author: fstorm
 */ 

#include "hm_main.h"

#include <avr/interrupt.h>


volatile uint8_t hm_timer_ticks = 0;
volatile uint8_t hm_timer_secs = 0;

void hm_timer_init()
{
	HM_TIMER_INIT();
}

ISR(HM_TIMER_ISR_VECT, ISR_BLOCK)
{
#if defined HM_TIMER_TASK_INCLUDE_HIGHSPEED && defined HM_TIMER_HIGHSPEED_TICKS_MAX

	static uint8_t hm_timer_ticks_highspeed = 0;

	HM_TIMER_TASK_INCLUDE_HIGHSPEED();
	
	if (++hm_timer_ticks_highspeed < HM_TIMER_HIGHSPEED_TICKS_MAX)
		return;
	hm_timer_ticks_highspeed = 0;

#endif

	if(++hm_timer_ticks >= HM_TIMER_TICKS_MAX)
	{
		hm_timer_ticks = 0;
		hm_timer_secs += HM_TIMER_TICKS_MAX * HM_TIMER_MS_PER_TICK / 1000;
		if (hm_timer_secs >= HM_TIMER_SECS_MAX)
			hm_timer_secs = 0;
	}
	
	hm_buttons_timer();
	
#ifdef HM_TIMER_TASK_INCLUDE
	HM_TIMER_TASK_INCLUDE();
#endif
}