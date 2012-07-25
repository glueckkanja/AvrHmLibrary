/*
 * main.c
 *
 * Created: 08.06.2012 09:30:41
 *  Author: fstorm
 */ 


#include "hm_defines_hardware.h"

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include "dimmer.h"
#include <hm_main.h>

#include "debug.h"


FUSES = {
	.extended = FUSE_BODLEVEL1,		// BOD 2V7
	.high = FUSE_SPIEN & FUSE_EESAVE & FUSE_BOOTSZ0 & FUSE_BOOTSZ1, 
	.low = FUSE_SUT1 & FUSE_SUT0 & FUSE_CKSEL3 & FUSE_CKSEL2 & FUSE_CKSEL0,	// BOD enabled, internal RC
	//.low = 0xFF,					// no fuse set -> EXTXOSC_8MHZ_XX_16KCK_65MS
};
LOCKBITS = (LB_MODE_1 & BLB0_MODE_1 & BLB1_MODE_1);		// no restrictions


int main(void)
{
	// watchdog is still enabled after a software reset and needs to be cleared immediately
	MCUSR = 0;
	wdt_disable();
	
	//uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(57600, F_CPU));
	
	buttons_leds_init();
	dimmer_init();
	hm_init();
	sei();
	
	while(1)
    {
		hm_task();
    }
}
