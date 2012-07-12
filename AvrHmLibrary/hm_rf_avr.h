/*
 * hm_rf_avr.h
 *
 * Created: 11.06.2012 11:45:01
 *  Author: fstorm
 */ 


#ifndef HM_RF_AVR_H_
#define HM_RF_AVR_H_


#define MAX_ASKSIN_MSG 30

void hm_rf_init(void);
bool hm_rf_receive(uint8_t *packet, uint8_t *rssi, uint8_t *lqi);
void hm_rf_send_packet(uint8_t *packet);


#endif /* HM_RF_AVR_H_ */