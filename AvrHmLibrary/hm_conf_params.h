/*
 * hm_conf_params.h
 *
 * Created: 14.06.2012 13:18:28
 *  Author: fstorm
 */ 


#ifndef HM_CONF_PARAMS_H_
#define HM_CONF_PARAMS_H_


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


int8_t hm_config_get_param_bank_id(uint8_t channel, uint8_t peer_id, uint8_t param_list, uint8_t param_index);
void hm_config_write(uint8_t channel, uint8_t peer_id, uint8_t param_list, hm_parameter_pair_t *p_params, uint8_t count, uint24hm_t *p_frm_addr_src);
bool hm_config_load();
void hm_config_store();


#endif /* HM_CONF_PARAMS_H_ */