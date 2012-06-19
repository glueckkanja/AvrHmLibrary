/*
 * debug.c
 *
 * Created: 11.06.2012 16:39:31
 *  Author: fstorm
 */ 

#include "debug.h"
#include "external/AvrHmLibrary/hm_main.h"

#include <util/atomic.h>
#include <util/delay.h>

char uart_out[20];

void debug_dump(uint8_t *p_buffer, uint16_t len, const char *prefix)
{
	if (prefix)
		uart_puts(prefix);
		
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		uint16_t timer_ms = (uint16_t)hm_timer_ticks * HM_TIMER_MS_PER_TICKS;
		uint8_t timer_secs = hm_timer_secs + timer_ms / 1000;
		timer_ms = timer_ms % 1000;
		sprintf(uart_out, "%03df%03d: ", timer_secs, timer_ms);
	}
	uart_puts(uart_out);		

	for (uint16_t i = 0; i < len; i++)
	{
		sprintf(uart_out, "%02X ", p_buffer[i]);
		uart_puts(uart_out);
	}
	uart_puts_P("\r\n");
}

void debug_dump_integer(int8_t value)
{
	sprintf(uart_out, "Value: %d\r\n", value);
	uart_puts(uart_out);
}

void blink_led(uint8_t count)
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