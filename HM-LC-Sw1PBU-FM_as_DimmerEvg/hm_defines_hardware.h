#ifndef HM_DEFINES_HARDWARE_H_
#define HM_DEFINES_HARDWARE_H_

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

//GD0
#define CC1100_OUT_PORT	PORTD
#define CC1100_OUT_DDR	DDRD
#define CC1100_OUT_BIT	2

//GD2
#define CC1100_IN_PORT	PORTD
#define CC1100_IN_DDR	DDRD
#define CC1100_IN_PIN	PIND
#define CC1100_IN_BIT	3
#define CC1100_INT		INT1
#define CC1100_INTVECT	INT1_vect
#define CC1100_ISC		ISC10
#define CC1100_EICR		EICRA

// "main" LED
#define LED_INIT()		LED_OFF(); DDRB  |= _BV(0);
#define LED_ON()		PORTB |= _BV(0);
#define LED_OFF()		PORTB &= ~_BV(0);
#define LED_TOGGLE()	PORTB ^= _BV(0);

// buttons digital inputs
#define B_UP_INIT()		PORTD |= _BV(0); DDRD &= ~_BV(0);	// pull-up on, input
#define B_UP_STATE		(!(PIND & _BV(0)))
#define B_DOWN_INIT()	PORTD |= _BV(6); DDRD &= ~_BV(6);	// pull-up on, input
#define B_DOWN_STATE	(!(PIND & _BV(6)))
#define B_CONFIG_INIT()	PORTD |= _BV(7); DDRD &= ~_BV(7);	// pull-up on, input
#define B_CONFIG_STATE	(!(PIND & _BV(7)))

// relay output
#define DIM_REL_PORT	PORTD
#define DIM_REL_DDR		DDRD
#define DIM_REL_BIT		4

// pwm
#define DIM_PWM_PORT	PORTB
#define DIM_PWM_DDR		DDRB
#define DIM_PWM_BIT		3
#define DIM_PWM_INIT() \
	TCCR0A = _BV(COM0A1) | _BV(COM0A0)	/* set OC0A on compare match (OCR0A), clear at bottom (inverting mode)	*/ \
			| _BV(WGM01) | _BV(WGM00);	/* fast PWM 8-Bit, TOP = 0xff											*/ \
	TCCR0B = _BV(CS02) | _BV(CS00);		/* prescaler 1024														*/
#define DIM_PWM_OUT		OCR0A

// timer
#define HM_TIMER_INIT() \
	OCR2A = 195;								/* ~ 40 Hz => 25 ms	*/	\
	TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20);	/* prescaler 1024	*/	\
	TCCR2A = _BV(WGM21);						/* CTC				*/	\
	TIMSK2 = _BV(OCIE2A);						/* enable interrupt */

#define HM_TIMER_ISR_VECT TIMER2_COMPA_vect

#define HM_TIMER_MS_PER_TICK			25
#define HM_TIMER_TICKS_MAX				120	// 3 secs per round
#define HM_TIMER_SECS_MAX				120 // 2 minutes

#endif