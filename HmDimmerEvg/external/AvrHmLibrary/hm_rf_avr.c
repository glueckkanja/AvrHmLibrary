/*
 * hm_rf_avr.c
 *
 * Created: 11.06.2012 11:44:38
 *  Author: fstorm
 */ 

#include "hm_main.h"

#include <avr/pgmspace.h>
#include <util/delay.h>
#include "cc1100_defines.h"


#define HM_CC1100_CS_ON		CC1100_CS_PORT &= ~_BV(CC1100_CS_BIT)
#define HM_CC1100_CS_OFF	CC1100_CS_PORT |= _BV(CC1100_CS_BIT)


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


static inline void hm_spi_init()
{
	PRR &= ~_BV(PRSPI);
	SPI_PORT |= _BV(SPI_SCLK);
	SPI_DDR  |= (_BV(SPI_MOSI) | _BV(SPI_SCLK) | _BV(SPI_SS));
	SPI_DDR  &= ~_BV(SPI_MISO);
	
	SPCR  = _BV(MSTR) | _BV(SPE);
	SPSR |= _BV(SPI2X);
}

static inline uint8_t hm_cc1100_send_byte(uint8_t data)
{
	SPDR = data;						// send byte
	loop_until_bit_is_set(SPSR, SPIF);	// wait until transfer finished
	return SPDR;
}

static inline uint8_t hm_cc1100_read_reg(uint8_t addr)
{
	HM_CC1100_CS_ON;
	hm_cc1100_send_byte(addr | CC1100_READ_BURST);
	uint8_t ret = hm_cc1100_send_byte(0);
	HM_CC1100_CS_OFF;

	return ret;
}

static inline void hm_cc1100_write_reg(uint8_t addr, uint8_t data)
{
	HM_CC1100_CS_ON;
	hm_cc1100_send_byte(addr | CC1100_WRITE_BURST);
	hm_cc1100_send_byte(data);
	HM_CC1100_CS_OFF;
}

static inline uint8_t hm_cc1100_send_strobe(uint8_t strobe)
{
	HM_CC1100_CS_ON;
	uint8_t ret = hm_cc1100_send_byte(strobe);
	HM_CC1100_CS_OFF;

	return ret;
}

static inline void hm_cc1100_receive_mode(void)
{
	uint8_t cnt = 0xff;
	while(cnt-- && (hm_cc1100_send_strobe(CC1100_SRX) & 0x70) != 1)
		_delay_us(10);
}

void hm_rf_init(void)
{
	hm_spi_init();
	
	EIMSK &= ~_BV(CC1100_INT);				// disable INT - we'll poll...
	CC1100_CS_DDR |= _BV(CC1100_CS_BIT);	// CS as output

	HM_CC1100_CS_OFF;						// Toggle chip select signal
	_delay_us(30);
	HM_CC1100_CS_ON;
	_delay_us(30);
	HM_CC1100_CS_OFF;
	_delay_us(45);

	hm_cc1100_send_strobe(CC1100_SRES);			// Send SRES command
	_delay_us(100);

	// load configuration
	for (uint8_t i = 0; i < 50; i += 2)
	{
		if (pgm_read_byte(&ASKSIN_CFG[i]) > 0x40)
			break;

		hm_cc1100_write_reg(pgm_read_byte(&ASKSIN_CFG[i]), pgm_read_byte(&ASKSIN_CFG[i+1]));
	}
	
	hm_cc1100_send_strobe(CC1100_SCAL);

    _delay_ms(4);             // 4ms: Found by trial and error
}

bool hm_rf_receive(uint8_t *packet, uint8_t *rssi, uint8_t *lqi)
{
	// see if a CRC OK pkt has been arrived
	if (bit_is_set(CC1100_IN_PIN, CC1100_IN_BIT))
	{
		packet[0] = hm_cc1100_read_reg(CC1100_RXFIFO) & 0x7f; // read len
		if (packet[0] >= MAX_ASKSIN_MSG)
			packet[0] = MAX_ASKSIN_MSG - 1;
		
		HM_CC1100_CS_ON;
		hm_cc1100_send_byte(CC1100_READ_BURST | CC1100_RXFIFO);
		for (uint8_t i = 0; i < packet[0]; i++) {
			packet[i + 1] = hm_cc1100_send_byte(0);
		}
		*rssi = hm_cc1100_send_byte(0);
		*lqi = hm_cc1100_send_byte(0);
		HM_CC1100_CS_OFF;

		hm_cc1100_send_strobe(CC1100_SFRX);
		hm_cc1100_send_strobe(CC1100_SIDLE);
		hm_cc1100_send_strobe(CC1100_SNOP);
		hm_cc1100_send_strobe(CC1100_SRX);

		return true;
	}
	
	switch (hm_cc1100_read_reg(CC1100_MARCSTATE))
	{
		case MARCSTATE_RXFIFO_OVERFLOW:
		case MARCSTATE_IDLE:
		hm_cc1100_send_strobe(CC1100_SFRX);
		hm_cc1100_send_strobe(CC1100_SIDLE);
		hm_cc1100_send_strobe(CC1100_SNOP);
		hm_cc1100_send_strobe(CC1100_SRX);
		break;
	}
	
	return false;
}

void hm_rf_send_packet(uint8_t *packet)
{
	hm_cc1100_send_strobe(CC1100_SIDLE);
	hm_cc1100_send_strobe(CC1100_SFRX);
	hm_cc1100_send_strobe(CC1100_SFTX);

	// send
	HM_CC1100_CS_ON;
	hm_cc1100_send_byte(CC1100_WRITE_BURST | CC1100_TXFIFO);
	for(uint8_t i = 0; i <= packet[0]; i++) {
		hm_cc1100_send_byte(packet[i]);
	}
	HM_CC1100_CS_OFF;

	hm_cc1100_send_strobe(CC1100_SFRX);
	hm_cc1100_send_strobe(CC1100_STX);
	
	while(hm_cc1100_read_reg(CC1100_MARCSTATE) != MARCSTATE_IDLE)
		_delay_ms(5);
	
	hm_cc1100_receive_mode();
}
