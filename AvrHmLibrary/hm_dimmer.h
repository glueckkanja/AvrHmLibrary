/*
 * hm_dimmer.h
 *
 * Created: 19.06.2012 14:50:04
 *  Author: fstorm
 */ 


#ifndef HM_DIMMER_H_
#define HM_DIMMER_H_


#define HM_PARAM_PAIRS_PER_DEVICE 5
#define HM_PARAM_PAIRS_PER_CHANNEL 5
#define HM_PARAM_BANKS_PER_PEERING 4
#define HM_PARAM_BYTES_PER_PEERING_PER_BANK 13
#define HM_CONFIG_PARAM_BANKS_COUNT (1 + HM_CHANNEL_COUNT + HM_PEERINGS_MAX * HM_PARAM_BANKS_PER_PEERING)	// device + x channels + y peerings * banks


bool hm_eeprom_check();
void hm_config_init();
void hm_config_reset();
void hm_peering_reset(uint8_t peering_id, uint8_t peer_channel);
void hm_dimmer_frame_handler();
void hm_pair_remote(uint24hm_t *p_remote_addr, uint8_t remote_channel_a, uint8_t remote_channel_b);


#endif /* HM_DIMMER_H_ */