/*
 * hm.c
 *
 * Created: 11.06.2012 13:36:29
 *  Author: fstorm
 */ 

#include "hm_main.h"

#include <util/delay.h>


void hm_system_frm_handler()
{
	hm_frm_in_ack_subtype = HM_REPLY_NONE;
	
	// autonomous pairing with remote (details just copied from a trace)
	if (hm_is_waiting_for_pairing && hm_frm_in.addr_dst.value == 0x000000
		&& HM_FRM_DEVICE_INFO_IS(hm_frm_in) && hm_frm_in.device_info.class == 0x40 && hm_frm_in.device_info.unknown == 01)
	{
		_delay_ms(300);
		
		hm_send_device_info(hm_frm_in.addr_src);
		_delay_ms(300);

		hm_pair_remote(&hm_frm_in.addr_src);
		
		return;
	}


	// check destination address
	if (!(hm_frm_in.addr_dst.value == hm_my_addr.value))
		return;	// not for me, ignore
	
	// check source address
	int8_t peer_id = -1;
	for (uint8_t i = 0; i < HM_PEERS_MAX; i++)
	{
		if (hm_peers[i].addr.value == hm_frm_in.addr_src.value)
		{
			peer_id = i;
			break;
		}
	}
	if (peer_id == -1 && !hm_is_waiting_for_pairing) // not from anybody I know
	{
		hm_frm_in_ack_subtype = HM_FRM_ACK_NACK_SUBTYPE_NACK;
		return;
	}		
	hm_is_message_from_ccu = peer_id == 0;
	
	if (HM_FRM_ACK_NACK_IS(hm_frm_in))
		return;	// ignore for now


	if (hm_is_waiting_for_pairing && HM_FRM_DEVICE_INFO_IS(hm_frm_in) &&
		hm_frm_in.device_info.class == 0x40 && hm_frm_in.device_info.unknown == 01)	// remote, details just copied from trace
	{
		HM_FRM_ACK_NACK_NEW(hm_frm_in, hm_frm_out, HM_FRM_ACK_NACK_SUBTYPE_ACK);
		hm_send();
		_delay_ms(300);
		
		hm_pair_remote(&hm_frm_in.addr_src);

		return;
	}
	

	hm_frm_in_ack_subtype = HM_FRM_ACK_NACK_SUBTYPE_ACK;


	if (HM_FRM_CONFIG_START_IS(hm_frm_in) && (hm_is_message_from_ccu || hm_is_waiting_for_pairing))
	{
		hm_mode_config_peer_id = hm_get_peer_id_by_addr_channel(&hm_frm_in.config_start.peer_address, hm_frm_in.config_start.peer_channel);
		if (hm_mode_config_peer_id == -1)
		{
			hm_frm_in_ack_subtype = HM_FRM_ACK_NACK_SUBTYPE_NACK_TARGET_INVALID;
			return;
		}
					
		hm_is_in_mode_config = true;
		hm_mode_config_channel = hm_frm_in.config_start.channel;
		hm_mode_config_param_list = hm_frm_in.config_start.param_list;
		
		return;
	}

	if (hm_is_in_mode_config && (hm_is_message_from_ccu || hm_is_waiting_for_pairing))
	{
		if (HM_FRM_CONFIG_WRITE_INDEX_IS(hm_frm_in) && hm_frm_in.config_write_pairs.channel == hm_mode_config_channel)
		{
			hm_config_write(hm_mode_config_channel, hm_mode_config_peer_id, hm_mode_config_param_list, 
				hm_frm_in.config_write_pairs.param_pairs,  HM_FRM_CONFIG_WRITE_INDEX_GET_COUNT(hm_frm_in), &hm_frm_in.addr_src);
			
			return;
		}
		else if (HM_FRM_CONFIG_END_IS(hm_frm_in) && hm_frm_in.config_end.channel == hm_mode_config_channel)
		{
			hm_is_waiting_for_pairing = false;
			hm_is_in_mode_config = false;
			
			return;
		}
	}
	
	// for now, leave pairing/config mode if anything else comes in for us
	hm_is_waiting_for_pairing = false;
	hm_is_in_mode_config = false;
	
	if (hm_is_message_from_ccu && HM_FRM_CONFIG_PARAM_REQ_IS(hm_frm_in))
	{
		int8_t peer_id = hm_get_peer_id_by_addr_channel(&hm_frm_in.config_param_req.peer_address, hm_frm_in.config_param_req.peer_channel);
		int8_t bank_id = hm_config_get_param_bank_id(hm_frm_in.config_param_req.channel, peer_id, hm_frm_in.config_param_req.param_list, 0x01);
		if (peer_id == -1 || bank_id == -1)
		{
			hm_frm_in_ack_subtype = HM_FRM_ACK_NACK_SUBTYPE_NACK_TARGET_INVALID;
			return;
		}
		
		hm_param_bank_t *p_bank = &hm_config_param_banks[bank_id];
		hm_frm_ctr = hm_frm_in.ctr;
		if (p_bank->is_sequence)
		{
			for (uint8_t i = bank_id; i < bank_id + 4; i++)
			{
				p_bank = &hm_config_param_banks[i];
				HM_FRM_INFO_PARAM_RESPONSE_SEQ_NEW(hm_frm_out, hm_frm_ctr++, hm_frm_in.addr_src, p_bank->param_bank_seq.param_count);
				hm_frm_out.info_param_response_seq.offset = p_bank->param_bank_seq.offset;
				memcpy(&hm_frm_out.info_param_response_seq.params[0], &p_bank->param_bank_seq.params[0], p_bank->param_bank_seq.param_count * sizeof(uint8_t));
						
				hm_send();
				_delay_ms(200);
			}
		}
		else
		{
			HM_FRM_INFO_PARAM_RESPONSE_PAIRS_NEW(hm_frm_out, hm_frm_ctr++, hm_frm_in.addr_src, p_bank->param_bank_pairs.pair_count);
			memcpy(&hm_frm_out.info_param_response_pairs.param_pairs[0], &p_bank->param_bank_pairs.param_pairs[0], p_bank->param_bank_pairs.pair_count * sizeof(hm_parameter_pair_t));
					
			hm_send();
			_delay_ms(200);
		}
				
		HM_FRM_INFO_PARAM_RESPONSE_SEQ_NEW(hm_frm_out, hm_frm_ctr, hm_frm_in.addr_src, 1);
		hm_frm_in_ack_subtype = HM_REPLY_IN_FRM_OUT;
		return;
	}		
	
	if (hm_is_message_from_ccu && HM_FRM_CONFIG_PEER_LIST_REQ_IS(hm_frm_in))
	{
		HM_FRM_INFO_PEER_LIST_NEW(hm_frm_out, hm_frm_in.ctr, hm_frm_in.addr_src);
		
		uint8_t peer_count = 0;
		for (uint8_t i = 1; i < HM_PEERS_MAX; i++)
		{
			if (hm_peers[i].addr.value != 0x000000)
				hm_frm_out.info_peer_list.peers[peer_count++] = hm_peers[i];
		}				
		hm_frm_out.len = HM_FRM_INFO_PEER_LIST_GET_LEN(++peer_count);
		
		hm_frm_in_ack_subtype = HM_REPLY_IN_FRM_OUT;
		return;
	}
	
	if (hm_is_message_from_ccu && HM_FRM_CONFIG_PEER_ADD_IS(hm_frm_in))
	{
		if (hm_frm_in.config_peer_add_remove.peer_channel_a)
			hm_init_peering(&hm_frm_in.config_peer_add_remove.peer_address, hm_frm_in.config_peer_add_remove.peer_channel_a);
		if (hm_frm_in.config_peer_add_remove.peer_channel_b)
			hm_init_peering(&hm_frm_in.config_peer_add_remove.peer_address, hm_frm_in.config_peer_add_remove.peer_channel_b);

		return;
	}
	
	if (hm_is_message_from_ccu && HM_FRM_CONFIG_PEER_REMOVE_IS(hm_frm_in))
	{
		if (hm_frm_in.config_peer_add_remove.peer_channel_a)
			hm_remove_peering(&hm_frm_in.config_peer_add_remove.peer_address, hm_frm_in.config_peer_add_remove.peer_channel_a);
		if (hm_frm_in.config_peer_add_remove.peer_channel_b)
			hm_remove_peering(&hm_frm_in.config_peer_add_remove.peer_address, hm_frm_in.config_peer_add_remove.peer_channel_b);

		return;
	}
	
	if (hm_is_message_from_ccu && HM_FRM_RESET_IS(hm_frm_in))
	{
		hm_config_reset();

		return;
	}

	HM_DEV_FRM_HANDLER;			

	return;
}
