/*
 * hm.c
 *
 * Created: 11.06.2012 13:36:29
 *  Author: fstorm
 */ 

#include "board.h"

#include <util/delay.h>
#include "rf.h"
#include "dimmer.h"

#include "hm.h"
#include "hm_general.h"
#include "hm_parameters_peers.h"
#include "hm_messages.h"

#include "debug.h"

uint8_t packet_enc[MAX_ASKSIN_MSG];
uint8_t rssi, lqi;

hm_msg_t hm_msg_in;
hm_msg_t hm_msg_out;


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




void hm_init()
{
	if (!hm_config_load())
		hm_config_reset();
	
	rf_asksin_init();
}

void hm_config_reset()
{
	memcpy_P(pp_00_00_00, PP_DEF_00_00_00, sizeof(pp_00_00_00));
	memcpy_P(pp_01_00_01, PP_DEF_01_00_01, sizeof(pp_01_00_01));
	for (uint8_t id = 0; id < sizeof(hm_config_param_banks) / sizeof(hm_param_bank_t); id++)
		hm_config_param_banks[id].is_dirty = true;
	
	memset(hm_peers, 0, sizeof(hm_peers));
	hm_init_peering(&HM_UINT24_FROM_UINT32(HM_MY_ADDR), 0x01);
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
	
void hm_task()
{
	uint8_t init_ack_nack_subcmd = 0xff;
	
	if (!rf_asksin_receive(packet_enc, &rssi, &lqi))
		return;	// no message received
		
	// "decrypt"
	hm_msg_decryt(&hm_msg_in, packet_enc);
	debug_dump((uint8_t*)&hm_msg_in, hm_msg_in.len + 1, "R: ");
	
	
	// autonomous pairing with remote (details just copied from a trace)
	if (hm_is_waiting_for_pairing && ISEQUAL_HM_UINT24_TO_UINT32(hm_msg_in.dst, 0x000000)
		&& IS_HM_MSG_DEVICE_INFO(hm_msg_in)
		&& hm_msg_in.device_info.class == 0x40 && hm_msg_in.device_info.unknown == 01)
	{
		hm_send_device_info(hm_msg_in.src);
		_delay_ms(200);
		
		hm_pair_remote(&hm_msg_in.src);
		hm_is_waiting_for_pairing = false;
		
		return;
	}


	// check destination address
	if (!(ISEQUAL_HM_UINT24_TO_UINT32(hm_msg_in.dst, HM_MY_ADDR)))
		return;	// not for me, ignore
	
	// check source address
	int8_t peer_id = -1;
	for (uint8_t i = 0; i < HM_PEERS_MAX; i++)
	{
		if (ISEQUAL_HM_UINT24_TO_HM_UINT24(hm_peers[i].addr, hm_msg_in.src))
		{
			peer_id = i;
			break;
		}
	}
	if (peer_id == -1 && !hm_is_waiting_for_pairing) // not from anybody I know
	{
		init_ack_nack_subcmd = HM_SUBCMD_NACK;
		goto finish;
	}		
	hm_is_message_from_ccu = peer_id == 0;
	
	if (IS_HM_MSG_ACK_NACK(hm_msg_in))
		return;	// ignore for now


	if (hm_is_waiting_for_pairing && IS_HM_MSG_DEVICE_INFO_DIRECTED(hm_msg_in) &&
		hm_msg_in.device_info.class == 0x20 && hm_msg_in.device_info.unknown == 01)	// remote, details just copied from trace
	{
		hm_pair_remote(&hm_msg_in.src);
		hm_is_waiting_for_pairing = false;
			
		return;
	}
	
	if (IS_HM_MSG_CONFIG_START(hm_msg_in) && (hm_is_message_from_ccu || hm_is_waiting_for_pairing))
	{
		hm_mode_config_peer_id = hm_get_peer_id_by_addr_channel(&hm_msg_in.config_start.peer_address, hm_msg_in.config_start.peer_channel);
		if (hm_mode_config_peer_id != -1)
		{
			hm_is_in_mode_config = true;
			hm_mode_config_channel = hm_msg_in.config_start.channel;
			hm_mode_config_param_list = hm_msg_in.config_start.param_list;
			
			init_ack_nack_subcmd = HM_SUBCMD_ACK;
		}
		else
		{
			init_ack_nack_subcmd = HM_SUBCMD_NACK_TARGET_INVALID;
		}						
		goto finish;
	}

	if (hm_is_in_mode_config && (hm_is_message_from_ccu || hm_is_waiting_for_pairing))
	{
		if (IS_HM_MSG_CONFIG_WRITE_PAIRS(hm_msg_in) && hm_msg_in.config_write_pairs.channel == hm_mode_config_channel)
		{
			hm_config_write(hm_mode_config_channel, hm_mode_config_peer_id, hm_mode_config_param_list, 
				hm_msg_in.config_write_pairs.param_pairs,  GET_COUNT_HM_MSG_CONFIG_WRITE_PAIRS(hm_msg_in), &hm_msg_in.src);
			init_ack_nack_subcmd = HM_SUBCMD_ACK;
			goto finish;
		}
		else if (IS_HM_MSG_CONFIG_END(hm_msg_in) && hm_msg_in.config_end.channel == hm_mode_config_channel)
		{
			hm_is_waiting_for_pairing = false;
			hm_is_in_mode_config = false;
			init_ack_nack_subcmd = HM_SUBCMD_ACK;
			goto finish;
		}
	}
	
	// for now, leave pairing/config mode if anything else comes in for us
	hm_is_waiting_for_pairing = false;
	hm_is_in_mode_config = false;
	
	if (hm_is_message_from_ccu && IS_HM_MSG_CONFIG_PARAM_REQ(hm_msg_in))
	{
		int8_t peer_id = hm_get_peer_id_by_addr_channel(&hm_msg_in.config_param_req.peer_address, hm_msg_in.config_param_req.peer_channel);
		if (peer_id != -1)
		{
			int8_t bank_id = hm_config_get_param_bank_id(hm_msg_in.config_param_req.channel, peer_id, 
				hm_msg_in.config_param_req.param_list, 0x01);
			if (bank_id != -1)
			{
				hm_param_bank_t *p_bank = &hm_config_param_banks[bank_id];
				hm_msgctr = hm_msg_in.msgctr;
				if (p_bank->is_sequence)
				{
					for (uint8_t i = bank_id; i < bank_id + 4; i++)
					{
						p_bank = &hm_config_param_banks[i];
						INIT_HM_MSG_INFO_PARAM_RESPONSE_SEQ(hm_msg_out, hm_msgctr++, hm_msg_in.src, p_bank->param_bank_seq.param_count);
						hm_msg_out.info_param_response_seq.offset = p_bank->param_bank_seq.offset;
						memcpy(&hm_msg_out.info_param_response_seq.params[0], &p_bank->param_bank_seq.params[0], p_bank->param_bank_seq.param_count * sizeof(uint8_t));
						hm_send();
						_delay_ms(200);
					}
				}
				else
				{
					INIT_HM_MSG_INFO_PARAM_RESPONSE_PAIRS(hm_msg_out, hm_msgctr++, hm_msg_in.src, p_bank->param_bank_pairs.pair_count);
					memcpy(&hm_msg_out.info_param_response_pairs.param_pairs[0], &p_bank->param_bank_pairs.param_pairs[0], p_bank->param_bank_pairs.pair_count * sizeof(hm_parameter_pair_t));
					hm_send();
					_delay_ms(200);
				}
				
				INIT_HM_MSG_INFO_PARAM_RESPONSE_SEQ(hm_msg_out, hm_msgctr, hm_msg_in.src, 1);
				goto finish;
			}
		}
		
		init_ack_nack_subcmd = HM_SUBCMD_NACK_TARGET_INVALID;															
		goto finish;
	}		
	
	if (hm_is_message_from_ccu && IS_HM_MSG_CONFIG_PEER_LIST_REQ(hm_msg_in))
	{
		INIT_HM_MSG_INFO_PEER_LIST(hm_msg_out, hm_msg_in.msgctr, hm_msg_in.src);
		
		uint8_t peer_count = 0;
		for (uint8_t i = 1; i < HM_PEERS_MAX; i++)
		{
			if (!ISEQUAL_HM_UINT24_TO_UINT32(hm_peers[i].addr, 0x000000))
				hm_msg_out.info_peer_list.peers[peer_count++] = hm_peers[i];
		}				
		hm_msg_out.len = GET_LEN_HM_MSG_INFO_PEER_LIST(++peer_count);
		
		goto finish;
	}
	
	if (hm_is_message_from_ccu && IS_HM_MSG_CONFIG_PEER_ADD(hm_msg_in))
	{
		if (hm_msg_in.config_peer_add_remove.peer_channel_a)
			hm_init_peering(&hm_msg_in.config_peer_add_remove.peer_address, hm_msg_in.config_peer_add_remove.peer_channel_a);
		if (hm_msg_in.config_peer_add_remove.peer_channel_b)
			hm_init_peering(&hm_msg_in.config_peer_add_remove.peer_address, hm_msg_in.config_peer_add_remove.peer_channel_b);

		init_ack_nack_subcmd = HM_SUBCMD_ACK;
		goto finish;
	}
	
	if (hm_is_message_from_ccu && IS_HM_MSG_CONFIG_PEER_REMOVE(hm_msg_in))
	{
		if (hm_msg_in.config_peer_add_remove.peer_channel_a)
			hm_remove_peering(&hm_msg_in.config_peer_add_remove.peer_address, hm_msg_in.config_peer_add_remove.peer_channel_a);
		if (hm_msg_in.config_peer_add_remove.peer_channel_b)
			hm_remove_peering(&hm_msg_in.config_peer_add_remove.peer_address, hm_msg_in.config_peer_add_remove.peer_channel_b);

		init_ack_nack_subcmd = HM_SUBCMD_ACK;
		goto finish;
	}
	
	if (hm_is_message_from_ccu && IS_HM_MSG_RESET(hm_msg_in))
	{
		hm_config_reset();

		init_ack_nack_subcmd = HM_SUBCMD_ACK;
		goto finish;
	}
			
	if (IS_HM_MSG_SET(hm_msg_in))
	{
		dimmer_set(hm_msg_in.set.value / 2);
		
		INIT_HM_MSG_ACK_STATUS(hm_msg_in, hm_msg_out);
		hm_msg_out.ack_nack.channel = hm_msg_in.set.channel;
		hm_msg_out.ack_nack.status = dim_level_percent * 2;
		goto finish;
	}

	if (IS_HM_MSG_CONFIG_STATUS_REQ(hm_msg_in))
	{
		INIT_HM_MSG_INFO_ACTUATOR_STATUS(hm_msg_out, hm_msg_in.msgctr, hm_msg_in.src);
		hm_msg_out.info_actuator_status.channel = hm_msg_in.config_status_req.channel;
		hm_msg_out.info_actuator_status.status = dim_level_percent * 2;
		goto finish;
	}
	
	if (IS_HM_MSG_REMOTE(hm_msg_in))
	{
		if (hm_msg_in.remote.long_press)
		{
			dimmer_change_ms(hm_msg_in.remote.button == 2, 250);
		}
		else
		{			
			dimmer_set(hm_msg_in.remote.button == 2 ? 100 : 0);
		}				
			
		INIT_HM_MSG_ACK_STATUS(hm_msg_in, hm_msg_out);
		hm_msg_out.ack_nack.channel = 0x01;
		hm_msg_out.ack_nack.status = dim_level_percent * 2;
		_delay_ms(100); // if sent to quick, a remote does not accept it!?
		goto finish;
	}
	
	
	init_ack_nack_subcmd = HM_SUBCMD_NACK;
	

finish:
	if (init_ack_nack_subcmd != 0xff)
	{
		INIT_HM_MSG_ACK_NACK(hm_msg_in, hm_msg_out, init_ack_nack_subcmd);
	}		
	hm_send();
	return;
}

void hm_send()
{
	debug_dump((uint8_t*)&hm_msg_out, hm_msg_out.len + 1, "S: ");

	// "crypt"
	hm_msg_encrypt(packet_enc, &hm_msg_out);
	
	rf_asksin_send(packet_enc);
}

void hm_send_device_info(hm_uint24_t dst)
{
	if (ISEQUAL_HM_UINT24_TO_UINT32(dst, 0x000000))
	{
		INIT_HM_MSG_DEVICE_INFO(hm_msg_out);
	}
	else
	{
		INIT_HM_MSG_DEVICE_INFO_DIRECTED(hm_msg_out,dst);
	}		
	hm_msg_out.device_info.firmware = HM_DEVINFO_FIRMWARE;
	hm_msg_out.device_info.type = HM_UINT16_FROM_UINT16(HM_DEVINFO_TYPE);
	memcpy_P(hm_msg_out.device_info.serial_no, PSTR(HM_DEVINFO_SERIAL_NO), 10);
	hm_msg_out.device_info.class = HM_DEVINFO_CLASS;
	hm_msg_out.device_info.peer_channel_a = HM_DEVINFO_PEER_CHANNEL_A;
	hm_msg_out.device_info.peer_channel_b = HM_DEVINFO_PEER_CHANNEL_B;
	
	hm_send();
	
	hm_is_waiting_for_pairing = true;
}

void hm_pair_remote_onechannel(hm_uint24_t *p_dst, uint8_t channel)
{
	hm_init_peering(p_dst, channel);

	// just copied from traces
	INIT_HM_MSG_CONFIG_START(hm_msg_out, *p_dst);
	hm_msg_out.config_start.channel = channel;
	hm_msg_out.config_start.peer_address = HM_UINT24_FROM_UINT32(HM_MY_ADDR);
	hm_msg_out.config_start.peer_channel = 1;
	hm_msg_out.config_start.param_list = 4;
	hm_send();
	_delay_ms(200);
	
	INIT_HM_MSG_CONFIG_UNKNOWN_01(hm_msg_out, *p_dst, 3);
	hm_msg_out.config_unknown_01.channel = channel;
	hm_msg_out.config_unknown_01.data[0] = 0x02;
	hm_msg_out.config_unknown_01.data[1] = 0x01;
	hm_msg_out.config_unknown_01.data[2] = 0x01;
	hm_send();
	_delay_ms(200);
	
	INIT_HM_MSG_CONFIG_END(hm_msg_out, *p_dst);
	hm_msg_out.config_end.channel = channel;
		
	hm_send();
	_delay_ms(200);
}
	
void hm_pair_remote(hm_uint24_t *p_dst)
{
	hm_pair_remote_onechannel(p_dst, 2);
	hm_pair_remote_onechannel(p_dst, 1);
}
