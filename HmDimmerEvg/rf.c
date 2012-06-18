/*
 * rf.c
 *
 * Created: 11.06.2012 11:44:38
 *  Author: fstorm
 */ 

#include "board.h"
#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include "cc1100.h"
#include "rf.h"

#define CC1100_ASSERT    CC1100_CS_PORT &= ~_BV(CC1100_CS_BIT)
#define CC1100_DEASSERT  CC1100_CS_PORT |= _BV(CC1100_CS_BIT)

const uint8_t PROGMEM ASKSIN_CFG[50] = {
	CC1100_IOCFG2, 0x07,
	CC1100_IOCFG0, 0x2e,
	CC1100_FIFOTHR, 0x0d,
	CC1100_SYNC1, 0xE9,
	CC1100_SYNC0, 0xCA,
	CC1100_PKTCTRL1, 0x0C,
	CC1100_FSCTRL1, 0x06,
	CC1100_FREQ2, 0x21,
	CC1100_FREQ1, 0x65,
	CC1100_FREQ0, 0x6A,
	CC1100_MDMCFG4, 0xC8,
	CC1100_MDMCFG3, 0x93,
	CC1100_MDMCFG2, 0x03,
	CC1100_DEVIATN, 0x34,
	CC1100_MCSM1, 0x30, // always go into IDLE
	CC1100_MCSM0, 0x18,
	CC1100_FOCCFG, 0x16,
	CC1100_AGCCTRL2, 0x43,
	CC1100_FREND1, 0x56,
	CC1100_FSCAL1, 0x00,
	CC1100_FSCAL0, 0x11,
	CC1100_TEST1, 0x35,
	CC1100_PATABLE, 0xc3,
	0xff
};

void spi_init(void)
{
	PRR &= ~_BV(PRSPI);
	SPI_PORT |= _BV(SPI_SCLK);
	SPI_DDR  |= (_BV(SPI_MOSI) | _BV(SPI_SCLK) | _BV(SPI_SS));
	SPI_DDR  &= ~_BV(SPI_MISO);
	
	SPCR  = _BV(MSTR) | _BV(SPE);
	SPSR |= _BV(SPI2X);
}

uint8_t cc1100_sendByte(uint8_t data)
{
	SPDR = data;						// send byte
	loop_until_bit_is_set(SPSR, SPIF);	// wait until transfer finished
	return SPDR;
}

uint8_t cc1100_readReg(uint8_t addr)
{
	CC1100_ASSERT;
	cc1100_sendByte(addr | CC1100_READ_BURST);
	uint8_t ret = cc1100_sendByte(0);
	CC1100_DEASSERT;

	return ret;
}

void cc1100_writeReg(uint8_t addr, uint8_t data)
{
	CC1100_ASSERT;
	cc1100_sendByte(addr | CC1100_WRITE_BURST);
	cc1100_sendByte(data);
	CC1100_DEASSERT;
}

uint8_t cc1100_sendStrobe(uint8_t strobe)
{
	CC1100_ASSERT;
	uint8_t ret = cc1100_sendByte(strobe);
	CC1100_DEASSERT;

	return ret;
}

void cc1100_receiveMode(void)
{
	uint8_t cnt = 0xff;
	while(cnt-- && (cc1100_sendStrobe(CC1100_SRX) & 0x70) != 1)
		_delay_us(10);
}

void rf_asksin_init(void)
{
	spi_init();
	
	EIMSK &= ~_BV(CC1100_INT);				// disable INT - we'll poll...
	CC1100_CS_DDR |= _BV(CC1100_CS_BIT);	// CS as output

	CC1100_DEASSERT;						// Toggle chip select signal
	_delay_us(30);
	CC1100_ASSERT;
	_delay_us(30);
	CC1100_DEASSERT;
	_delay_us(45);

	cc1100_sendStrobe(CC1100_SRES);			// Send SRES command
	_delay_us(100);

	// load configuration
	for (uint8_t i = 0; i < 50; i += 2)
	{
		if (pgm_read_byte(&ASKSIN_CFG[i]) > 0x40)
			break;

		cc1100_writeReg(pgm_read_byte(&ASKSIN_CFG[i]), pgm_read_byte(&ASKSIN_CFG[i+1]));
	}
	
	cc1100_sendStrobe(CC1100_SCAL);

    _delay_ms(4);             // 4ms: Found by trial and error
}

bool rf_asksin_receive(uint8_t *packet, uint8_t *rssi, uint8_t *lqi)
{
	// see if a CRC OK pkt has been arrived
	if (bit_is_set(CC1100_IN_PIN, CC1100_IN_BIT))
	{
		packet[0] = cc1100_readReg(CC1100_RXFIFO) & 0x7f; // read len
		if (packet[0] >= MAX_ASKSIN_MSG)
			packet[0] = MAX_ASKSIN_MSG - 1;
		
		CC1100_ASSERT;
		cc1100_sendByte(CC1100_READ_BURST | CC1100_RXFIFO);
		for (uint8_t i = 0; i < packet[0]; i++) {
			packet[i + 1] = cc1100_sendByte(0);
		}
		*rssi = cc1100_sendByte(0);
		*lqi = cc1100_sendByte(0);
		CC1100_DEASSERT;

		cc1100_sendStrobe(CC1100_SFRX);
		cc1100_sendStrobe(CC1100_SIDLE);
		cc1100_sendStrobe(CC1100_SNOP);
		cc1100_sendStrobe(CC1100_SRX);

		return true;
	}
	
	switch (cc1100_readReg(CC1100_MARCSTATE))
	{
		case MARCSTATE_RXFIFO_OVERFLOW:
		case MARCSTATE_IDLE:
		cc1100_sendStrobe(CC1100_SFRX);
		cc1100_sendStrobe(CC1100_SIDLE);
		cc1100_sendStrobe(CC1100_SNOP);
		cc1100_sendStrobe(CC1100_SRX);
		break;
	}
	
	return false;
}

void rf_asksin_send(uint8_t *packet)
{
	cc1100_sendStrobe(CC1100_SIDLE);
	cc1100_sendStrobe(CC1100_SFRX);
	cc1100_sendStrobe(CC1100_SFTX);

	// send
	CC1100_ASSERT;
	cc1100_sendByte(CC1100_WRITE_BURST | CC1100_TXFIFO);
	for(uint8_t i = 0; i <= packet[0]; i++) {
		cc1100_sendByte(packet[i]);
	}
	CC1100_DEASSERT;

	cc1100_sendStrobe(CC1100_SFRX);
	cc1100_sendStrobe(CC1100_STX);
	
	while(cc1100_readReg(CC1100_MARCSTATE) != MARCSTATE_IDLE)
		_delay_ms(5);
	
	cc1100_receiveMode();
}
