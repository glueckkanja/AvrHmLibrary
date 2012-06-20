/*
 * buttons_leds.h
 *
 * Created: 20.06.2012 15:53:59
 *  Author: fstorm
 */ 
#ifndef BUTTONS_LEDS_H_
#define BUTTONS_LEDS_H_


#include "dimmer.h"


typedef enum {
	led_off = 0,
	led_on,
	led_blink_slow,
	led_blink_fast
} led_state_t;

extern volatile led_state_t led1_state;
extern volatile uint8_t led1_count_down;

void buttons_leds_init();
void buttons_leds_timer();

void b3_short();
void b3_long();
void b3_long_end();

void reset_device();


static inline __attribute__((always_inline)) void button0_short()
{
	dimmer_set(0);
}
static inline __attribute__((always_inline)) void button1_short()
{
	dimmer_set(100);
}

static inline __attribute__((always_inline)) void button0_long()
{
	dimmer_change_ms(0, 250);
}
static inline __attribute__((always_inline)) void button1_long()
{
	dimmer_change_ms(1, 250);
}


#endif /* BUTTONS_LEDS_H_ */