/*
 * hm.c
 *
 * Created: 11.06.2012 13:36:29
 *  Author: fstorm
 */ 

#include "hm_main.h"


uint8_t packet_enc[MAX_ASKSIN_MSG];
uint8_t rssi, lqi;


uint24hm_t hm_my_addr;

uint8_t hm_frm_ctr;

hm_frame_t hm_frm_in;
uint8_t hm_frm_in_ack_subtype;

hm_frame_t hm_frm_out;

bool hm_is_message_from_ccu;
bool hm_is_waiting_for_pairing;

bool hm_is_in_mode_config;
uint8_t hm_mode_config_channel;
int8_t hm_mode_config_peer_id;
uint8_t hm_mode_config_param_list;


void hm_init()
{
	hm_my_addr.value = HM_MY_ADDR;
	
	if (!hm_config_load())
		hm_config_reset();
	
	hm_rf_init();
}

void hm_task()
{
	if (!hm_rf_receive(packet_enc, &rssi, &lqi))
		return;	// no message received
		
	// "decrypt"
	hm_msg_decryt(&hm_frm_in, packet_enc);
	
	debug_dump((uint8_t*)&hm_frm_in, hm_frm_in.len + 1, "R: ");
	
	
	hm_system_frm_handler();

	if (hm_frm_in_ack_subtype != HM_REPLY_NONE &&
		(hm_frm_in.flags.bidi || hm_frm_in_ack_subtype == HM_REPLY_IN_FRM_OUT))
	{
		if (hm_frm_in_ack_subtype != HM_REPLY_IN_FRM_OUT)
		{
			HM_FRM_ACK_NACK_NEW(hm_frm_in, hm_frm_out, hm_frm_in_ack_subtype);
		}			
		hm_send();
	}
}

void hm_send()
{
	debug_dump((uint8_t*)&hm_frm_out, hm_frm_out.len + 1, "S: ");

	// "crypt"
	hm_msg_encrypt(packet_enc, &hm_frm_out);
	
	hm_rf_send_packet(packet_enc);
}

void hm_send_device_info(uint24hm_t addr_dst)
{
	HM_FRM_DEVICE_INFO_NEW(hm_frm_out, addr_dst);
	if (addr_dst.value == 0x000000)
	{
		hm_frm_out.flags.bidi = 0;
		hm_frm_out.flags.bcast = 1;
	}		

	hm_frm_out.device_info.firmware = HM_DEVINFO_FIRMWARE;
	hm_frm_out.device_info.type.value = HM_DEVINFO_TYPE;
	memcpy_P(hm_frm_out.device_info.serial_no, PSTR(HM_DEVINFO_SERIAL_NO), 10);
	hm_frm_out.device_info.class = HM_DEVINFO_CLASS;
	hm_frm_out.device_info.peer_channel_a = HM_DEVINFO_PEER_CHANNEL_A;
	hm_frm_out.device_info.peer_channel_b = HM_DEVINFO_PEER_CHANNEL_B;
	
	hm_send();
	
	hm_is_waiting_for_pairing = true;
}


// "decrypt"
void hm_msg_decryt(hm_frame_t *hm_msg_dec, uint8_t *buffer_enc)
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
void hm_msg_encrypt(uint8_t *buffer_enc, hm_frame_t *hm_msg_dec)
{
	uint8_t *buffer_dec = (uint8_t *)hm_msg_dec;
	uint8_t l;
	
	buffer_enc[0] = buffer_dec[0];
	buffer_enc[1] = (~buffer_dec[1]) ^ 0x89;
	for (l = 2; l < buffer_dec[0]; l++)
	buffer_enc[l] = (buffer_enc[l - 1] + 0xdc) ^ buffer_dec[l];
	buffer_enc[l] = buffer_dec[l] ^ buffer_dec[2];
}
