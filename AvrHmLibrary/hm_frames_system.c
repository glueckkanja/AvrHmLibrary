/*
 * hm_frames_system.c
 *
 * Created: 11.06.2012 13:36:29
 *  Author: fstorm
 */ 

#include "hm_main.h"

#include <util/delay.h>


void hm_system_frame_handler()
{
	hm_frm_in_ack_subtype = HM_REPLY_NONE;
	
	// autonomous pairing with remote (details just copied from a trace)
	if (hm_is_waiting_for_pairing && hm_frm_in.addr_dst.value == 0x000000 && HM_FRM_DEVICE_INFO_IS(hm_frm_in) 
		&& hm_frm_in.device_info.class == HM_DEVICE_CLASS_REMOTE && hm_frm_in.device_info.channel_a)
	{
		_delay_ms(300);
		
		hm_send_device_info(hm_frm_in.addr_src);
		_delay_ms(300);

		hm_pair_remote(&hm_frm_in.addr_src, hm_frm_in.device_info.channel_a, hm_frm_in.device_info.channel_b);
		hm_pairing_process_end();

		return;
	}


	// check destination address
	if (!(hm_frm_in.addr_dst.value == hm_ee_contents.my_addr.value))
		return;	// not for me, ignore
	
	// check source address
	if (hm_ee_contents.config.ccu_addr.value != 0x000000 && hm_frm_in.addr_src.value == hm_ee_contents.config.ccu_addr.value)
	{
		hm_is_frame_from_ccu = true;
	}
	else
	{
		uint8_t i;
		for (i = 0; i < HM_PEERINGS_MAX; i++)
		{
			if (hm_frm_in.addr_src.value == hm_ee_contents.config.peerings[i].peer_addr.value)
			{
				break;
			}
		}
		if (i >= HM_PEERINGS_MAX && !hm_is_waiting_for_pairing) // not from anybody I know
		{
			hm_frm_in_ack_subtype = HM_FRM_ACK_NACK_SUBTYPE_NACK;
			return;
		}
	}
	
	if (HM_FRM_ACK_NACK_IS(hm_frm_in))
		return;	// ignore for now


	if (hm_is_waiting_for_pairing && HM_FRM_DEVICE_INFO_IS(hm_frm_in) 
		&& hm_frm_in.device_info.class == HM_DEVICE_CLASS_REMOTE && hm_frm_in.device_info.channel_a)	// remote, details just copied from trace
	{
		HM_FRM_ACK_NACK_NEW(hm_frm_in, hm_frm_out, HM_FRM_ACK_NACK_SUBTYPE_ACK);
		hm_send();
		_delay_ms(300);
		
		hm_pair_remote(&hm_frm_in.addr_src, hm_frm_in.device_info.channel_a, hm_frm_in.device_info.channel_b);
		hm_pairing_process_end();
		
		return;
	}
	

	hm_frm_in_ack_subtype = HM_FRM_ACK_NACK_SUBTYPE_ACK;


	if (HM_FRM_CONFIG_START_IS(hm_frm_in) && (hm_is_frame_from_ccu || hm_is_waiting_for_pairing))
	{
		hm_mode_config_peering_id = hm_get_peering_id_by_addr_channel(hm_frm_in.config_start.channel, &hm_frm_in.config_start.peer_address, hm_frm_in.config_start.peer_channel);
		if (hm_mode_config_peering_id == 0xff)
		{
			hm_frm_in_ack_subtype = HM_FRM_ACK_NACK_SUBTYPE_NACK_TARGET_INVALID;
			return;
		}
					
		hm_is_in_mode_config = true;
		hm_mode_config_channel = hm_frm_in.config_start.channel;
		hm_mode_config_param_list = hm_frm_in.config_start.param_list;
		
		return;
	}

	if (HM_FRM_CONFIG_END_IS(hm_frm_in))
	{
		uint8_t i = 3;
	}
			
	if (hm_is_in_mode_config && (hm_is_frame_from_ccu || hm_is_waiting_for_pairing))
	{
		if (HM_FRM_CONFIG_WRITE_INDEX_IS(hm_frm_in) && hm_frm_in.config_write_index.channel == hm_mode_config_channel)
		{
			hm_config_write(hm_mode_config_peering_id, hm_mode_config_param_list, 
				hm_frm_in.config_write_index.param_pairs, HM_FRM_CONFIG_WRITE_INDEX_GET_COUNT(hm_frm_in), &hm_frm_in.addr_src);
			
			return;
		}
		else if (HM_FRM_CONFIG_END_IS(hm_frm_in) && hm_frm_in.config_end.channel == hm_mode_config_channel)
		{
			hm_pairing_process_end();
			hm_is_in_mode_config = false;
			
			return;
		}
	}
	
	// for now, leave pairing/config mode if anything else comes in for us
	if (hm_is_waiting_for_pairing)
	{
		hm_pairing_process_end();
	}
	
	hm_is_in_mode_config = false;
	
	if (hm_is_frame_from_ccu && HM_FRM_CONFIG_PARAM_REQ_IS(hm_frm_in))
	{
		uint8_t peering_id = hm_get_peering_id_by_addr_channel(hm_frm_in.config_param_req.channel, &hm_frm_in.config_param_req.peer_address, hm_frm_in.config_param_req.peer_channel);
		uint8_t bank_id = hm_config_get_param_bank_id(peering_id, hm_frm_in.config_param_req.param_list, 0x01);
		if (peering_id == 0xff || bank_id == 0xff)
		{
			hm_frm_in_ack_subtype = HM_FRM_ACK_NACK_SUBTYPE_NACK_TARGET_INVALID;
			return;
		}
		
		hm_param_bank_t *p_bank = &hm_config_param_banks[bank_id];
		hm_frm_cnt = hm_frm_in.cnt;
		if (p_bank->is_sequence)
		{
			for (uint8_t i = bank_id; i < bank_id + 4; i++)
			{
				p_bank = &hm_config_param_banks[i];
				HM_FRM_INFO_PARAM_RESPONSE_SEQ_NEW(hm_frm_out, hm_frm_cnt++, hm_frm_in.addr_src, p_bank->param_bank_seq.param_count);
				hm_frm_out.info_param_response_seq.offset = p_bank->param_bank_seq.offset;
				memcpy(&hm_frm_out.info_param_response_seq.params[0], &p_bank->param_bank_seq.params[0], p_bank->param_bank_seq.param_count * sizeof(uint8_t));
						
				hm_send();
				_delay_ms(200);
			}
		}
		else
		{
			HM_FRM_INFO_PARAM_RESPONSE_PAIRS_NEW(hm_frm_out, hm_frm_cnt++, hm_frm_in.addr_src, p_bank->param_bank_pairs.pair_count);
			memcpy(&hm_frm_out.info_param_response_pairs.param_pairs[0], &p_bank->param_bank_pairs.param_pairs[0], p_bank->param_bank_pairs.pair_count * sizeof(hm_parameter_pair_t));
					
			hm_send();
			_delay_ms(200);
		}
				
		HM_FRM_INFO_PARAM_RESPONSE_SEQ_NEW(hm_frm_out, hm_frm_cnt++, hm_frm_in.addr_src, 1);
		hm_frm_in_ack_subtype = HM_REPLY_IN_FRM_OUT;
		return;
	}		
	
	if (hm_is_frame_from_ccu && HM_FRM_CONFIG_PEER_LIST_REQ_IS(hm_frm_in))
	{
		hm_frm_cnt = hm_frm_in.cnt;

		uint8_t peer_count = 0;
		HM_FRM_INFO_PEER_LIST_NEW(hm_frm_out, hm_frm_cnt++, hm_frm_in.addr_src);
		for (uint8_t i = 0; i < HM_PEERINGS_MAX; i++)
		{
			hm_peering_t *p_peering = &hm_ee_contents.config.peerings[i];
			if (p_peering->local_channel == hm_frm_in.config_peer_list_req.channel
				&& p_peering->peer_addr.value != 0x000000)
			{
				hm_frm_out.info_peer_list.links[peer_count].peer_addr = p_peering->peer_addr;
				hm_frm_out.info_peer_list.links[peer_count].peer_channel = p_peering->peer_channel;
				peer_count++;
			}
			if(peer_count >= 4)
			{
				hm_frm_out.len = HM_FRM_INFO_PEER_LIST_GET_LEN(peer_count);
				hm_send();
				_delay_ms(200);
				
				peer_count = 0;
				HM_FRM_INFO_PEER_LIST_NEW(hm_frm_out, hm_frm_cnt++, hm_frm_in.addr_src);
			}				
		}
		hm_frm_out.len = HM_FRM_INFO_PEER_LIST_GET_LEN(++peer_count);	// one peer 0x000000 as terminator
		hm_frm_in_ack_subtype = HM_REPLY_IN_FRM_OUT;
		
		return;
	}
	
	if (hm_is_frame_from_ccu && HM_FRM_CONFIG_PEER_ADD_IS(hm_frm_in))
	{
		if (hm_frm_in.config_peer_add_remove.peer_channel_a)
			hm_init_peering(hm_frm_in.config_peer_add_remove.channel, &hm_frm_in.config_peer_add_remove.peer_address, hm_frm_in.config_peer_add_remove.peer_channel_a);
		if (hm_frm_in.config_peer_add_remove.peer_channel_b)
			hm_init_peering(hm_frm_in.config_peer_add_remove.channel, &hm_frm_in.config_peer_add_remove.peer_address, hm_frm_in.config_peer_add_remove.peer_channel_b);

		return;
	}
	
	if (hm_is_frame_from_ccu && HM_FRM_CONFIG_PEER_REMOVE_IS(hm_frm_in))
	{
		if (hm_frm_in.config_peer_add_remove.peer_channel_a)
			hm_remove_peering(hm_frm_in.config_peer_add_remove.channel, &hm_frm_in.config_peer_add_remove.peer_address, hm_frm_in.config_peer_add_remove.peer_channel_a);
		if (hm_frm_in.config_peer_add_remove.peer_channel_b)
			hm_remove_peering(hm_frm_in.config_peer_add_remove.channel, &hm_frm_in.config_peer_add_remove.peer_address, hm_frm_in.config_peer_add_remove.peer_channel_b);

		return;
	}
	
	if (hm_is_frame_from_ccu && HM_FRM_RESET_IS(hm_frm_in))
	{
		hm_config_reset();

		return;
	}

	HM_DEV_FRAME_HANDLER();			

	return;
}
