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

#include "../../board_defines.h"
#include "../../device_hm_defines.h"


#include "hm_timer.h"
#include "hm_rf_avr.h"


#define HM_FRM_TRUNK_SIZE 10
#define HM_FRM_PAYLOAD_SIZE MAX_ASKSIN_MSG - HM_FRM_TRUNK_SIZE

typedef struct {uint16_t value;} uint16hm_t;
typedef struct {uint32_t value:24;} uint24hm_t;


#include "hm_conf_params.h"
#include "hm_peers.h"
#include "hm_frames_dimmer.h"
#include "hm_frames_system.h"
#include "hm_general.h"
#include "hm_dimmer.h"


#endif /* HM_MAIN_H_ */