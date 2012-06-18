#ifndef _BOARD_H
#define _BOARD_H

#define F_CPU 8000000L


#define HM_MY_ADDR 0xBEEF01
#define HM_DEVINFO_FIRMWARE 0x01
#define HM_DEVINFO_TYPE 0x0057
#define HM_DEVINFO_SERIAL_NO "GK08154711"
#define HM_DEVINFO_CLASS 0x20
#define HM_DEVINFO_PEER_CHANNEL_A 0x11
#define HM_DEVINFO_PEER_CHANNEL_B 0x01
#define HM_CHANNELS 1


#include <avr/io.h>

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

#define DIM_REL_PORT	PORTB
#define DIM_REL_DDR		DDRB
#define DIM_REL_BIT		0
#define DIM_PWM_PORT	PORTD
#define DIM_PWM_DDR		DDRD
#define DIM_PWM_BIT		4
//#define DIM_PWM_PORT	PORTB
//#define DIM_PWM_DDR		DDRB
//#define DIM_PWM_BIT		3

#define DIM_T1_PORT		PORTA
#define DIM_T1_DDR		DDRA
#define DIM_T1_PIN		PINA
#define DIM_T1_BIT		4
#define DIM_T2_PORT		PORTB
#define DIM_T2_DDR		DDRB
#define DIM_T2_PIN		PINB
#define DIM_T2_BIT		2
#define DIM_T3_PORT		PORTD
#define DIM_T3_DDR		DDRD
#define DIM_T3_PIN		PIND
#define DIM_T3_BIT		3

#define LED_PORT		PORTA
#define LED_DDR			DDRA
#define LED_BIT			1
#define LED_ON_PORT		PORTA
#define LED_ON_DDR		DDRA
#define LED_ON_BIT		0

#endif