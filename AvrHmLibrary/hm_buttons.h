/*
 * hm_buttons.h
 *
 * Created: 19.06.2012 18:03:37
 *  Author: fstorm
 */ 


#ifndef HM_BUTTONS_H_
#define HM_BUTTONS_H_

typedef enum {
	short_click = 0,
	long_press_start,
	long_press_repeat,
	long_press_end,
	button_down
} hm_button_signal_type_t;
	
typedef enum { 
	open = 0, 
	lower, 
	upper, 
	both, 
	shorted
} hm_button_gira_nebenstelle_state_t;

typedef struct {
	uint8_t btn_state_last;
	uint8_t btn_cnt_smoothing;
	uint8_t btn_cnt_long;
	uint8_t btn_cnt_long_rpt;
} hm_button_buffer_t;

void hm_buttons_init();
void hm_buttons_timer();


#ifdef HM_BUTTONS_GIRA_BUFFER_SIZE

extern volatile uint8_t hm_buttons_gira_adc_values[HM_BUTTONS_GIRA_BUFFER_SIZE];

static inline __attribute__((always_inline)) void hm_buttons_gira_save_adc_value(uint8_t value)
{
	static uint8_t buffer_pos = 0;
	
	hm_buttons_gira_adc_values[buffer_pos] = value;
	
	if (++buffer_pos >= sizeof(hm_buttons_gira_adc_values))
		buffer_pos = 0;
}

#endif /* HM_BUTTONS_GIRA_BUFFER_SIZE */


#endif /* HM_BUTTONS_H_ */