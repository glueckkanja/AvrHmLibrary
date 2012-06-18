/*
 * hm_parameters_peers.h
 *
 * Created: 14.06.2012 13:18:28
 *  Author: fstorm
 */ 


#ifndef HM_PARAMETERS_PEERS_H_
#define HM_PARAMETERS_PEERS_H_


#include "hm_general.h"


typedef struct hm_parameter_pair {
	uint8_t index;
	uint8_t value;
} hm_parameter_pair_t;

typedef struct {
	uint8_t pair_count;
	hm_parameter_pair_t *param_pairs;
} hm_param_bank_pairs_t;

typedef struct {
	uint8_t offset;
	uint8_t param_count;
	uint8_t *params;
} hm_param_bank_sequence_t;

typedef struct {
	uint8_t channel;
	uint8_t peer_id;
	uint8_t param_list;
	bool is_sequence;
	union {
		hm_param_bank_pairs_t param_bank_pairs;
		hm_param_bank_sequence_t param_bank_seq;
	};
	void *p_eeprom_address;
	bool is_dirty;
} hm_param_bank_t;


#define HM_PEERS_MAX 1 + 4 // CCU + 4 other devices
typedef struct {
	hm_uint24_t addr;
	uint8_t channel;
} hm_peer_t;
extern hm_peer_t hm_peers[HM_PEERS_MAX];
extern bool hm_is_peers_dirty;


#endif /* HM_PARAMETERS_PEERS_H_ */