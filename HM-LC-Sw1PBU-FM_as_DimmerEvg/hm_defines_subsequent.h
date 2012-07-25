/*
 * hm_defines_subsequent.h
 *
 * Created: 17.07.2012 08:44:44
 *  Author: fstorm
 */ 


#ifndef HM_DEFINES_SUBSEQUENT_H_
#define HM_DEFINES_SUBSEQUENT_H_


typedef struct {
	uint24hm_t ccu_addr;
	hm_peering_t peerings[HM_PEERINGS_MAX];
	hm_parameter_pair_t params_device[HM_PARAM_PAIRS_PER_DEVICE];
	hm_parameter_pair_t params_channels[HM_CHANNEL_COUNT][HM_PARAM_PAIRS_PER_CHANNEL];
	uint8_t params_peerings[HM_PEERINGS_MAX][HM_PARAM_BANKS_PER_PEERING][HM_PARAM_BYTES_PER_PEERING_PER_BANK];
} hm_config_data_t;
	
typedef struct {
	char ee_cookie[2];
	uint8_t ee_version;
	uint24hm_t my_addr;
	char my_serial[10];
	hm_config_data_t config;
} hm_ee_contents_t;

// would be nicer in hm_dimmer.h, but difficult due to include dependencies
extern hm_ee_contents_t EEMEM hm_ee_contents_E;
extern hm_ee_contents_t hm_ee_contents;
extern hm_param_bank_t hm_config_param_banks[HM_CONFIG_PARAM_BANKS_COUNT];

#endif /* HM_DEFINES_SUBSEQUENT_H_ */