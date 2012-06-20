/*
 * hm_frames_system.h
 *
 * Created: 11.06.2012 21:05:53
 *  Author: fstorm
 */ 


#ifndef HM_FRAMES_SYSTEM_H_
#define HM_FRAMES_SYSTEM_H_


#define HM_FRM_NEW(frm_buf_tgt, frm_len, frm_ctr, frm_type, frm_dst_addr) \
	memset(&(frm_buf_tgt), 0, sizeof((frm_buf_tgt))); \
	(frm_buf_tgt).len = (frm_len); \
	(frm_buf_tgt).ctr = (frm_ctr); \
	(frm_buf_tgt).flags.rpten = 1; \
	(frm_buf_tgt).flags.bidi = 1; \
	(frm_buf_tgt).type = (frm_type); \
	(frm_buf_tgt).addr_src = hm_my_addr; \
	(frm_buf_tgt).addr_dst = (frm_dst_addr);
//(frm_buf_tgt) = { (frm_len), (frm_ctr), 0xa0, (frm_type), hm_my_addr, frm_dst_addr };


#define HM_FRM_DEVICE_INFO_IS(frm) ((frm).type == 0x00)
#define HM_FRM_DEVICE_INFO_NEW(frm_buf_tgt, frm_dst_addr) \
	HM_FRM_NEW((frm_buf_tgt), 26, ++hm_frm_ctr, 0x00, (frm_dst_addr));
typedef struct {
	uint8_t firmware;
	uint16hm_t type;
	char serial_no[10];
	uint8_t class;
	uint8_t peer_channel_a;
	uint8_t peer_channel_b;
	uint8_t unknown;
} hm_frm_device_info_t;


#define HM_FRM_CONFIG_PEER_ADD_IS(frm) ((frm).type == 0x01 && (frm).config_peer_add_remove.subtype == 0x01)
#define HM_FRM_CONFIG_PEER_ADD_NEW(frm_buf_tgt, frm_dst_addr) \
	HM_FRM_NEW((frm_buf_tgt), 16, ++hm_frm_ctr, 0x01, (frm_dst_addr)); \
	(frm_buf_tgt).config_peer_add_remove.subtype = 0x01;
#define HM_FRM_CONFIG_PEER_REMOVE_IS(frm) ((frm).type == 0x01 && (frm).config_peer_add_remove.subtype == 0x02)
#define HM_FRM_CONFIG_PEER_REMOVE_NEW(frm_buf_tgt, frm_dst_addr) \
	HM_FRM_NEW((frm_buf_tgt), 16, ++hm_frm_ctr, 0x01, (frm_dst_addr)); \
	(frm_buf_tgt).config_peer_add_remove.subtype = 0x02;
typedef struct {
	uint8_t channel;
	uint8_t subtype;
	uint24hm_t peer_address;
	uint8_t peer_channel_a;
	uint8_t peer_channel_b;
} hm_frm_config_peer_add_remove_t;

#define HM_FRM_CONFIG_PEER_LIST_REQ_IS(frm) ((frm).type == 0x01 && (frm).config_peer_list_req.subtype == 0x03)
#define HM_FRM_CONFIG_PEER_LIST_REQ_NEW(frm_buf_tgt, frm_dst_addr) \
	HM_FRM_NEW((frm_buf_tgt), 11, ++hm_frm_ctr, 0x01, (frm_dst_addr)); \
	(frm_buf_tgt).config_peer_list_req.subtype = 0x03;
typedef struct {
	uint8_t channel;
	uint8_t subtype;
} hm_frm_config_peer_list_req_t;

#define HM_FRM_CONFIG_PARAM_REQ_IS(frm) ((frm).type == 0x01 && (frm).config_param_req.subtype == 0x04)
#define HM_FRM_CONFIG_PARAM_REQ_NEW(frm_buf_tgt, frm_dst_addr) \
	HM_FRM_NEW((frm_buf_tgt), 16, ++hm_frm_ctr, 01, (frm_dst_addr)); \
	(frm_buf_tgt).config_param_req.subtype = 0x04;
typedef struct {
	uint8_t channel;
	uint8_t subtype;
	uint24hm_t peer_address;
	uint8_t peer_channel;
	uint8_t param_list;
} hm_frm_config_param_req_t;

#define HM_FRM_CONFIG_START_IS(frm) ((frm).type == 0x01 && (frm).config_start.subtype == 0x05)
#define HM_FRM_CONFIG_START_NEW(frm_buf_tgt, frm_dst_addr) \
	HM_FRM_NEW((frm_buf_tgt), 16, ++hm_frm_ctr, 0x01, (frm_dst_addr)); \
	(frm_buf_tgt).config_start.subtype = 0x05;
typedef struct {
	uint8_t channel;
	uint8_t subtype;
	uint24hm_t peer_address;
	uint8_t peer_channel;
	uint8_t param_list;
} hm_frm_config_start_t;

#define HM_FRM_CONFIG_END_IS(frm) ((frm).type == 0x01 && (frm).config_end.subtype == 0x06)
#define HM_FRM_CONFIG_END_NEW(frm_buf_tgt, frm_dst_addr) \
	HM_FRM_NEW((frm_buf_tgt), 11, ++hm_frm_ctr, 0x01, (frm_dst_addr)); \
	(frm_buf_tgt).config_end.subtype = 0x06;
typedef struct {
	uint8_t channel;
	uint8_t subtype;
} hm_frm_config_end_t;

#define HM_FRM_CONFIG_UNKNOWN_01_IS(frm) ((frm).type == 0x01 && (frm).config_unknown_01.subtype == 0x07)
#define HM_FRM_CONFIG_UNKNOWN_01_NEW(frm_buf_tgt, frm_dst_addr, data_count) \
	HM_FRM_NEW((frm_buf_tgt), 11 + (data_count), ++hm_frm_ctr, 0x01, (frm_dst_addr)); \
	(frm_buf_tgt).config_unknown_01.subtype = 0x07;
typedef struct {
	uint8_t channel;
	uint8_t subtype;
	uint8_t data[(HM_FRM_PAYLOAD_SIZE - 2) / sizeof(uint8_t)];
} hm_frm_config_unknown_01_t;

#define HM_FRM_CONFIG_WRITE_INDEX_IS(frm) ((frm).type == 0x01 && (frm).config_write_index.subtype == 0x08)
#define HM_FRM_CONFIG_WRITE_INDEX_GET_COUNT(frm) (((frm).len - 11) / sizeof(hm_parameter_pair_t))
#define HM_FRM_CONFIG_WRITE_INDEX_NEW(frm_buf_tgt, frm_dst_addr, param_pairs_count) \
	HM_FRM_NEW((frm_buf_tgt), 11 + (param_pairs_count) * sizeof(hm_parameter_pair_t), ++hm_frm_ctr, 0x01, (frm_dst_addr)); \
	(frm_buf_tgt).config_write_index.subtype = 0x08;
typedef struct {
	uint8_t channel;
	uint8_t subtype;
	hm_parameter_pair_t param_pairs[(HM_FRM_PAYLOAD_SIZE - 2) / sizeof(hm_parameter_pair_t)];
} hm_frm_config_write_index_t;

#define HM_FRM_CONFIG_STATUS_REQ_IS(frm) ((frm).type == 0x01 && (frm).config_status_req.subtype == 0x0e)
#define HM_FRM_CONFIG_STATUS_REQ_NEW(frm_buf_tgt, frm_dst_addr) \
	HM_FRM_NEW((frm_buf_tgt), 11, ++hm_frm_ctr, 0x01, (frm_dst_addr)); \
	(frm_buf_tgt).config_status_req.subtype = 0x0e;
typedef struct {
	uint8_t channel;
	uint8_t subtype;
} hm_frm_config_status_req_t;


#define HM_FRM_ACK_NACK_SUBTYPE_ACK 0x00
#define HM_FRM_ACK_NACK_SUBTYPE_ACK_STATUS 0x01
#define HM_FRM_ACK_NACK_SUBTYPE_AES_REQUEST 0x04
#define HM_FRM_ACK_NACK_SUBTYPE_NACK 0x80
#define HM_FRM_ACK_NACK_SUBTYPE_NACK_TARGET_INVALID 0x84
#define HM_FRM_ACK_NACK_IS(frm) ((frm).type == 0x02)
#define HM_FRM_ACK_NACK_NEW(frm_buf_src, frm_buf_tgt, ack_nack_subtype) \
	HM_FRM_NEW((frm_buf_tgt), (ack_nack_subtype) == HM_FRM_ACK_NACK_SUBTYPE_ACK_STATUS ? 14 : 10, (frm_buf_src).ctr, 0x02, (frm_buf_src).addr_src); \
	(frm_buf_tgt).ack_nack.subtype = (ack_nack_subtype); \
	(frm_buf_tgt).flags.bidi = 0;
typedef struct {
	uint8_t subtype;
	uint8_t channel;
	uint8_t status;
	uint8_t unknown;
	uint8_t hm_rssi;
} hm_frm_ack_nack_t;


#define HM_FRM_INFO_PEER_LIST_IS(frm) ((frm).type == 0x10 && (frm).info_peer_list.subtype == 0x01)
#define HM_FRM_INFO_PEER_LIST_NEW(frm_buf_tgt, frm_ctr, frm_dst_addr) \
	HM_FRM_NEW((frm_buf_tgt), 14, (frm_ctr), 0x10, (frm_dst_addr)); \
	(frm_buf_tgt).info_peer_list.subtype = 0x01;
#define HM_FRM_INFO_PEER_LIST_GET_LEN(peers_count) (10 + (peers_count) * (sizeof(hm_peer_t)))
typedef struct {
	uint8_t subtype;
	hm_peer_t peers[4];
} hm_frm_info_peer_list_t;

#define HM_FRM_INFO_PARAM_RESPONSE_PAIRS_IS(frm) ((frm).type == 0x10 && (frm).info_param_response_pairs.subtype == 0x02)
#define HM_FRM_INFO_PARAM_RESPONSE_PAIRS_NEW(frm_buf_tgt, frm_ctr, frm_dst_addr, param_pairs_count) \
	HM_FRM_NEW((frm_buf_tgt), 10 + (param_pairs_count) * sizeof(hm_parameter_pair_t), (frm_ctr), 0x10, (frm_dst_addr)); \
	(frm_buf_tgt).info_param_response_pairs.subtype = 0x02;
typedef struct {
	uint8_t subtype;
	hm_parameter_pair_t param_pairs[(HM_FRM_PAYLOAD_SIZE - 1) / sizeof(hm_parameter_pair_t)];
} hm_frm_info_param_response_pairs_t;

#define HM_FRM_INFO_PARAM_RESPONSE_SEQ_IS(frm) ((frm).type == 0x10 && (frm).info_param_response_seq.subtype == 0x03)
#define HM_FRM_INFO_PARAM_RESPONSE_SEQ_NEW(frm_buf_tgt, frm_ctr, frm_dst_addr, param_count) \
	HM_FRM_NEW((frm_buf_tgt), 11 + (param_count) * sizeof(uint8_t), (frm_ctr), 0x10, (frm_dst_addr)); \
	(frm_buf_tgt).info_param_response_seq.subtype = 0x03;
typedef struct {
	uint8_t subtype;
	uint8_t offset;
	uint8_t params[(HM_FRM_PAYLOAD_SIZE - 2) / sizeof(uint8_t)];
} hm_frm_info_param_response_seq_t;

#define HM_FRM_RESET_IS(frm) ((frm).type == 0x11 && (frm).reset.subtype == 0x04 && (frm).reset.unknown00 == 0x00)
#define HM_FRM_RESET_NEW(frm_buf_tgt, frm_dst_addr) \
	HM_FRM_NEW((frm_buf_tgt), 11, ++hm_frm_ctr, 0x11, (frm_dst_addr)); \
	(frm_buf_tgt).reset.subtype = 0x04;
typedef struct {
	uint8_t subtype;
	uint8_t unknown00;
} hm_frm_reset_t;

#define HM_FRM_SWITCH_IS(frm) ((frm).type == 0x3e)
#define HM_FRM_SWITCH_NEW(frm_buf_tgt, frm_dst_addr) \
	HM_FRM_NEW((frm_buf_tgt), 15, ++hm_frm_ctr, 0x3e, (frm_dst_addr));
typedef struct {
	uint24hm_t addr_dst;
	uint8_t unknown;
	uint8_t channel;
	uint8_t counter;
} hm_frm_switchfrm_t;

#define HM_FRM_REMOTE_IS(frm) ((frm).type == 0x40)
#define HM_FRM_REMOTE_NEW(frm_buf_tgt, frm_dst_addr) \
	HM_FRM_NEW((frm_buf_tgt), 11, ++hm_frm_ctr, 0x40, (frm_dst_addr));
typedef struct {
	uint8_t button:6;
	uint8_t long_press:1;
	uint8_t lowbat:1;
	uint8_t counter;
} hm_frm_remote_t;


typedef struct {
	uint8_t len;
	uint8_t ctr;
	struct {
		uint8_t wakeup:1;
		uint8_t wakemeup:1;
		uint8_t bcast:1;
		uint8_t bit3:1;
		uint8_t burst:1;
		uint8_t bidi:1;
		uint8_t rpted:1;
		uint8_t rpten:1;
	} flags;
	uint8_t type;		
	uint24hm_t addr_src;
	uint24hm_t addr_dst;
	union {
		uint8_t payload_raw[HM_FRM_PAYLOAD_SIZE];
		hm_frm_ack_nack_t ack_nack;
		hm_frm_device_info_t device_info;
		hm_frm_config_peer_add_remove_t config_peer_add_remove;
		hm_frm_config_param_req_t config_param_req;
		hm_frm_config_peer_list_req_t config_peer_list_req;
		hm_frm_config_start_t config_start;
		hm_frm_config_end_t config_end;
		hm_frm_config_unknown_01_t config_unknown_01;
		hm_frm_config_write_index_t config_write_index;
		hm_frm_config_status_req_t config_status_req;
		hm_frm_info_peer_list_t info_peer_list;
		hm_frm_info_param_response_pairs_t info_param_response_pairs;
		hm_frm_info_param_response_seq_t info_param_response_seq;
		hm_frm_reset_t reset;
		hm_frm_switchfrm_t switchfrm;
		hm_frm_remote_t remote;
		HM_FRAMES_DEVICE
	};
} __attribute__ ((packed)) hm_frame_t;


void hm_system_frame_handler();


#endif /* HM_FRAMES_SYSTEM_H_ */