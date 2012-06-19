/*
 * hm_peers.h
 *
 * Created: 14.06.2012 13:18:28
 *  Author: fstorm
 */ 


#ifndef HM_PEERS_H_
#define HM_PEERS_H_


typedef struct {
	uint24hm_t addr;
	uint8_t channel;
} hm_peer_t;

extern hm_peer_t hm_peers[HM_PEERS_MAX];
extern bool hm_peers_is_dirty;


bool hm_init_peering(uint24hm_t *p_peer_addr, uint8_t peer_channel);
bool hm_remove_peering(uint24hm_t *p_peer_addr, uint8_t peer_channel);
int8_t hm_get_peer_id_by_addr_channel(uint24hm_t *p_peer_addr, uint8_t peer_channel);


#endif /* HM_PEERS_H_ */