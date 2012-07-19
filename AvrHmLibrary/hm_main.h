/*
 * hm_main.h
 *
 * Created: 14.06.2012 13:12:16
 *  Author: fstorm
 */ 


#ifndef HM_MAIN_H_
#define HM_MAIN_H_


#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct {uint16_t value;} uint16hm_t;
typedef struct {uint32_t value:24;} uint24hm_t;


// make sure both AvrHmLibrary and "real" project directory are added as include directory (-I)
// project directory needs to contain these two files and the bottom one to configure AvrHmLibrary:
#include <hm_defines_hardware.h>
#include <hm_defines_device.h>


#include <util/delay.h>


#include "hm_timer.h"
#include "hm_rf_avr.h"
#include "hm_conf_params.h"
#include "hm_peers.h"
#include "hm_frames_dimmer.h"
#include "hm_frames_system.h"

#include "hm_general.h"
#include "hm_timer.h"
#include "hm_buttons.h"
#include "hm_dimmer.h"


#include <hm_defines_subsequent.h>


#endif /* HM_MAIN_H_ */