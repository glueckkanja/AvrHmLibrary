/*
 * hm_general.h
 *
 * Created: 14.06.2012 13:12:16
 *  Author: fstorm
 */ 


#ifndef HM_GENERAL_H_
#define HM_GENERAL_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "rf.h"


#define ASKSIN_MSG_TRUNK_SIZE 10
#define ASKSIN_MSG_REMAINDER_SIZE MAX_ASKSIN_MSG - ASKSIN_MSG_TRUNK_SIZE

typedef struct hm_uint16 {uint8_t byte[2];} hm_uint16_t;
typedef struct hm_uint24 {uint8_t byte[3];} hm_uint24_t;
#define HM_UINT24_FROM_UINT32(input_uint32_t) (struct hm_uint24) {{ (uint8_t)((uint32_t)(input_uint32_t) >> 16), (uint8_t)((uint16_t)(input_uint32_t) >> 8), (uint8_t)(input_uint32_t) }}
#define UINT32_FROM_HM_UINT24(input_hm_uint24_t) ((uint32_t)(input_hm_uint24_t).byte[0] << 16 |  (uint16_t)(input_hm_uint24_t).byte[1] << 8 | (input_hm_uint24_t).byte[2])
#define HM_UINT16_FROM_UINT16(input_uint16_t) (struct hm_uint16) {{ (uint8_t)((uint16_t)(input_uint16_t) >> 8), (uint8_t)(input_uint16_t) }}
#define UINT16_FROM_HM_UINT16(input_hm_uint16_t) ((uint16_t)(input_hm_uint16_t).byte[0] << 8 | (uint8_t)(input_hm_uint16_t).byte[1])
#define ISEQUAL_HM_UINT16_TO_UINT16(input_hm_uint16_t,input_uint16_t) ( \
	(input_hm_uint16_t).byte[0] == (uint8_t)((uint16_t)(input_uint16_t) >> 8) && \
	(input_hm_uint16_t).byte[1] == (uint8_t)(input_uint16_t))
#define ISEQUAL_HM_UINT24_TO_UINT32(input_hm_uint24_t,input_uint32_t) ( \
	0 == (uint8_t)((uint32_t)(input_uint32_t) >> 24) && \
	(input_hm_uint24_t).byte[0] == (uint8_t)((uint32_t)(input_uint32_t) >> 16) && \
	(input_hm_uint24_t).byte[1] == (uint8_t)((uint16_t)(input_uint32_t) >> 8) && \
	(input_hm_uint24_t).byte[2] == (uint8_t)(input_uint32_t))
#define ISEQUAL_HM_UINT24_TO_HM_UINT24(input_hm_uint24_t,input2_hm_uint24_t) ( \
	(input_hm_uint24_t).byte[0] == (input2_hm_uint24_t).byte[0] && \
	(input_hm_uint24_t).byte[1] == (input2_hm_uint24_t).byte[1] && \
	(input_hm_uint24_t).byte[2] == (input2_hm_uint24_t).byte[2])


#include "hm_parameters_peers.h"

extern bool hm_is_message_from_ccu;
extern bool hm_is_waiting_for_pairing;

extern bool hm_is_in_mode_config;
extern uint8_t hm_mode_config_channel;
extern int8_t hm_mode_config_peer_id;
extern uint8_t hm_mode_config_param_list;

bool hm_init_peering(hm_uint24_t *p_peer_addr, uint8_t peer_channel);
bool hm_remove_peering(hm_uint24_t *p_peer_addr, uint8_t peer_channel);
int8_t hm_get_peer_id_by_addr_channel(hm_uint24_t *p_peer_addr, uint8_t peer_channel);
int8_t hm_config_get_param_bank_id(uint8_t channel, uint8_t peer_id, uint8_t param_list, uint8_t param_index);
void hm_config_write(uint8_t channel, uint8_t peer_id, uint8_t param_list, hm_parameter_pair_t *p_params, uint8_t count, hm_uint24_t *p_msg_src_addr);
bool hm_config_load();
void hm_config_store();


#endif /* HM_GENERAL_H_ */