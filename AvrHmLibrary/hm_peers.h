/*
 * hm_peers.h
 *
 * Created: 14.06.2012 13:18:28
 *  Author: fstorm
 */ 


#ifndef HM_PEERS_H_
#define HM_PEERS_H_


typedef struct {
	uint8_t local_channel;
	uint24hm_t peer_addr;
	uint8_t peer_channel;
} hm_peering_t;

extern bool hm_peerings_is_dirty;


bool hm_init_peering(uint8_t local_channel, uint24hm_t *p_peer_addr, uint8_t peer_channel);
bool hm_remove_peering(uint8_t local_channel, uint24hm_t *p_peer_addr, uint8_t peer_channel);
uint8_t hm_get_peering_id_by_addr_channel(uint8_t local_channel, uint24hm_t *p_peer_addr, uint8_t peer_channel);


#endif /* HM_PEERS_H_ */