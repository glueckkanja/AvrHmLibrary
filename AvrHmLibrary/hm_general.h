/*
 * hm.h
 *
 * Created: 11.06.2012 14:17:24
 *  Author: fstorm
 */ 


#ifndef HM_GENERAL_H_
#define HM_GENERAL_H_


#include <avr/eeprom.h>


extern uint24hm_t my_addr;

extern uint8_t hm_frm_cnt;

extern hm_frame_t hm_frm_in;
#define HM_REPLY_IN_FRM_OUT 0xfe
#define HM_REPLY_NONE 0xff
extern uint8_t hm_frm_in_ack_subtype;

extern hm_frame_t hm_frm_out;

extern volatile bool hm_do_reset;

extern volatile bool hm_do_start_pairing;
extern volatile bool hm_do_stop_pairing;
extern bool hm_is_waiting_for_pairing;

extern bool hm_is_in_mode_config;
extern uint8_t hm_mode_config_channel;
extern uint8_t hm_mode_config_peering_id;
extern uint8_t hm_mode_config_param_list;

extern bool hm_is_frame_from_ccu;


void hm_init();
void hm_task();
void hm_send();

void hm_pairing_process_start(bool send_device_info);
void hm_pairing_process_end();
void hm_send_device_info(uint24hm_t addr_dst);

void hm_packet_decryt(hm_frame_t *hm_packet_dec, uint8_t *buffer_enc);
void hm_packet_encrypt(uint8_t *buffer_enc, hm_frame_t *hm_packet_dec);

#endif /* HM_GENERAL_H_ */