/*
 * rf.h
 *
 * Created: 11.06.2012 11:45:01
 *  Author: fstorm
 */ 


#ifndef RF_H_
#define RF_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_ASKSIN_MSG 30

void rf_asksin_init(void);
bool rf_asksin_receive(uint8_t *packet, uint8_t *rssi, uint8_t *lqi);
void rf_asksin_send(uint8_t *packet);

#endif /* RF_H_ */