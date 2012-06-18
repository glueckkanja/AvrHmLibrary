/*
 * hm_common.c
 *
 * Created: 14.06.2012 16:19:37
 *  Author: fstorm
 */ 

#include <avr/eeprom.h>

#include "hm_general.h"
#include "hm_parameters_peers.h"
#include "hm_messages.h"
#include "hm.h"


hm_peer_t hm_peers[HM_PEERS_MAX];
bool hm_is_peers_dirty;

bool hm_is_message_from_ccu;
bool hm_is_waiting_for_pairing;

bool hm_is_in_mode_config;
uint8_t hm_mode_config_channel;
int8_t hm_mode_config_peer_id;
uint8_t hm_mode_config_param_list;


bool hm_init_peering(hm_uint24_t *p_peer_addr, uint8_t peer_channel)
{
	for (uint8_t id = 1; id < HM_PEERS_MAX; id++)
	{
		hm_peer_t *p_peer = &hm_peers[id];
		if (ISEQUAL_HM_UINT24_TO_UINT32(p_peer->addr, 0x000000))
		{
			p_peer->addr = *p_peer_addr;
			p_peer->channel = peer_channel;
			hm_peering_reset(id);
			
			hm_is_peers_dirty = true;
			hm_config_store();

			return true;
		}
	}
	return false;
}

bool hm_remove_peering(hm_uint24_t *p_peer_addr, uint8_t peer_channel)
{
	int8_t peer_id = hm_get_peer_id_by_addr_channel(p_peer_addr, peer_channel);
	if (peer_id == -1)
		return false;
		
	hm_peers[peer_id].addr = HM_UINT24_FROM_UINT32(0x000000);
	hm_peers[peer_id].channel = 0x00;
	hm_is_peers_dirty = true;
	hm_config_store();
	
	return true;
}
	
int8_t hm_get_peer_id_by_addr_channel(hm_uint24_t *p_peer_addr, uint8_t peer_channel)
{
	if (ISEQUAL_HM_UINT24_TO_UINT32(*p_peer_addr, 0x000000))
		return 0;	// my own local config p_params
		
	for (uint8_t id = 1; id < HM_PEERS_MAX; id++)
	{
		hm_peer_t *p_peer = &hm_peers[id];
		if (ISEQUAL_HM_UINT24_TO_HM_UINT24(p_peer->addr, *p_peer_addr) && p_peer->channel == peer_channel)
			return id;
	}
	
	return -1;
}

int8_t hm_config_get_param_bank_id(uint8_t channel, uint8_t peer_id, uint8_t param_list, uint8_t param_index)
{
	for (uint8_t id = 0; id < sizeof(hm_config_param_banks) / sizeof(hm_param_bank_t); id++)
	{
		hm_param_bank_t *p_bank = &hm_config_param_banks[id];
		if (p_bank->channel == channel && p_bank->peer_id == peer_id && p_bank->param_list == param_list &&
			(p_bank->is_sequence == false || (p_bank->param_bank_seq.offset <= param_index &&
				param_index < p_bank->param_bank_seq.offset + p_bank->param_bank_seq.param_count)))
			return id;
	}
	return -1;
}

void hm_config_write(uint8_t channel, uint8_t peer_id, uint8_t param_list, hm_parameter_pair_t *p_params, uint8_t count, hm_uint24_t *p_msg_src_addr)
{
	for (uint8_t i = 0; i < count; i++)
	{
		hm_parameter_pair_t *p_param_src = &p_params[i];
		int8_t bank_id = hm_config_get_param_bank_id(channel, peer_id, param_list, p_param_src->index);
		if (bank_id != -1)
		{
			hm_param_bank_t *p_bank = &hm_config_param_banks[bank_id];
			if (p_bank->is_sequence)
			{
				uint8_t index_dst = p_param_src->index - p_bank->param_bank_seq.offset;
				p_bank->param_bank_seq.params[index_dst] = p_param_src->value;
			}
			else
			{
				for (uint8_t j = 0; j < p_bank->param_bank_pairs.pair_count; j++)
				{
					hm_parameter_pair_t *p_param_dst = &p_bank->param_bank_pairs.param_pairs[j];
					if (p_param_dst->index == p_param_src->index)
					{
						p_param_dst->value = p_param_src->value;
						break;
					}
				}
			}
			p_bank->is_dirty = true;
		}
		
		if (p_param_src->index == 0x02)	// CCU pairing parameter
		{
			switch (p_param_src->value)
			{
				case 0x81:
					hm_peers[0].addr = *p_msg_src_addr;
					break;
				case 0x00:
					hm_peers[0].addr = HM_UINT24_FROM_UINT32(0x000000);
					hm_is_waiting_for_pairing = true;	// for CONFIG_END to succeed
					break;
			}
			hm_is_peers_dirty = true;
		}

		hm_config_store();
	}
}

bool hm_config_load()
{
	hm_param_bank_t *p_bank = &hm_config_param_banks[0];
	if (eeprom_read_byte(p_bank->p_eeprom_address) != 0x02 ||
		eeprom_read_byte((p_bank->p_eeprom_address) + 2) != 0x0A ||
		eeprom_read_byte((p_bank->p_eeprom_address) + 4) != 0x0B ||
		eeprom_read_byte((p_bank->p_eeprom_address) + 6) != 0x0C)
		return false;	// invalid eeprom content
		
	for (uint8_t id = 0; id < sizeof(hm_config_param_banks) / sizeof(hm_param_bank_t); id++)
	{
		p_bank = &hm_config_param_banks[id];
		if (p_bank->is_sequence)
			eeprom_read_block(&p_bank->param_bank_seq.params[0], p_bank->p_eeprom_address, 
				p_bank->param_bank_seq.param_count * sizeof(uint8_t));
		else
			eeprom_read_block(&p_bank->param_bank_pairs.param_pairs[0], p_bank->p_eeprom_address, 
				p_bank->param_bank_pairs.pair_count * sizeof(hm_parameter_pair_t));
		p_bank->is_dirty = false;
	}
	
	eeprom_read_block(&hm_peers[0], HM_PEERS_EEPROM_LOC, sizeof(hm_peers));
	hm_is_peers_dirty = false;
	
	return true;
}

void hm_config_store()
{
	for (uint8_t id = 0; id < sizeof(hm_config_param_banks) / sizeof(hm_param_bank_t); id++)
	{
		hm_param_bank_t *p_bank = &hm_config_param_banks[id];
		if (p_bank->is_dirty)
		{
			if (p_bank->is_sequence)
				eeprom_update_block(&p_bank->param_bank_seq.params[0], p_bank->p_eeprom_address, 
					p_bank->param_bank_seq.param_count * sizeof(uint8_t));
			else
				eeprom_update_block(&p_bank->param_bank_pairs.param_pairs[0], p_bank->p_eeprom_address, 
					p_bank->param_bank_pairs.pair_count * sizeof(hm_parameter_pair_t));
			p_bank->is_dirty = false;
		}
	}
	
	if(hm_is_peers_dirty)
	{
		eeprom_update_block(&hm_peers[0], HM_PEERS_EEPROM_LOC, sizeof(hm_peers));
		hm_is_peers_dirty = false;
	}
}
