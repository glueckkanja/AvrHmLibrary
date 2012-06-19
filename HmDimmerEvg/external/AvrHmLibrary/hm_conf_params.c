/*
 * hm_conf_params.c
 *
 * Created: 14.06.2012 16:19:37
 *  Author: fstorm
 */ 

#include "hm_main.h"

#include <avr/eeprom.h>


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

void hm_config_write(uint8_t channel, uint8_t peer_id, uint8_t param_list, hm_parameter_pair_t *p_params, uint8_t count, uint24hm_t *p_frm_addr_src)
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
					hm_peers[0].addr = *p_frm_addr_src;
					break;
				case 0x00:
					hm_peers[0].addr.value = 0x000000;
					hm_is_waiting_for_pairing = true;	// for CONFIG_END to succeed
					break;
			}
			hm_peers_is_dirty = true;
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
	hm_peers_is_dirty = false;
	
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
	
	if(hm_peers_is_dirty)
	{
		eeprom_update_block(&hm_peers[0], HM_PEERS_EEPROM_LOC, sizeof(hm_peers));
		hm_peers_is_dirty = false;
	}
}
