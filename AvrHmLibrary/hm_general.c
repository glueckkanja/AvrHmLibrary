/*
 * hm_general.c
 *
 * Created: 11.06.2012 13:36:29
 *  Author: fstorm
 */ 

#include "hm_main.h"


uint8_t hm_packet_enc[MAX_ASKSIN_MSG];
uint8_t hm_rssi, hm_lqi;


uint8_t hm_frm_cnt;

hm_frame_t hm_frm_in;
uint8_t hm_frm_in_ack_subtype;

hm_frame_t hm_frm_out;

volatile bool hm_do_reset;

volatile bool hm_do_start_pairing;
volatile bool hm_do_stop_pairing;
bool hm_is_waiting_for_pairing;
uint8_t hm_waiting_for_pairing_start_secs;

bool hm_is_in_mode_config;
uint8_t hm_mode_config_channel;
uint8_t hm_mode_config_peering_id;
uint8_t hm_mode_config_param_list;

bool hm_is_frame_from_ccu;


void hm_init()
{
	eeprom_read_block(&hm_ee_contents, &hm_ee_contents_E, sizeof(hm_ee_contents));
	if (!hm_eeprom_check())
	{
		// no address etc. - unrecoverable error, blink forever...
		while (1)
		{
			LED_ON(); _delay_ms(1000); LED_OFF(); _delay_ms(150); 
			LED_ON(); _delay_ms(150); LED_OFF(); _delay_ms(150); 
			LED_ON(); _delay_ms(150); LED_OFF(); _delay_ms(1000); 
		}
	}					
	
	hm_do_reset = false;
	hm_do_start_pairing = false;
	hm_is_waiting_for_pairing = false;
	hm_is_in_mode_config = false;
	
	hm_config_init();
	
	hm_timer_init();
	hm_buttons_init();
	hm_rf_init();
	
#ifdef HM_INIT_TASK_INCLUDE
	HM_INIT_TASK_INCLUDE();
#endif
}

void hm_task()
{
	if (hm_do_reset)
	{
		hm_do_reset = false;
		hm_config_reset();
		
		HM_RESET_DEVICE();
	}

	if (hm_do_start_pairing)
	{
		hm_do_start_pairing = false;
		hm_pairing_process_start(true);
	}
	if (hm_do_stop_pairing ||
		(hm_is_waiting_for_pairing && hm_timer_is_expired_secs(hm_waiting_for_pairing_start_secs, 20)))
	{
		hm_do_stop_pairing = false;
		hm_pairing_process_end();
	}

	if (!hm_rf_receive(hm_packet_enc, &hm_rssi, &hm_lqi))
		return;	// no frame received
		
	// "decrypt"
	hm_packet_decryt(&hm_frm_in, hm_packet_enc);
	
	//debug_dump((uint8_t*)&hm_frm_in, hm_frm_in.len + 1, "R: ");
	
	
	hm_system_frame_handler();

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
	//debug_dump((uint8_t*)&hm_frm_out, hm_frm_out.len + 1, "S: ");

	// "crypt"
	hm_packet_encrypt(hm_packet_enc, &hm_frm_out);
	
	hm_rf_send_packet(hm_packet_enc);
}

void hm_pairing_process_start(bool send_device_info)
{
	if (send_device_info)
		hm_send_device_info((uint24hm_t){ 0x000000 });
	
	hm_waiting_for_pairing_start_secs = hm_timer_secs;
	hm_is_waiting_for_pairing = true;
	HM_SIGNAL_WAITING_FOR_PAIRING_ON();
}

void hm_pairing_process_end()
{
	HM_SIGNAL_WAITING_FOR_PAIRING_OFF();
	hm_is_waiting_for_pairing = false;
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
	memcpy(hm_frm_out.device_info.serial_no, hm_ee_contents.my_serial, sizeof(hm_frm_out.device_info.serial_no));
	hm_frm_out.device_info.class = HM_DEVINFO_CLASS;
	hm_frm_out.device_info.channel_info = HM_DEVINFO_CHANNEL_INFO;
	hm_frm_out.device_info.channel_a = HM_DEVINFO_CHANNEL_A;
	hm_frm_out.device_info.channel_b = HM_DEVINFO_CHANNEL_B;
	
	hm_send();
}


// "decrypt"
void hm_packet_decryt(hm_frame_t *hm_packet_dec, uint8_t *buffer_enc)
{
	uint8_t *buffer_dec = (uint8_t *)hm_packet_dec;
	uint8_t l;
	
	buffer_dec[0] = buffer_enc[0];
	buffer_dec[1] = (~buffer_enc[1]) ^ 0x89;
	for (l = 2; l < buffer_dec[0]; l++)
	buffer_dec[l] = (buffer_enc[l - 1] + 0xdc) ^ buffer_enc[l];
	buffer_dec[l] = buffer_enc[l] ^ buffer_dec[2];
}

// "crypt"
void hm_packet_encrypt(uint8_t *buffer_enc, hm_frame_t *hm_packet_dec)
{
	uint8_t *buffer_dec = (uint8_t *)hm_packet_dec;
	uint8_t l;
	
	buffer_enc[0] = buffer_dec[0];
	buffer_enc[1] = (~buffer_dec[1]) ^ 0x89;
	for (l = 2; l < buffer_dec[0]; l++)
	buffer_enc[l] = (buffer_enc[l - 1] + 0xdc) ^ buffer_dec[l];
	buffer_enc[l] = buffer_dec[l] ^ buffer_dec[2];
}
