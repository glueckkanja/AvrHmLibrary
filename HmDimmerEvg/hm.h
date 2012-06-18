/*
 * hm.h
 *
 * Created: 11.06.2012 14:17:24
 *  Author: fstorm
 */ 


#ifndef HM_H_
#define HM_H_

#include "hm_general.h"
#include "hm_parameters_peers.h"

extern hm_param_bank_t hm_config_param_banks[18];
#define HM_PEERS_EEPROM_LOC (void *)(0x0014 + 13 * 16)

void hm_init();
void hm_config_reset();
void hm_peering_reset(uint8_t peer_id);

void hm_task();
void hm_send();

void hm_send_device_info(hm_uint24_t dst);
void hm_pair_remote(hm_uint24_t *p_dst);

#endif /* HM_H_ */