/*
 * hm_peers.c
 *
 * Created: 14.06.2012 16:19:37
 *  Author: fstorm
 */ 

#include "hm_main.h"

bool hm_peerings_is_dirty;

bool hm_init_peering(uint8_t local_channel, uint24hm_t *p_peer_addr, uint8_t peer_channel)
{
	//TBD: check for duplicates
	
	for (uint8_t id = 0; id < HM_PEERINGS_MAX; id++)
	{
		hm_peering_t *p_peering = &hm_ee_contents.config.peerings[id];
		if (p_peering->peer_addr.value == 0x000000)
		{
			p_peering->local_channel = local_channel;
			p_peering->peer_addr = *p_peer_addr;
			p_peering->peer_channel = peer_channel;
			hm_peering_reset(id, peer_channel);
			
			hm_peerings_is_dirty = true;
			hm_config_store();

			return true;
		}
	}
	return false;
}

bool hm_remove_peering(uint8_t local_channel, uint24hm_t *p_peer_addr, uint8_t peer_channel)
{
	uint8_t peering_id = hm_get_peering_id_by_addr_channel(local_channel, p_peer_addr, peer_channel);
	if (peering_id >= 0xc0)		// "local" or no peering found
		return false;
		
	hm_peering_t *p_peering = &hm_ee_contents.config.peerings[peering_id];
	p_peering->local_channel = 0x00;
	p_peering->peer_addr.value = 0x000000;
	p_peering->peer_channel = 0x00;
	hm_peerings_is_dirty = true;
	hm_config_store();
	
	return true;
}

uint8_t hm_get_peering_id_by_addr_channel(uint8_t local_channel, uint24hm_t *p_peer_addr, uint8_t peer_channel)
{
	if (p_peer_addr->value == 0x000000 && peer_channel == 0)
		return 0xc0 + local_channel;	// "local" peerings ids for device (=channel 0) and channel configuration data
		
	for (uint8_t id = 0; id < HM_PEERINGS_MAX; id++)
	{
		hm_peering_t *p_peering = &hm_ee_contents.config.peerings[id];
		if (p_peering->local_channel == local_channel
			&& p_peering->peer_addr.value == p_peer_addr->value 
			&& p_peering->peer_channel == peer_channel)
			return id;
	}
	
	return 0xff;
}
