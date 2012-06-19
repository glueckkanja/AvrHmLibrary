/*
 * HmDimmerEvg.c
 *
 * Created: 08.06.2012 09:30:41
 *  Author: fstorm
 */ 


#include "board_defines.h"

#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "dimmer.h"
#include "external/AvrHmLibrary/hm_main.h"

#include "debug.h"

int main(void)
{
	clock_prescale_set(clock_div_1);
	
    //#ifdef DEBUG
	LED_INIT();_delay_ms(200);
	LED_ON();_delay_ms(200);LED_OFF();_delay_ms(200);
	LED_ON();_delay_ms(200);LED_OFF();_delay_ms(200);
	LED_ON();_delay_ms(200);LED_OFF();_delay_ms(200);
	//_delay_ms(1000);
	//LED_ON();_delay_ms(500);LED_OFF();_delay_ms(200);
	//LED_ON();_delay_ms(500);LED_OFF();
	
	uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(9600, F_CPU));
	//#endif

	hm_timer_init();
	
	dimmer_init();
	//while(1)
	//{
		//dimmer_set(100);
		//_delay_ms(1000);
		//dimmer_set(75);
		//_delay_ms(1000);
		//dimmer_set(50);
		//_delay_ms(1000);
		//dimmer_set(25);
		//_delay_ms(1000);
		//dimmer_set(0);
		//_delay_ms(1000);
	//}		
	
	hm_init();
	
	sei();
	
	while(1)
    {
        hm_task();
		dimmer_check_keys();
		
		if (uart_getc() != UART_NO_DATA)
			hm_send_device_info((uint24hm_t){ 0x000000 });
    }
}