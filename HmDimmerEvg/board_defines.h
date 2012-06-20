#ifndef _BOARD_DEFINES_H
#define _BOARD_DEFINES_H

#define F_CPU 8000000L


#include <avr/io.h>
#include <avr/sfr_defs.h>


#define SPI_PORT		PORTB
#define SPI_DDR			DDRB
#define SPI_SS			4
#define SPI_MISO		6
#define SPI_MOSI		5
#define SPI_SCLK		7

#define CC1100_CS_PORT	SPI_PORT
#define CC1100_CS_DDR	SPI_DDR
#define CC1100_CS_BIT	SPI_SS

#define CC1100_OUT_PORT	PORTB
#define CC1100_OUT_DDR	DDRB
#define CC1100_OUT_BIT	1

#define CC1100_IN_PORT	PORTD
#define CC1100_IN_DDR	DDRD
#define CC1100_IN_PIN	PIND
#define CC1100_IN_BIT	2
#define CC1100_INT		INT0
#define CC1100_INTVECT	INT0_vect
#define CC1100_ISC		ISC00
#define CC1100_EICR		EICRA

#define LED0_INIT()		LED0_OFF(); DDRA  |= _BV(0);
#define LED0_ON()		PORTA |= _BV(0)
#define LED0_OFF()		PORTA &= ~_BV(0)
#define LED0_TOGGLE()	PORTA ^= _BV(0)

#define LED1_INIT()		LED1_OFF(); DDRA  |= _BV(1);
#define LED1_ON()		PORTA |= _BV(1)
#define LED1_OFF()		PORTA &= ~_BV(1)
#define LED1_TOGGLE()	PORTA ^= _BV(1)

#define B1_INIT()		PORTA &= ~_BV(4); PORTA &= ~_BV(4);
#define B1_STATE		(PINA & _BV(4))
#define B2_INIT()		PORTB &= ~_BV(2); PORTB &= ~_BV(2);
#define B2_STATE		(PINB & _BV(2))
#define B3_INIT()		PORTD &= ~_BV(3); PORTD &= ~_BV(3);
#define B3_STATE		(PIND & _BV(3))

#define DIM_REL_PORT	PORTB
#define DIM_REL_DDR		DDRB
#define DIM_REL_BIT		0
#define DIM_PWM_PORT	PORTD
#define DIM_PWM_DDR		DDRD
#define DIM_PWM_BIT		4
//#define DIM_PWM_PORT	PORTB
//#define DIM_PWM_DDR		DDRB
//#define DIM_PWM_BIT		3

#define HM_TIMER_INIT() \
	OCR0A = 195;						/* ~ 40 Hz => 25 ms	*/	\
	TCCR0B = _BV(CS02) | _BV(CS00);		/* prescaler 1024	*/	\
	TCCR0A = _BV(WGM01);				/* CTC				*/	\
	TIMSK0 = _BV(OCIE0A);				/* enable interrupt */
#define HM_TIMER_ISR_VECT TIMER0_COMPA_vect

#define HM_TIMER_MS_PER_TICKS	25
#define HM_TIMER_TICKS_MAX		120		// ~ 3 secs per round
#define HM_TIMER_SECS_MAX		120

#endif