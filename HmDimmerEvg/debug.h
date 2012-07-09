/*
 * debug.h
 *
 * Created: 11.06.2012 11:54:00
 *  Author: fstorm
 */ 


#ifndef DEBUG_H_
#define DEBUG_H_

#include "board_defines.h"
#include <stdint.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "external/pfleury/uart.h"

void debug_print(const char *print_string);
void debug_dump(uint8_t *p_buffer, uint16_t len, const char *prefix);
void debug_dump_integer(uint8_t value);
void debug_dump_integer_raw(uint8_t value);
void debug_blink_led(uint8_t count);
void debug_blink_led_burst();

#endif /* DEBUG_H_ */