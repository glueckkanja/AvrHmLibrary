/*
 * hm_defines_device.h
 *
 * Created: 19.06.2012 07:58:24
 *  Author: fstorm
 */ 


#ifndef HM_DEFINES_DEVICE_H_
#define HM_DEFINES_DEVICE_H_


///////////////////////////////////////////////////////
// Headers for HM Library to See
///////////////////////////////////////////////////////

#include "dimmer.h"
#include "buttons_leds.h"


///////////////////////////////////////////////////////
// Core Device Defines
///////////////////////////////////////////////////////

#define HM_MY_ADDR 0x01EFBE		// switched endianness!!!
#define HM_MY_SERIAL "GK00BEEF01"
#define HM_DEVINFO_FIRMWARE 0x01
#define HM_DEVINFO_TYPE 0x5700	// switched endianness!!!
#define HM_DEVINFO_CLASS 0x20
#define HM_DEVINFO_CHANNEL_INFO 0x11
#define HM_DEVINFO_CHANNEL_A 0x01
#define HM_DEVINFO_CHANNEL_B 0x00

#define HM_CHANNEL_COUNT 0x01
#define HM_PEERINGS_MAX 32 // 32 devices/channels/buttons (one remote is two peerings, one for each button)


///////////////////////////////////////////////////////
// Init/Timer Device Functions
///////////////////////////////////////////////////////

//#define HM_INIT_TASK_INCLUDE() hm_something_init();
#define HM_TIMER_TASK_INCLUDE() buttons_leds_timer();


///////////////////////////////////////////////////////
// Buttons
///////////////////////////////////////////////////////

#define HM_BUTTONS_BUFFER_COUNT 3

#define HM_BUTTONS_INIT() B_UP_INIT(); B_DOWN_INIT(); B_CONFIG_INIT();
#define HM_BUTTONS_TASK() \
	hm_button_task_debounce(0, B_UP_STATE, 100, 400, 250, button1_short, button1_long, button1_long, NULL, NULL); \
	hm_button_task_debounce(1, B_DOWN_STATE, 100, 400, 250, button2_short, button2_long, button2_long, NULL, NULL); \
	hm_button_task_debounce(2, B_CONFIG_STATE, 50, 5000, 1000, b3_short, b3_long, NULL, b3_long_end, NULL);


///////////////////////////////////////////////////////
// Signals from HM Library to Device Code
///////////////////////////////////////////////////////

#define HM_SIGNAL_WAITING_FOR_PAIRING_ON() led_state = led_blink_slow;
#define HM_SIGNAL_WAITING_FOR_PAIRING_OFF() led_state = led_off;
#define HM_RESET_DEVICE() reset_device();


///////////////////////////////////////////////////////
// Frame Handlers
///////////////////////////////////////////////////////

#define HM_DEV_FRAME_HANDLER() hm_dimmer_frame_handler();
#define HM_DEV_DIMMER_SET_LEVEL(percent) dimmer_set(percent);
#define HM_DEV_DIMMER_BUTTON_LONG(is_up) dimmer_change_ms((is_up), 250);
#define HM_DEV_DIMMER_LEVEL_PERCENT dim_level_percent


#endif /* HM_DEFINES_DEVICE_H_ */