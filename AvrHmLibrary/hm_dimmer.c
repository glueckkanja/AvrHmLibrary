/*
 * hm_dimmer.c
 *
 * Created: 11.06.2012 13:36:29
 *  Author: fstorm
 */ 

#include "hm_main.h"

#include <avr/pgmspace.h>
#include <util/delay.h>


hm_ee_contents_t EEMEM hm_ee_contents_E = {
	.ee_cookie = "GK",
	.ee_version = 0x01,
	.my_addr = { .value = HM_MY_ADDR },
	.my_serial = HM_MY_SERIAL
};
hm_ee_contents_t hm_ee_contents;
hm_param_bank_t hm_config_param_banks[HM_CONFIG_PARAM_BANKS_COUNT];


const hm_parameter_pair_t PROGMEM hm_params_device_default_P[5] = {{0x02, 0x00}, {0x0A, 0x00}, {0x0B, 0x00}, {0x0C, 0x00}, {0x15, 0xFF}};
const hm_parameter_pair_t PROGMEM hm_params_channel_default_P[5] = {{0x30, 0x06}, {0x32, 0x50}, {0x33, 0x64}, {0x34, 0x4B}, {0x35, 0x50}};
const uint8_t PROGMEM hm_params_peering_default_peer_channel_a_short_0_P[13] = {0x00, 0x00, 0x00, 0x32, 0x64, 0x00, 0xFF, 0x00, 0xFF, 0x01, 0x12, 0x22, 0x23};
const uint8_t PROGMEM hm_params_peering_default_peer_channel_a_short_1_P[13] = {0x20, 0x00, 0x14, 0xC8, 0x0A, 0x05, 0x05, 0x00, 0xC8, 0x0A, 0x0A, 0x04, 0x04};
const uint8_t PROGMEM hm_params_peering_default_peer_channel_a_long_0_P[13] = {0x00, 0x00, 0x00, 0x32, 0x64, 0x00, 0xFF, 0x00, 0xFF, 0x24, 0x12, 0x22, 0x23};
const uint8_t PROGMEM hm_params_peering_default_peer_channel_a_long_1_P[13] = {0x20, 0x00, 0x14, 0xC8, 0x0A, 0x05, 0x05, 0x00, 0xC8, 0x0A, 0x0A, 0x04, 0x04};
const uint8_t PROGMEM hm_params_peering_default_peer_channel_b_short_0_P[13] = {0x00, 0x00, 0x00, 0x32, 0x64, 0x00, 0xFF, 0x00, 0xFF, 0x01, 0x44, 0x54, 0x64};
const uint8_t PROGMEM hm_params_peering_default_peer_channel_b_short_1_P[13] = {0x20, 0x00, 0x14, 0xC8, 0x0A, 0x05, 0x05, 0x00, 0xC8, 0x0A, 0x0A, 0x04, 0x04};
const uint8_t PROGMEM hm_params_peering_default_peer_channel_b_long_0_P[13] = {0x00, 0x00, 0x00, 0x32, 0x64, 0x00, 0x0A, 0x00, 0xFF, 0xA5, 0x44, 0x54, 0x64};
const uint8_t PROGMEM hm_params_peering_default_peer_channel_b_long_1_P[13] = {0x20, 0x00, 0x14, 0xC8, 0x0A, 0x05, 0x05, 0x00, 0xC8, 0x0A, 0x0A, 0x04, 0x04};

bool hm_eeprom_check()
{
	return (memcmp(hm_ee_contents.ee_cookie, "GK", sizeof(hm_ee_contents.ee_cookie)) == 0 
		&& hm_ee_contents.ee_version == 0x01);
}

void hm_config_init()
{
	hm_config_param_banks[0].peering_id = 0xc0;	// params for device
	hm_config_param_banks[0].param_bank_pairs.pair_count = HM_PARAM_PAIRS_PER_DEVICE;
	hm_config_param_banks[0].param_bank_pairs.param_pairs = hm_ee_contents.config.params_device;
	hm_config_param_banks[0].p_eeprom_address = hm_ee_contents_E.config.params_device;
	
	for (uint8_t i = 0; i < HM_CHANNEL_COUNT; i++)	// params per channel
	{
		uint8_t j = 1 + i;
		hm_config_param_banks[j].peering_id = 0xc0 + j;
		hm_config_param_banks[j].param_list = 1;
		hm_config_param_banks[j].param_bank_pairs.pair_count = HM_PARAM_PAIRS_PER_CHANNEL;
		hm_config_param_banks[j].param_bank_pairs.param_pairs = hm_ee_contents.config.params_channels[i];
		hm_config_param_banks[j].p_eeprom_address = hm_ee_contents_E.config.params_channels[i];
	}
	
	for (uint8_t i = 0; i < HM_PEERINGS_MAX; i++)	// params per peering
	{
		uint8_t j = 1 + HM_CHANNEL_COUNT + i * HM_PARAM_BANKS_PER_PEERING;
		hm_config_param_banks[j].peering_id = i;
		hm_config_param_banks[j].param_list = 3;
		hm_config_param_banks[j].is_sequence = true;
		hm_config_param_banks[j].param_bank_seq.offset = 0x01;
		hm_config_param_banks[j].param_bank_seq.param_count = HM_PARAM_BYTES_PER_PEERING_PER_BANK;
		hm_config_param_banks[j].param_bank_seq.params = hm_ee_contents.config.params_peerings[i][0];
		hm_config_param_banks[j].p_eeprom_address = hm_ee_contents_E.config.params_peerings[i][0];

		j++;
		hm_config_param_banks[j].peering_id = i;
		hm_config_param_banks[j].param_list = 3;
		hm_config_param_banks[j].is_sequence = true;
		hm_config_param_banks[j].param_bank_seq.offset = 0x0e;
		hm_config_param_banks[j].param_bank_seq.param_count = HM_PARAM_BYTES_PER_PEERING_PER_BANK;
		hm_config_param_banks[j].param_bank_seq.params = hm_ee_contents.config.params_peerings[i][1];
		hm_config_param_banks[j].p_eeprom_address = hm_ee_contents_E.config.params_peerings[i][1];

		j++;
		hm_config_param_banks[j].peering_id = i;
		hm_config_param_banks[j].param_list = 3;
		hm_config_param_banks[j].is_sequence = true;
		hm_config_param_banks[j].param_bank_seq.offset = 0x81;
		hm_config_param_banks[j].param_bank_seq.param_count = HM_PARAM_BYTES_PER_PEERING_PER_BANK;
		hm_config_param_banks[j].param_bank_seq.params = hm_ee_contents.config.params_peerings[i][2];
		hm_config_param_banks[j].p_eeprom_address = hm_ee_contents_E.config.params_peerings[i][2];

		j++;
		hm_config_param_banks[j].peering_id = i;
		hm_config_param_banks[j].param_list = 3;
		hm_config_param_banks[j].is_sequence = true;
		hm_config_param_banks[j].param_bank_seq.offset = 0x8e;
		hm_config_param_banks[j].param_bank_seq.param_count = HM_PARAM_BYTES_PER_PEERING_PER_BANK;
		hm_config_param_banks[j].param_bank_seq.params = hm_ee_contents.config.params_peerings[i][3];
		hm_config_param_banks[j].p_eeprom_address = hm_ee_contents_E.config.params_peerings[i][3];
	}
	
	// check if HM structures are valid
	if (hm_ee_contents.config.params_device[0].index != 0x02 ||
		hm_ee_contents.config.params_device[1].index != 0x0A ||
		hm_ee_contents.config.params_device[2].index != 0x0B ||
		hm_ee_contents.config.params_device[3].index != 0x0C)
		{
			hm_config_reset();
		}			
}

void hm_config_reset()
{
	// clear complete config
	memset(&hm_ee_contents.config, 0, sizeof(hm_ee_contents.config));
	
	// reset params
	memcpy_P(hm_ee_contents.config.params_device, hm_params_device_default_P, sizeof(hm_params_device_default_P));
	for (uint8_t i = 0; i < HM_CHANNEL_COUNT; i++)
		memcpy_P(hm_ee_contents.config.params_channels[i], hm_params_channel_default_P, sizeof(hm_params_channel_default_P));
	
	// mark all banks as dirty
	for (uint8_t i = 0; i < HM_CONFIG_PARAM_BANKS_COUNT; i++)
		hm_config_param_banks[i].is_dirty = true;
	
	// default peerings of local buttons with local channel
	hm_init_peering(0x01, &hm_ee_contents.my_addr, 0x01);
	hm_init_peering(0x01, &hm_ee_contents.my_addr, 0x02);
}

void hm_peering_reset(uint8_t peering_id, uint8_t peer_channel)
{
	uint8_t pb_id = 1 + HM_CHANNEL_COUNT + peering_id * HM_PARAM_BANKS_PER_PEERING;
	
	if (peer_channel & 1)
	{
		memcpy_P(hm_config_param_banks[pb_id].param_bank_seq.params, hm_params_peering_default_peer_channel_b_short_0_P, sizeof(hm_params_peering_default_peer_channel_b_short_0_P));
		hm_config_param_banks[pb_id++].is_dirty = true;
		memcpy_P(hm_config_param_banks[pb_id].param_bank_seq.params, hm_params_peering_default_peer_channel_b_short_1_P, sizeof(hm_params_peering_default_peer_channel_b_short_1_P));
		hm_config_param_banks[pb_id++].is_dirty = true;
		memcpy_P(hm_config_param_banks[pb_id].param_bank_seq.params, hm_params_peering_default_peer_channel_b_long_0_P, sizeof(hm_params_peering_default_peer_channel_b_long_0_P));
		hm_config_param_banks[pb_id++].is_dirty = true;
		memcpy_P(hm_config_param_banks[pb_id].param_bank_seq.params, hm_params_peering_default_peer_channel_b_long_1_P, sizeof(hm_params_peering_default_peer_channel_b_long_1_P));
		hm_config_param_banks[pb_id++].is_dirty = true;
	}
	else
	{
		memcpy_P(hm_config_param_banks[pb_id].param_bank_seq.params, hm_params_peering_default_peer_channel_a_short_0_P, sizeof(hm_params_peering_default_peer_channel_a_short_0_P));
		hm_config_param_banks[pb_id++].is_dirty = true;
		memcpy_P(hm_config_param_banks[pb_id].param_bank_seq.params, hm_params_peering_default_peer_channel_a_short_1_P, sizeof(hm_params_peering_default_peer_channel_a_short_1_P));
		hm_config_param_banks[pb_id++].is_dirty = true;
		memcpy_P(hm_config_param_banks[pb_id].param_bank_seq.params, hm_params_peering_default_peer_channel_a_long_0_P, sizeof(hm_params_peering_default_peer_channel_a_long_0_P));
		hm_config_param_banks[pb_id++].is_dirty = true;
		memcpy_P(hm_config_param_banks[pb_id].param_bank_seq.params, hm_params_peering_default_peer_channel_a_long_1_P, sizeof(hm_params_peering_default_peer_channel_a_long_1_P));
		hm_config_param_banks[pb_id++].is_dirty = true;
	}	
}
	
void hm_dimmer_frame_handler()
{
	if (HM_FRM_DIMMER_SET_IS(hm_frm_in))
	{
		HM_DEV_DIMMER_SET_LEVEL(hm_frm_in.dimmer_set.level / 2);
		
		HM_FRM_ACK_NACK_NEW(hm_frm_in, hm_frm_out, HM_FRM_ACK_NACK_SUBTYPE_ACK_STATUS);
		hm_frm_out.ack_nack.channel = hm_frm_in.dimmer_set.channel;
		hm_frm_out.ack_nack.status = HM_DEV_DIMMER_LEVEL_PERCENT * 2;

		hm_frm_in_ack_subtype = HM_REPLY_IN_FRM_OUT;
		return;
	}

	if (HM_FRM_CONFIG_STATUS_REQ_IS(hm_frm_in))
	{
		HM_FRM_DIMMER_INFO_LEVEL_NEW(hm_frm_out, hm_frm_in.cnt, hm_frm_in.addr_src);
		hm_frm_out.dimmer_info_level.channel = hm_frm_in.config_status_req.channel;
		hm_frm_out.dimmer_info_level.level = dim_level_percent * 2;

		hm_frm_in_ack_subtype = HM_REPLY_IN_FRM_OUT;
		return;
	}
	
	if (HM_FRM_REMOTE_IS(hm_frm_in))
	{
		if (hm_frm_in.remote.long_press)
		{
			HM_DEV_DIMMER_BUTTON_LONG(!(hm_frm_in.remote.button & 1));
		}
		else
		{			
			HM_DEV_DIMMER_SET_LEVEL(!(hm_frm_in.remote.button & 1) ? 100 : 0);
		}				
			
		if (hm_frm_in.flags.bidi)
		{
			HM_FRM_ACK_NACK_NEW(hm_frm_in, hm_frm_out, HM_FRM_ACK_NACK_SUBTYPE_ACK_STATUS);
			hm_frm_out.ack_nack.channel = 0x01;
			hm_frm_out.ack_nack.status = HM_DEV_DIMMER_LEVEL_PERCENT * 2;
			_delay_ms(100); // if sent to quick, a remote does not accept it!?
			
			hm_frm_in_ack_subtype = HM_REPLY_IN_FRM_OUT;
		}
		
		return;
	}
	
	
	hm_frm_in_ack_subtype = HM_FRM_ACK_NACK_SUBTYPE_NACK;
}

void hm_pair_remote(uint24hm_t *p_remote_addr, uint8_t remote_channel_a, uint8_t remote_channel_b)
{
	if (remote_channel_a)
		hm_init_peering(0x01, p_remote_addr, remote_channel_a);
	if (remote_channel_b)
		hm_init_peering(0x01, p_remote_addr, remote_channel_b);
}
