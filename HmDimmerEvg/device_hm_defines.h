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
#include "buttons_leds.h"

#define HM_MY_ADDR 0x01EFBE		// backwards!!!
#define HM_DEVINFO_FIRMWARE 0x01
#define HM_DEVINFO_TYPE 0x5700	// backwards!!!
#define HM_DEVINFO_SERIAL_NO "GK08154711"
#define HM_DEVINFO_CLASS 0x20
#define HM_DEVINFO_PEER_CHANNEL_A 0x11
#define HM_DEVINFO_PEER_CHANNEL_B 0x01


#define HM_PEERS_MAX 1 + 1 + 16 // CCU + self + 4 other devices/channels/buttons (one remote is to peers, one for each button)

//#define HM_INIT_TASK_INCLUDE() hm_something_init();
//#define HM_TIMER_TASK_INCLUDE() hm_something_timer();
#define HM_TIMER_TASK_INCLUDE() buttons_leds_timer();

#define HM_BUTTONS_INIT() B1_INIT(); B2_INIT(); B3_INIT();
#define HM_BUTTON_SLR_BUFFER_COUNT 3
#define HM_BUTTONS_TASK() \
	hm_button_task_short_long_rpt(0, B1_STATE, 50, 400, 250, button0_short, button0_long, button0_long, NULL, NULL); \
	hm_button_task_short_long_rpt(1, B2_STATE, 50, 400, 250, button1_short, button1_long, button1_long, NULL, NULL); \
	hm_button_task_short_long_rpt(2, B3_STATE, 50, 5000, 1000, b3_short, b3_long, NULL, b3_long_end, NULL);

#define HM_SIGNAL_WAITING_FOR_PAIRING_ON() led1_state = led_blink_fast;
#define HM_SIGNAL_WAITING_FOR_PAIRING_OFF() led1_state = led_off;

#define HM_DEV_FRAME_HANDLER() hm_dimmer_frame_handler();
#define HM_DEV_DIMMER_SET_LEVEL(percent) dimmer_set(percent);
#define HM_DEV_DIMMER_BUTTON_LONG(is_up) dimmer_change_ms((is_up), 250);
#define HM_DEV_DIMMER_LEVEL_PERCENT dim_level_percent

#define HM_RESET_DEVICE() reset_device();

#endif /* HM_DEVICE_DEFINES_H_ */