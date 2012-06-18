/*
 * clock.h
 *
 * Created: 15.06.2012 17:39:46
 *  Author: fstorm
 */ 


#ifndef CLOCK_H_
#define CLOCK_H_


extern volatile uint8_t ticks_10ms;
extern volatile uint8_t secs;

void clock_init();


#endif /* CLOCK_H_ */