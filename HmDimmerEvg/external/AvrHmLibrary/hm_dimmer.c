/*
 * hm_dimmer.c
 *
 * Created: 11.06.2012 13:36:29
 *  Author: fstorm
 */ 

#include "hm_main.h"

#include <util/delay.h>


hm_parameter_pair_t pp_00_00_00[5];
hm_parameter_pair_t pp_01_00_01[5];
uint8_t ps_01_01_03_01[13];
uint8_t ps_01_01_03_0e[13];
uint8_t ps_01_01_03_81[13];
uint8_t ps_01_01_03_8e[13];
uint8_t ps_01_02_03_01[13];
uint8_t ps_01_02_03_0e[13];
uint8_t ps_01_02_03_81[13];
uint8_t ps_01_02_03_8e[13];
uint8_t ps_01_03_03_01[13];
uint8_t ps_01_03_03_0e[13];
uint8_t ps_01_03_03_81[13];
uint8_t ps_01_03_03_8e[13];
uint8_t ps_01_04_03_01[13];
uint8_t ps_01_04_03_0e[13];
uint8_t ps_01_04_03_81[13];
uint8_t ps_01_04_03_8e[13];

#define PARAM_ID_FIRST_PEERING 2
hm_param_bank_t hm_config_param_banks[18] = {
	{ 0x00, 0x00, 0x00, false, .param_bank_pairs = { 5, pp_00_00_00 }, (void *) 0x0000 },
	{ 0x01, 0x00, 0x01, false, .param_bank_pairs = { 5, pp_01_00_01 }, (void *) 0x000A },
	{ 0x01, 0x01, 0x03, true, .param_bank_seq = { 0x01, 13, ps_01_01_03_01 }, (void *) 0x0014 + 13 * 0 },
	{ 0x01, 0x01, 0x03, true, .param_bank_seq = { 0x0e, 13, ps_01_01_03_0e }, (void *) 0x0014 + 13 * 1 },
	{ 0x01, 0x01, 0x03, true, .param_bank_seq = { 0x81, 13, ps_01_01_03_81 }, (void *) 0x0014 + 13 * 2 },
	{ 0x01, 0x01, 0x03, true, .param_bank_seq = { 0x8e, 13, ps_01_01_03_8e }, (void *) 0x0014 + 13 * 3 },
	{ 0x01, 0x02, 0x03, true, .param_bank_seq = { 0x01, 13, ps_01_02_03_01 }, (void *) 0x0014 + 13 * 4 },
	{ 0x01, 0x02, 0x03, true, .param_bank_seq = { 0x0e, 13, ps_01_02_03_0e }, (void *) 0x0014 + 13 * 5 },
	{ 0x01, 0x02, 0x03, true, .param_bank_seq = { 0x81, 13, ps_01_02_03_81 }, (void *) 0x0014 + 13 * 6 },
	{ 0x01, 0x02, 0x03, true, .param_bank_seq = { 0x8e, 13, ps_01_02_03_8e }, (void *) 0x0014 + 13 * 7 },
	{ 0x01, 0x03, 0x03, true, .param_bank_seq = { 0x01, 13, ps_01_03_03_01 }, (void *) 0x0014 + 13 * 8 },
	{ 0x01, 0x03, 0x03, true, .param_bank_seq = { 0x0e, 13, ps_01_03_03_0e }, (void *) 0x0014 + 13 * 9 },
	{ 0x01, 0x03, 0x03, true, .param_bank_seq = { 0x81, 13, ps_01_03_03_81 }, (void *) 0x0014 + 13 * 10 },
	{ 0x01, 0x03, 0x03, true, .param_bank_seq = { 0x8e, 13, ps_01_03_03_8e }, (void *) 0x0014 + 13 * 11 },
	{ 0x01, 0x03, 0x03, true, .param_bank_seq = { 0x01, 13, ps_01_04_03_01 }, (void *) 0x0014 + 13 * 12 },
	{ 0x01, 0x03, 0x03, true, .param_bank_seq = { 0x0e, 13, ps_01_04_03_0e }, (void *) 0x0014 + 13 * 13 },
	{ 0x01, 0x03, 0x03, true, .param_bank_seq = { 0x81, 13, ps_01_04_03_81 }, (void *) 0x0014 + 13 * 14 },
	{ 0x01, 0x03, 0x03, true, .param_bank_seq = { 0x8e, 13, ps_01_04_03_8e }, (void *) 0x0014 + 13 * 15 }
};

const hm_parameter_pair_t PROGMEM PP_DEF_00_00_00[5] = {{0x02, 0x00}, {0x0A, 0x00}, {0x0B, 0x00}, {0x0C, 0x00}, {0x15, 0xFF}};
const hm_parameter_pair_t PROGMEM PP_DEF_01_00_01[5] = {{0x30, 0x06}, {0x32, 0x50}, {0x33, 0x64}, {0x34, 0x4B}, {0x35, 0x50}};
const uint8_t PROGMEM PS_DEF_01_03_01[13] = {0x00, 0x00, 0x00, 0x32, 0x64, 0x00, 0xFF, 0x00, 0xFF, 0x01, 0x14, 0x52, 0x63};
const uint8_t PROGMEM PS_DEF_01_03_0E[13] = {0x20, 0x00, 0x14, 0xC8, 0x0A, 0x05, 0x05, 0x00, 0xC8, 0x0A, 0x0A, 0x04, 0x04};
const uint8_t PROGMEM PS_DEF_01_03_81[13] = {0x00, 0x00, 0x00, 0x32, 0x64, 0x00, 0xFF, 0x00, 0xFF, 0x26, 0x14, 0x52, 0x63};
const uint8_t PROGMEM PS_DEF_01_03_8E[13] = {0x20, 0x00, 0x14, 0xC8, 0x0A, 0x05, 0x05, 0x00, 0xC8, 0x0A, 0x0A, 0x04, 0x04};


void hm_config_reset()
{
	memcpy_P(pp_00_00_00, PP_DEF_00_00_00, sizeof(pp_00_00_00));
	memcpy_P(pp_01_00_01, PP_DEF_01_00_01, sizeof(pp_01_00_01));
	for (uint8_t id = 0; id < sizeof(hm_config_param_banks) / sizeof(hm_param_bank_t); id++)
		hm_config_param_banks[id].is_dirty = true;
	
	memset(hm_peers, 0, sizeof(hm_peers));
	hm_init_peering(&hm_my_addr, 0x01);
}

void hm_peering_reset(uint8_t peer_id)
{
	uint8_t pb_id = PARAM_ID_FIRST_PEERING + (peer_id - 1) * 4;
	memcpy_P(hm_config_param_banks[pb_id].param_bank_seq.params, PS_DEF_01_03_01, sizeof(PS_DEF_01_03_01));
	hm_config_param_banks[pb_id++].is_dirty = true;
	memcpy_P(hm_config_param_banks[pb_id].param_bank_seq.params, PS_DEF_01_03_0E, sizeof(PS_DEF_01_03_0E));
	hm_config_param_banks[pb_id++].is_dirty = true;
	memcpy_P(hm_config_param_banks[pb_id].param_bank_seq.params, PS_DEF_01_03_81, sizeof(PS_DEF_01_03_81));
	hm_config_param_banks[pb_id++].is_dirty = true;
	memcpy_P(hm_config_param_banks[pb_id].param_bank_seq.params, PS_DEF_01_03_8E, sizeof(PS_DEF_01_03_8E));
	hm_config_param_banks[pb_id++].is_dirty = true;
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
		HM_FRM_DIMMER_INFO_LEVEL_NEW(hm_frm_out, hm_frm_in.ctr, hm_frm_in.addr_src);
		hm_frm_out.dimmer_info_level.channel = hm_frm_in.config_status_req.channel;
		hm_frm_out.dimmer_info_level.level = dim_level_percent * 2;

		hm_frm_in_ack_subtype = HM_REPLY_IN_FRM_OUT;
		return;
	}
	
	if (HM_FRM_REMOTE_IS(hm_frm_in))
	{
		if (hm_frm_in.remote.long_press)
		{
			HM_DEV_DIMMER_BUTTON_LONG(hm_frm_in.remote.button == 2);
		}
		else
		{			
			HM_DEV_DIMMER_SET_LEVEL(hm_frm_in.remote.button == 2 ? 100 : 0);
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

void hm_pair_remote_onechannel(uint24hm_t *p_dst, uint8_t channel)
{
	_delay_ms(300);
	hm_init_peering(p_dst, channel);

	// just copied from traces
	HM_FRM_CONFIG_START_NEW(hm_frm_out, *p_dst);
	hm_frm_out.config_start.channel = channel;
	hm_frm_out.config_start.peer_address = hm_my_addr;
	hm_frm_out.config_start.peer_channel = 1;
	hm_frm_out.config_start.param_list = 4;
	hm_send();
	_delay_ms(300);
	
	HM_FRM_CONFIG_UNKNOWN_01_NEW(hm_frm_out, *p_dst, 3);
	hm_frm_out.config_unknown_01.channel = channel;
	hm_frm_out.config_unknown_01.data[0] = 0x02;
	hm_frm_out.config_unknown_01.data[1] = 0x01;
	hm_frm_out.config_unknown_01.data[2] = 0x01;
	hm_send();
	_delay_ms(300);
	
	HM_FRM_CONFIG_END_NEW(hm_frm_out, *p_dst);
	hm_frm_out.config_end.channel = channel;
		
	hm_send();
	_delay_ms(300);
}
	
void hm_pair_remote(uint24hm_t *p_dst)
{
	hm_pair_remote_onechannel(p_dst, 2);
	hm_pair_remote_onechannel(p_dst, 1);
}
