/*
 * hm_buttons.c
 *
 * Created: 19.06.2012 18:03:24
 *  Author: fstorm
 */ 

#include "hm_main.h"

void hm_buttons_init()
{
	HM_BUTTONS_INIT();
}


////////////////////////////////////////////////////////////////////////////////////////////////
// * Use for buttons that need to be bebounced.
// * Set any function pointer to NULL if you are not interested in the respective signal.
// * Set long_press_repeat_timeout_ms to 0 to leave out logic for repeats AND long press end.
// * Set long_press_timeout_ms to 0 to only receive button down.
// * By inlining and optimizing, the compiler will generate more or less slim code from this.
////////////////////////////////////////////////////////////////////////////////////////////////

hm_button_slr_buffer_t btn_slr_buffer[HM_BUTTON_SLR_BUFFER_COUNT];

static inline __attribute__((always_inline)) void hm_button_task_short_long_rpt(
	uint8_t btn_slr_buffer_pos, bool btn_state_now, 
	uint16_t debounce_timeout_ms, uint16_t long_press_timeout_ms, uint16_t long_press_repeat_timeout_ms, 
	void (*signal_short_click)(), void (*signal_long_press_start)(), void (*signal_long_press_repeat)(), 
	void (*signal_long_press_end)(), void (*signal_button_down)())
{
	hm_button_slr_buffer_t *buffer = &(btn_slr_buffer[btn_slr_buffer_pos]);
	bool detect_long_press = long_press_timeout_ms;
	bool detect_long_repeat_end = long_press_repeat_timeout_ms;

	// ignore button state while counter debounce ignore is running
	if (btn_state_now != buffer->btn_state_last && buffer->btn_ctr_debounce_ignore == 0)
	{
		// start counter debounce ignore
		buffer->btn_ctr_debounce_ignore = HM_TIMER_TICKS_FROM_MS(debounce_timeout_ms);
			
		// button pressed
		if (btn_state_now)
		{
			if (detect_long_press)
				// start counter long
				buffer->btn_ctr_long = HM_TIMER_TICKS_FROM_MS(long_press_timeout_ms) + 1;
			if (signal_button_down)
				// signal button down
				signal_button_down();
		}
		else if (detect_long_press)
		{
			// if counter long is still running, it was a short click
			if (buffer->btn_ctr_long)
			{
				if (signal_short_click)
					signal_short_click();
			}
			// if counter repeat is already running, we might be interested in the end of the long press
			if (detect_long_repeat_end && buffer->btn_ctr_long_rpt)
			{
				if (signal_long_press_end)
					signal_long_press_end();
			}
			// reset these
			buffer->btn_ctr_long = buffer->btn_ctr_long_rpt = 0;
		}
		
		// save button state
		buffer->btn_state_last = btn_state_now;
	}
	
	// if counter long is about to end, start counter long repeat and signal long press (start)
	if (detect_long_press && buffer->btn_ctr_long == 1)
	{
		if (detect_long_repeat_end)
			buffer->btn_ctr_long_rpt = HM_TIMER_TICKS_FROM_MS(long_press_repeat_timeout_ms) + 1;
		if (signal_long_press_start)
			signal_long_press_start();
	}
	
	// restart before we loose state and signal long repeat
	if (detect_long_repeat_end && buffer->btn_ctr_long_rpt == 1)
	{
		buffer->btn_ctr_long_rpt = HM_TIMER_TICKS_FROM_MS(long_press_repeat_timeout_ms) + 1;
		if (signal_long_press_repeat)
			signal_long_press_repeat();
	}
	
	// decrement counters if in use
	if (buffer->btn_ctr_debounce_ignore)
		buffer->btn_ctr_debounce_ignore--;
	if (detect_long_press && buffer->btn_ctr_long)
		buffer->btn_ctr_long--;
	if (detect_long_repeat_end && buffer->btn_ctr_long_rpt)
		buffer->btn_ctr_long_rpt--;
}	
	
void hm_buttons_timer()
{
	HM_BUTTONS_TASK();
}
