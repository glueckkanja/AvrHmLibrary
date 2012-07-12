/*
 * debug.c
 *
 * Created: 11.06.2012 16:39:31
 *  Author: fstorm
 */ 

#include "debug.h"
#include <hm_main.h>

#include <util/atomic.h>
#include <util/delay.h>

char uart_out[20];

void debug_print_time()
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		uint16_t timer_ms = (uint16_t)hm_timer_ticks * HM_TIMER_MS_PER_TICK;
		uint8_t timer_secs = hm_timer_secs + timer_ms / 1000;
		timer_ms = timer_ms % 1000;
		sprintf(uart_out, "%03df%03d: ", timer_secs, timer_ms);
	}
	uart_puts(uart_out);		
}

void debug_print(const char *print_string)
{
	debug_print_time();
	uart_puts(print_string);
}
		
void debug_dump(uint8_t *p_buffer, uint16_t len, const char *prefix)
{
	debug_print_time();
	
	if (prefix)
		uart_puts(prefix);
		
	for (uint16_t i = 0; i < len; i++)
	{
		sprintf(uart_out, "%02X ", p_buffer[i]);
		uart_puts(uart_out);
	}
	uart_puts_P("\r\n");
}

void debug_dump_integer(uint8_t value)
{
	debug_print_time();
	
	sprintf(uart_out, "Value: %d\r\n", value);
	uart_puts(uart_out);
}

void debug_dump_integer_raw(uint8_t value)
{
	sprintf(uart_out, "%03d ", value);
	uart_puts(uart_out);
}

void debug_blink_led(uint8_t count)
{
	LED_OFF();
	_delay_ms(500);
	while (count--)
	{
		LED_ON();
		_delay_ms(200);
		LED_OFF();
		_delay_ms(200);
	}
	_delay_ms(500);
}

void debug_blink_led_burst()
{
	LED_OFF(); _delay_ms(200);
	LED_TOGGLE(); _delay_ms(50);
	LED_TOGGLE(); _delay_ms(50);
	LED_TOGGLE(); _delay_ms(50);
	LED_TOGGLE(); _delay_ms(50);
	LED_TOGGLE(); _delay_ms(50);
	LED_OFF(); _delay_ms(200);
}
