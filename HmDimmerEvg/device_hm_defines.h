/*
 * hm_device_defines.h
 *
 * Created: 19.06.2012 07:58:24
 *  Author: fstorm
 */ 


#ifndef HM_DEVICE_DEFINES_H_
#define HM_DEVICE_DEFINES_H_


#include "debug.h"
#include "dimmer.h"

#define HM_MY_ADDR 0x01EFBE		// backwards!!!
#define HM_DEVINFO_FIRMWARE 0x01
#define HM_DEVINFO_TYPE 0x5700	// backwards!!!
#define HM_DEVINFO_SERIAL_NO "GK08154711"
#define HM_DEVINFO_CLASS 0x20
#define HM_DEVINFO_PEER_CHANNEL_A 0x11
#define HM_DEVINFO_PEER_CHANNEL_B 0x01


#define HM_PEERS_MAX 1 + 4 // CCU + 4 other devices

//#define HM_TIMER_TASK_INCLUDE keys_task();

#define HM_DEV_FRM_HANDLER hm_dimmer_frm_handler();
#define HM_DEV_DIMMER_SET_LEVEL(percent) dimmer_set(percent);
#define HM_DEV_DIMMER_BUTTON_LONG(is_up) dimmer_change_ms((is_up), 250);
#define HM_DEV_DIMMER_LEVEL_PERCENT dim_level_percent

#endif /* HM_DEVICE_DEFINES_H_ */