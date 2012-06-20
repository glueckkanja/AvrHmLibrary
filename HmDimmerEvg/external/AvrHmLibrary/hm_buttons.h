/*
 * hm_buttons.h
 *
 * Created: 19.06.2012 18:03:37
 *  Author: fstorm
 */ 


#ifndef HM_BUTTONS_H_
#define HM_BUTTONS_H_

typedef struct {
	bool btn_state_last;
	uint8_t btn_ctr_debounce_ignore;
	uint8_t btn_ctr_long;
	uint8_t btn_ctr_long_rpt;
} hm_button_slr_buffer_t;

void hm_buttons_init();
void hm_buttons_timer();


#endif /* HM_BUTTONS_H_ */