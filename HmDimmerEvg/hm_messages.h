/*
 * hm_messages.h
 *
 * Created: 11.06.2012 21:05:53
 *  Author: fstorm
 */ 


#ifndef HM_MESSAGES_H_
#define HM_MESSAGES_H_


#include "hm_general.h"
#include "hm_parameters_peers.h"


uint8_t hm_msgctr;


#define INIT_HM_MSG_TRUNK(hm_msg_target,msg_len,msg_ctr,msg_cmd_uint16,msg_dst) \
	memset(&(hm_msg_target), 0, sizeof((hm_msg_target))); \
	(hm_msg_target).len = (msg_len); \
	(hm_msg_target).msgctr = (msg_ctr); \
	(hm_msg_target).cmd = HM_UINT16_FROM_UINT16((msg_cmd_uint16)); \
	(hm_msg_target).src = HM_UINT24_FROM_UINT32(HM_MY_ADDR); \
	(hm_msg_target).dst = (msg_dst);

#define HM_ISEQUAL_HM_MSG_CMD_TO_UINT16(hm_msg_cmd,cmd_compare) ((UINT16_FROM_HM_UINT16((hm_msg_cmd)) & ~0x0400) == cmd_compare)


#define HM_SUBCMD_ACK 0x00
#define HM_SUBCMD_ACK_STATUS 0x01
#define HM_SUBCMD_NACK 0x80
#define HM_SUBCMD_NACK_TARGET_INVALID 0x84
#define IS_HM_MSG_ACK_NACK(hm_msg) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg).cmd, 0x8002))
#define INIT_HM_MSG_ACK_NACK(hm_msg_source,hm_msg_target,ack_nack_subcmd) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 10, (hm_msg_source).msgctr, 0x8002, (hm_msg_source).src); \
	(hm_msg_target).ack_nack.subtype = (ack_nack_subcmd);
#define INIT_HM_MSG_ACK_STATUS(hm_msg_source,hm_msg_target) \
	INIT_HM_MSG_ACK_NACK((hm_msg_source),(hm_msg_target),HM_SUBCMD_ACK_STATUS); \
	(hm_msg_target).len = 14;
typedef struct {
	uint8_t subtype;
	uint8_t channel;
	uint8_t status;
	uint8_t unknown;
	uint8_t rssi;
} hmmsg_ack_nack_t;

#define IS_HM_MSG_DEVICE_INFO(hm_msg) (HM_ISEQUAL_HM_MSG_CMD_TO_UINT16((hm_msg).cmd, 0x8000))
#define INIT_HM_MSG_DEVICE_INFO(hm_msg_target) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 26, ++hm_msgctr, 0x8000, HM_UINT24_FROM_UINT32(0x000000));
#define IS_HM_MSG_DEVICE_INFO_DIRECTED(hm_msg) (HM_ISEQUAL_HM_MSG_CMD_TO_UINT16((hm_msg).cmd, 0xA000))
#define INIT_HM_MSG_DEVICE_INFO_DIRECTED(hm_msg_target,msg_dst) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 26, ++hm_msgctr, 0xA000, (msg_dst));
typedef struct {
	uint8_t firmware;
	hm_uint16_t type;
	char serial_no[10];
	uint8_t class;
	uint8_t peer_channel_a;
	uint8_t peer_channel_b;
	uint8_t unknown;
} hmmsg_device_info_t;

#define IS_HM_MSG_CONFIG_PEER_ADD(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa001) && \
	(hm_msg_target).config_peer_add_remove.subtype == 0x01)
#define INIT_HM_MSG_CONFIG_PEER_ADD(hm_msg_target,msg_dst) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 16, ++hm_msgctr, 0xa001, (msg_dst)); \
	(hm_msg_target).config_peer_add_remove.subtype = 0x01;
#define IS_HM_MSG_CONFIG_PEER_REMOVE(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa001) && \
	(hm_msg_target).config_peer_add_remove.subtype == 0x02)
#define INIT_HM_MSG_CONFIG_PEER_REMOVE(hm_msg_target,msg_dst) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 16, ++hm_msgctr, 0xa001, (msg_dst)); \
	(hm_msg_target).config_peer_add_remove.subtype = 0x02;
typedef struct {
	uint8_t channel;
	uint8_t subtype;
	hm_uint24_t peer_address;
	uint8_t peer_channel_a;
	uint8_t peer_channel_b;
} hmmsg_config_peer_add_remove_t;

#define IS_HM_MSG_CONFIG_PEER_LIST_REQ(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa001) && \
	(hm_msg_target).config_peer_list_req.subtype == 0x03)
#define INIT_HM_MSG_CONFIG_PEER_LIST_REQ(hm_msg_target,msg_dst) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 11, ++hm_msgctr, 0xa001, (msg_dst)); \
	(hm_msg_target).config_peer_list_req.subtype = 0x03;
typedef struct {
	uint8_t channel;
	uint8_t subtype;
} hmmsg_config_peer_list_req_t;

#define IS_HM_MSG_CONFIG_PARAM_REQ(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa001) && \
	(hm_msg_target).config_param_req.subtype == 0x04)
#define INIT_HM_MSG_CONFIG_PARAM_REQ(hm_msg_target,msg_dst) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 16, ++hm_msgctr, 0xa001, (msg_dst)); \
	(hm_msg_target).config_param_req.subtype = 0x04;
typedef struct {
	uint8_t channel;
	uint8_t subtype;
	hm_uint24_t peer_address;
	uint8_t peer_channel;
	uint8_t param_list;
} hmmsg_config_param_req_t;

#define IS_HM_MSG_CONFIG_START(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa001) && \
	(hm_msg_target).config_start.subtype == 0x05)
#define INIT_HM_MSG_CONFIG_START(hm_msg_target,msg_dst) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 16, ++hm_msgctr, 0xa001, (msg_dst)); \
	(hm_msg_target).config_start.subtype = 0x05;
typedef struct {
	uint8_t channel;
	uint8_t subtype;
	hm_uint24_t peer_address;
	uint8_t peer_channel;
	uint8_t param_list;
} hmmsg_config_start_t;

#define IS_HM_MSG_CONFIG_END(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa001) && \
	(hm_msg_target).config_end.subtype == 0x06)
#define INIT_HM_MSG_CONFIG_END(hm_msg_target,msg_dst) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 11, ++hm_msgctr, 0xa001, (msg_dst)); \
	(hm_msg_target).config_end.subtype = 0x06;
typedef struct {
	uint8_t channel;
	uint8_t subtype;
} hmmsg_config_end_t;

#define IS_HM_MSG_CONFIG_UNKNOWN_01(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa001) && \
	(hm_msg_target).config_unknown_01.subtype == 0x07)
#define INIT_HM_MSG_CONFIG_UNKNOWN_01(hm_msg_target,msg_dst,data_count) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 11 + (data_count), ++hm_msgctr, 0xa001, (msg_dst)); \
	(hm_msg_target).config_unknown_01.subtype = 0x07;
typedef struct {
	uint8_t channel;
	uint8_t subtype;
	uint8_t data[(ASKSIN_MSG_REMAINDER_SIZE - 2) / sizeof(uint8_t)];
} hmmsg_config_unknown_01_t;

#define IS_HM_MSG_CONFIG_WRITE_PAIRS(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa001) && \
	(hm_msg_target).config_write_pairs.subtype == 0x08)
#define INIT_HM_MSG_CONFIG_WRITE_PAIRS(hm_msg_target,msg_dst,param_pairs_count) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 11 + (param_pairs_count) * sizeof(hm_parameter_pair_t), ++hm_msgctr, 0xa001, (msg_dst)); \
	(hm_msg_target).config_write_pairs.subtype = 0x08;
#define GET_COUNT_HM_MSG_CONFIG_WRITE_PAIRS(hm_msg_source) \
	(((hm_msg_source).len - 11) / sizeof(hm_parameter_pair_t))
typedef struct {
	uint8_t channel;
	uint8_t subtype;
	hm_parameter_pair_t param_pairs[(ASKSIN_MSG_REMAINDER_SIZE - 2) / sizeof(hm_parameter_pair_t)];
} hmmsg_config_write_pairs_t;

#define IS_HM_MSG_CONFIG_STATUS_REQ(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa001) && \
	(hm_msg_target).config_status_req.subtype == 0x0e)
#define INIT_HM_MSG_CONFIG_STATUS_REQ(hm_msg_target,msg_dst) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 11, ++hm_msgctr, 0xa001, (msg_dst)); \
	(hm_msg_target).config_status_req.subtype = 0x0e;
typedef struct {
	uint8_t channel;
	uint8_t subtype;
} hmmsg_config_status_req_t;

#define IS_HM_MSG_INFO_PEER_LIST(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa010) && \
	(hm_msg_target).info_peer_list.subtype == 0x01)
#define INIT_HM_MSG_INFO_PEER_LIST(hm_msg_target,msg_ctr,msg_dst) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 14, (msg_ctr), 0xa010, (msg_dst)); \
	(hm_msg_target).info_peer_list.subtype = 0x01;
#define GET_LEN_HM_MSG_INFO_PEER_LIST(peers_count) (10 + (peers_count) * (sizeof(hm_uint24_t) + sizeof(uint8_t)))
typedef struct {
	uint8_t subtype;
	hm_peer_t peers[4];
} hmmsg_info_peer_list_t;

#define IS_HM_MSG_INFO_PARAM_RESPONSE_PAIRS(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa010) && \
	(hm_msg_target).info_param_response_pairs.subtype == 0x02)
#define INIT_HM_MSG_INFO_PARAM_RESPONSE_PAIRS(hm_msg_target,msg_ctr,msg_dst,param_pairs_count) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 10 + (param_pairs_count) * sizeof(hm_parameter_pair_t), (msg_ctr), 0xa010, (msg_dst)); \
	(hm_msg_target).info_param_response_pairs.subtype = 0x02;
typedef struct {
	uint8_t subtype;
	hm_parameter_pair_t param_pairs[(ASKSIN_MSG_REMAINDER_SIZE - 1) / sizeof(hm_parameter_pair_t)];
} hmmsg_info_param_response_pairs_t;

#define IS_HM_MSG_INFO_PARAM_RESPONSE_SEQ(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa010) && \
	(hm_msg_target).info_param_response_seq.subtype == 0x03)
#define INIT_HM_MSG_INFO_PARAM_RESPONSE_SEQ(hm_msg_target,msg_ctr,msg_dst,param_count) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 11 + (param_count) * sizeof(uint8_t), (msg_ctr), 0xa010, (msg_dst)); \
	(hm_msg_target).info_param_response_seq.subtype = 0x03;
typedef struct {
	uint8_t subtype;
	uint8_t offset;
	uint8_t params[(ASKSIN_MSG_REMAINDER_SIZE - 2) / sizeof(uint8_t)];
} hmmsg_info_param_response_seq_t;

#define IS_HM_MSG_INFO_ACTUATOR_STATUS(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa010) && \
	(hm_msg_target).info_actuator_status.subtype == 0x06)
#define INIT_HM_MSG_INFO_ACTUATOR_STATUS(hm_msg_target,msg_ctr,msg_dst) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 14, (msg_ctr), 0xa010, (msg_dst)); \
	(hm_msg_target).info_actuator_status.subtype = 0x06;
typedef struct {
	uint8_t subtype;
	uint8_t channel;
	uint8_t status;
	uint8_t unknown;
	uint8_t rssi;
} hmmsg_info_actuator_status_t;

#define IS_HM_MSG_SET(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa011) && \
	(hm_msg_target).set.subtype == 0x02)
#define INIT_HM_MSG_SET(hm_msg_target,msg_dst) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 14, ++hm_msgctr, 0xa011, (msg_dst)); \
	(hm_msg_target).set.subtype = 0x02;
#define INIT_HM_MSG_SET_WITH_DURATION(hm_msg_target,msg_dst) \
	INIT_HM_MSG_SET((hm_msg_target),(msg_dst)); \
	(hm_msg_target).len = 16;
typedef struct {
	uint8_t subtype;
	uint8_t channel;
	uint8_t value;
	uint8_t ramptime;
	uint8_t ontime;
	uint16_t duration;
} hmmsg_set_t;
//TBD: decode/encode HM_TIME16

#define IS_HM_MSG_RESET(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa011) && \
	ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).reset.subtype, 0x0400))
#define INIT_HM_MSG_RESET(hm_msg_target,msg_dst) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 11, ++hm_msgctr, 0xa011, (msg_dst)); \
	(hm_msg_target).reset.subtype = HM_UINT16_FROM_UINT16(0x0400);
typedef struct {
	hm_uint16_t subtype;
} hmmsg_reset_t;

#define IS_HM_MSG_SWITCH(hm_msg_target) (ISEQUAL_HM_UINT16_TO_UINT16((hm_msg_target).cmd, 0xa03e))
#define INIT_HM_MSG_SWITCH(hm_msg_target,msg_dst) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 15, ++hm_msgctr, 0xa03e, (msg_dst));
typedef struct {
	hm_uint24_t dst;
	uint8_t unknown;
	uint8_t channel;
	uint8_t counter;
} hmmsg_switchmsg_t;

#define IS_HM_MSG_REMOTE(hm_msg_target) (HM_ISEQUAL_HM_MSG_CMD_TO_UINT16((hm_msg_target).cmd, 0x8040) \
	|| HM_ISEQUAL_HM_MSG_CMD_TO_UINT16((hm_msg_target).cmd, 0xa040))
#define INIT_HM_MSG_REMOTE(hm_msg_target,msg_dst) \
	INIT_HM_MSG_TRUNK((hm_msg_target), 11, ++hm_msgctr, 0xa040, (msg_dst));
typedef struct {
	uint8_t button:6;
	uint8_t long_press:1;
	uint8_t lowbat:1;
	uint8_t counter;
} hmmsg_remote_t;


typedef struct {
	uint8_t len;
	uint8_t msgctr;
	hm_uint16_t cmd;
	hm_uint24_t src;
	hm_uint24_t dst;
	union {
		uint8_t payload_raw[ASKSIN_MSG_REMAINDER_SIZE];
		hmmsg_ack_nack_t ack_nack;
		hmmsg_device_info_t device_info;
		hmmsg_config_peer_add_remove_t config_peer_add_remove;
		hmmsg_config_peer_list_req_t config_peer_list_req;
		hmmsg_config_param_req_t config_param_req;
		hmmsg_config_start_t config_start;
		hmmsg_config_end_t config_end;
		hmmsg_config_unknown_01_t config_unknown_01;
		hmmsg_config_write_pairs_t config_write_pairs;
		hmmsg_config_status_req_t config_status_req;
		hmmsg_info_peer_list_t info_peer_list;
		hmmsg_info_param_response_pairs_t info_param_response_pairs;
		hmmsg_info_param_response_seq_t info_param_response_seq;
		hmmsg_info_actuator_status_t info_actuator_status;
		hmmsg_set_t set;
		hmmsg_reset_t reset;
		hmmsg_switchmsg_t switchmsg;
		hmmsg_remote_t remote;
	};
} hm_msg_t;


// "decrypt"
static inline void hm_msg_decryt(hm_msg_t *hm_msg_dec, uint8_t *buffer_enc)
{
	uint8_t *buffer_dec = (uint8_t *)hm_msg_dec;
	uint8_t l;
	
	buffer_dec[0] = buffer_enc[0];
	buffer_dec[1] = (~buffer_enc[1]) ^ 0x89;
	for (l = 2; l < buffer_dec[0]; l++)
		buffer_dec[l] = (buffer_enc[l - 1] + 0xdc) ^ buffer_enc[l];
	buffer_dec[l] = buffer_enc[l] ^ buffer_dec[2];
}

// "crypt"
static inline void hm_msg_encrypt(uint8_t *buffer_enc, hm_msg_t *hm_msg_dec)
{
	uint8_t *buffer_dec = (uint8_t *)hm_msg_dec;
	uint8_t l;
	
	buffer_enc[0] = buffer_dec[0];
	buffer_enc[1] = (~buffer_dec[1]) ^ 0x89;
	for (l = 2; l < buffer_dec[0]; l++)
		buffer_enc[l] = (buffer_enc[l - 1] + 0xdc) ^ buffer_dec[l];
	buffer_enc[l] = buffer_dec[l] ^ buffer_dec[2];
}


#endif /* HM_MESSAGES_H_ */