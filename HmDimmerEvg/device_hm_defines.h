/*
 * hm_device_defines.h
 *
 * Created: 19.06.2012 07:58:24
 *  Author: fstorm
 */ 


#ifndef HM_DEVICE_DEFINES_H_
#define HM_DEVICE_DEFINES_H_


// needs to be up here for includes chain to work
#define HM_BUTTONS_GIRA_BUFFER_SIZE 50


#include "dimmer.h"
#include "buttons_leds.h"

#define HM_MY_ADDR 0x01EFBE		// switched endianness!!!
#define HM_DEVINFO_FIRMWARE 0x01
#define HM_DEVINFO_TYPE 0x5700	// switched endianness!!!
#define HM_DEVINFO_SERIAL_NO "GK08154711"
#define HM_DEVINFO_CLASS 0x20
#define HM_DEVINFO_PEER_CHANNEL_A 0x11
#define HM_DEVINFO_PEER_CHANNEL_B 0x01


#define HM_PEERS_MAX 1 + 1 + 16 // CCU + self + 16 other devices/channels/buttons (one remote is to peers, one for each button)


//#define HM_INIT_TASK_INCLUDE() hm_something_init();
#define HM_TIMER_TASK_INCLUDE() buttons_leds_timer();


#define HM_BUTTONS_BUFFER_COUNT 3

// for 2 push buttons as controlling inputs
#define HM_BUTTONS_INIT0() B1_INIT(); B2_INIT(); B3_INIT();
#define HM_BUTTONS_TASK0() \
	hm_button_task_halfwave(0, B1_STATE, 100, 400, 250, button1_short, button1_long, button1_long, NULL, NULL); \
	hm_button_task_halfwave(1, B2_STATE, 100, 400, 250, button2_short, button2_long, button2_long, NULL, NULL); \
	hm_button_task_debounce(2, B3_STATE, 50, 5000, 1000, b3_short, b3_long, NULL, b3_long_end, NULL);

// for "Gira System 2000 Nebenstelle" as controlling input (needs high speed timer)
#define HM_BUTTONS_INIT() B1A_INIT(); B3_INIT(); B1A_START();
#define HM_BUTTONS_TASK() \
	hm_button_task_gira_nebenstelle(0, 400, 250, 0, gira_event); \
	hm_button_task_debounce(1, B3_STATE, 50, 5000, 1000, b3_short, b3_long, NULL, b3_long_end, NULL);
#define HM_TIMER_TASK_INCLUDE_HIGHSPEED() hm_buttons_gira_save_adc_value(B1A_VALUE); B1A_START(); 


#define HM_SIGNAL_WAITING_FOR_PAIRING_ON() led_state = led_blink_slow;
#define HM_SIGNAL_WAITING_FOR_PAIRING_OFF() led_state = led_off;


#define HM_DEV_FRAME_HANDLER() hm_dimmer_frame_handler();
#define HM_DEV_DIMMER_SET_LEVEL(percent) dimmer_set(percent);
#define HM_DEV_DIMMER_BUTTON_LONG(is_up) dimmer_change_ms((is_up), 250);
#define HM_DEV_DIMMER_LEVEL_PERCENT dim_level_percent


#define HM_RESET_DEVICE() reset_device();


#endif /* HM_DEVICE_DEFINES_H_ */