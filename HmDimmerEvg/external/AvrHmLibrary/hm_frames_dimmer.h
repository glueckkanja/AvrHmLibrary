/*
 * hm_frames_dimmer.h
 *
 * Created: 11.06.2012 21:05:53
 *  Author: fstorm
 */ 


#ifndef HM_FRAMES_DIMMER_H_
#define HM_FRAMES_DIMMER_H_


#define HM_FRM_DIMMER_SET_IS(frm) ((frm).type == 0x10 && (frm).dimmer_set.subtype == 0x02)
#define HM_FRM_DIMMER_SET_NEW(frm_buf_tgt, frm_dst_addr) \
	HM_FRM_NEW((frm_buf_tgt), 16, ++hm_frm_ctr, 0x11, (frm_dst_addr)); \
	(frm_buf_tgt).dimmer_set.subtype = 0x02;
typedef struct {
	uint8_t subtype;
	uint8_t channel;
	uint8_t level;
	uint16_t ramp_time;
	uint16_t on_time;
} hm_frm_dimmer_level_set_t;


#define HM_FRM_DIMMER_INFO_LEVEL_IS(frm) ((frm).type == 0x10 && (frm).dimmer_info_level.subtype == 0x06)
#define HM_FRM_DIMMER_INFO_LEVEL_NEW(frm_buf_tgt, frm_ctr, frm_dst_addr) \
	HM_FRM_NEW((frm_buf_tgt), 14, (hm_frm_ctr), 0x10, (frm_dst_addr)); \
	(frm_buf_tgt).dimmer_info_level.subtype = 0x06;
typedef struct {
	uint8_t subtype;
	uint8_t channel;
	uint8_t level;
	uint8_t state_flags;
//	uint8_t rssi;
} hm_frm_dimmer_info_level_t;

#define HM_FRAMES_DEVICE hm_frm_dimmer_info_level_t dimmer_info_level; hm_frm_dimmer_level_set_t dimmer_set;


#endif /* HM_FRAMES_DIMMER_H_ */