/*
 * hm_dimmer.h
 *
 * Created: 19.06.2012 14:50:04
 *  Author: fstorm
 */ 


#ifndef HM_DIMMER_H_
#define HM_DIMMER_H_



extern hm_param_bank_t hm_config_param_banks[18];
#define HM_PEERS_EEPROM_LOC (void *)(0x0014 + 13 * 16)


void hm_config_reset();
void hm_peering_reset(uint8_t peer_id);
void hm_dimmer_frm_handler();
void hm_pair_remote(uint24hm_t *p_dst);


#endif /* HM_DIMMER_H_ */