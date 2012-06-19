/*
 * hm.h
 *
 * Created: 11.06.2012 14:17:24
 *  Author: fstorm
 */ 


#ifndef HM_H_
#define HM_H_


extern uint24hm_t hm_my_addr;

extern uint8_t hm_frm_ctr;

extern hm_frame_t hm_frm_in;
#define HM_REPLY_IN_FRM_OUT 0xfe
#define HM_REPLY_NONE 0xff
extern uint8_t hm_frm_in_ack_subtype;

extern hm_frame_t hm_frm_out;

extern bool hm_is_message_from_ccu;
extern bool hm_is_waiting_for_pairing;

extern bool hm_is_in_mode_config;
extern uint8_t hm_mode_config_channel;
extern int8_t hm_mode_config_peer_id;
extern uint8_t hm_mode_config_param_list;


void hm_init();
void hm_task();
void hm_send();

void hm_send_device_info(uint24hm_t addr_dst);

void hm_msg_decryt(hm_frame_t *hm_msg_dec, uint8_t *buffer_enc);
void hm_msg_encrypt(uint8_t *buffer_enc, hm_frame_t *hm_msg_dec);

#endif /* HM_H_ */