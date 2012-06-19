/*
 * hm_peers.c
 *
 * Created: 14.06.2012 16:19:37
 *  Author: fstorm
 */ 

#include "hm_main.h"

hm_peer_t hm_peers[HM_PEERS_MAX];
bool hm_peers_is_dirty;

bool hm_init_peering(uint24hm_t *p_peer_addr, uint8_t peer_channel)
{
	//TBD: check for duplicates
	
	for (uint8_t id = 1; id < HM_PEERS_MAX; id++)
	{
		hm_peer_t *p_peer = &hm_peers[id];
		if (p_peer->addr.value == 0x000000)
		{
			p_peer->addr = *p_peer_addr;
			p_peer->channel = peer_channel;
			hm_peering_reset(id);
			
			hm_peers_is_dirty = true;
			hm_config_store();

			return true;
		}
	}
	return false;
}

bool hm_remove_peering(uint24hm_t *p_peer_addr, uint8_t peer_channel)
{
	int8_t peer_id = hm_get_peer_id_by_addr_channel(p_peer_addr, peer_channel);
	if (peer_id == -1)
		return false;
		
	hm_peers[peer_id].addr.value = 0x000000;
	hm_peers[peer_id].channel = 0x00;
	hm_peers_is_dirty = true;
	hm_config_store();
	
	return true;
}
	
int8_t hm_get_peer_id_by_addr_channel(uint24hm_t *p_peer_addr, uint8_t peer_channel)
{
	if (p_peer_addr->value == 0x000000)
		return 0;	// my own local config p_params
		
	for (uint8_t id = 1; id < HM_PEERS_MAX; id++)
	{
		hm_peer_t *p_peer = &hm_peers[id];
		if (p_peer->addr.value == p_peer_addr->value && p_peer->channel == peer_channel)
			return id;
	}
	
	return -1;
}
