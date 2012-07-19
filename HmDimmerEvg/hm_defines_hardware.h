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
#define CC1100_OUT_PORT	PORTB
#define CC1100_OUT_DDR	DDRB
#define CC1100_OUT_BIT	1

//GD2
#define CC1100_IN_PORT	PORTD
#define CC1100_IN_DDR	DDRD
#define CC1100_IN_PIN	PIND
#define CC1100_IN_BIT	2
#define CC1100_INT		INT0
#define CC1100_INTVECT	INT0_vect
#define CC1100_ISC		ISC00
#define CC1100_EICR		EICRA

// "main" LED
#define LED_INIT()		LED0_INIT(); LED10_INIT(); LED11_INIT();
#define LED_ON()		LED0_ON(); LED10_ON(); LED11_ON();
#define LED_OFF()		LED0_OFF(); LED10_OFF(); LED11_OFF();
#define LED_TOGGLE()	LED0_TOGGLE(); LED10_TOGGLE(); LED11_TOGGLE();

// LED on Dimmer Board
#define LED0_INIT()		LED0_OFF(); DDRA  |= _BV(5);
#define LED0_ON()		PORTA &= ~_BV(5);
#define LED0_OFF()		PORTA |= _BV(5);
#define LED0_TOGGLE()	PORTA ^= _BV(5);

// LEDs on CSM module
#define LED10_INIT()	LED10_OFF(); DDRA  |= _BV(0);
#define LED10_ON()		PORTA |= _BV(0);
#define LED10_OFF()		PORTA &= ~_BV(0);
#define LED10_TOGGLE()	PORTA ^= _BV(0);
#define LED11_INIT()	LED11_OFF(); DDRA  |= _BV(1);
#define LED11_ON()		PORTA |= _BV(1);
#define LED11_OFF()		PORTA &= ~_BV(1);
#define LED11_TOGGLE()	PORTA ^= _BV(1);

// buttons digital inputs
#define B_UP_INIT()		PORTA &= ~_BV(4); DDRA &= ~_BV(4);	// pull-up off, input
#define B_UP_STATE		(PINA & _BV(4))
#define B_DOWN_INIT()	PORTB &= ~_BV(2); DDRB &= ~_BV(2);	// pull-up off, input
#define B_DOWN_STATE	(PINB & _BV(2))
#define B_CONFIG_INIT()	DDRD &= ~_BV(3); PORTD |= _BV(3);	// input, pull-up on
#define B_CONFIG_STATE	(!(PIND & _BV(3)))

// analog input for gira system 2000 nebenstelle
// disable digital input PD4
// 1.1V ref, result left adjust, source PD4
// ADC enable, frequency prescaler 128
#define G2000_INIT()	DIDR0 &= ~_BV(ADC4D); \
						ADMUX = _BV(REFS1) | _BV(ADLAR) | _BV(MUX2); \
						ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
#define G2000_START()	ADCSRA |= _BV(ADSC);	// set start bit
#define G2000_VALUE		ADCH					// use high byte only

// relay and pwm outputs
#define DIM_REL_PORT	PORTB
#define DIM_REL_DDR		DDRB
#define DIM_REL_BIT		0
#define DIM_PWM_PORT	PORTD
#define DIM_PWM_DDR		DDRD
#define DIM_PWM_BIT		4

// timer
#define HM_TIMER_INIT0() \
	OCR0A = 195;						/* ~ 40 Hz => 25 ms	*/	\
	TCCR0B = _BV(CS02) | _BV(CS00);		/* prescaler 1024	*/	\
	TCCR0A = _BV(WGM01);				/* CTC				*/	\
	TIMSK0 = _BV(OCIE0A);				/* enable interrupt */

#define HM_TIMER_INIT()					/* high speed timer	*/ \
	OCR0A = 124;						/* 1000 Hz => 1 ms	*/ \
	TCCR0B = _BV(CS01) | _BV(CS00);		/* prescaler 64		*/ \
	TCCR0A = _BV(WGM01);				/* CTC				*/ \
	TIMSK0 = _BV(OCIE0A);				/* enable interrupt	*/
#define HM_TIMER_HIGHSPEED_TICKS_MAX	25

#define HM_TIMER_ISR_VECT TIMER0_COMPA_vect

#define HM_TIMER_MS_PER_TICK			25
#define HM_TIMER_TICKS_MAX				120	// 3 secs per round
#define HM_TIMER_SECS_MAX				120 // 2 minutes

#endif