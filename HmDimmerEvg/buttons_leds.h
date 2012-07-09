/*
 * buttons_leds.h
 *
 * Created: 20.06.2012 15:53:59
 *  Author: fstorm
 */ 
#ifndef BUTTONS_LEDS_H_
#define BUTTONS_LEDS_H_


#include "external/AvrHmLibrary/hm_buttons.h"
#include "dimmer.h"


typedef enum {
	led_off = 0,
	led_on,
	led_blink_slow,
	led_blink_fast
} led_state_t;

extern volatile led_state_t led_state;

void buttons_leds_init();
void buttons_leds_timer();

void b3_short();
void b3_long();
void b3_long_end();

void reset_device();


static inline __attribute__((always_inline)) void button1_short()
{
	dimmer_set(100);
}
static inline __attribute__((always_inline)) void button1_long()
{
	dimmer_change_ms(1, 250);
}

static inline __attribute__((always_inline)) void button2_short()
{
	dimmer_set(0);
}
static inline __attribute__((always_inline)) void button2_long()
{
	dimmer_change_ms(0, 250);
}

static inline __attribute__((always_inline)) void gira_event(hm_button_signal_type_t signal_type, hm_button_gira_nebenstelle_state_t button)
{
	if (signal_type == short_click)
	{
		if (button == upper)
			dimmer_set(100);
		else if (button == lower)
			dimmer_set(0);
		else if (button == both)
			dimmer_toggle();
	}
	else if (signal_type == long_press_start || signal_type == long_press_repeat)
	{
		if (button == upper)
			dimmer_change_ms(1, 250);
		else if (button == lower)
			dimmer_change_ms(0, 250);
	}			
}
#endif /* BUTTONS_LEDS_H_ */