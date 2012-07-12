/*
 * hm_buttons.c
 *
 * Created: 19.06.2012 18:03:24
 *  Author: fstorm
 */ 

#include "hm_main.h"


hm_button_buffer_t btn_buffer[HM_BUTTONS_BUFFER_COUNT];


void hm_buttons_init()
{
	HM_BUTTONS_INIT();
}


////////////////////////////////////////////////////////////////////////////////////////////////
// * Use for normal buttons that need to be bebounced.
// * Set any function pointer to NULL if you are not interested in the respective signal.
// * Set long_press_repeat_timeout_ms to 0 to leave out logic for repeats AND long press end.
// * Set long_press_timeout_ms to 0 to only receive button down.
// * By inlining and optimizing, the compiler will generate more or less slim code from this.
////////////////////////////////////////////////////////////////////////////////////////////////

static inline __attribute__((always_inline)) void hm_button_task_debounce(
	uint8_t btn_slr_buffer_pos, bool btn_state_now, 
	uint16_t debounce_timeout_ms, uint16_t long_press_timeout_ms, uint16_t long_press_repeat_timeout_ms, 
	void (*signal_short_click)(), void (*signal_long_press_start)(), void (*signal_long_press_repeat)(), 
	void (*signal_long_press_end)(), void (*signal_button_down)())
{
	hm_button_buffer_t *buffer = &(btn_buffer[btn_slr_buffer_pos]);
	bool detect_long_press = long_press_timeout_ms;
	bool detect_long_repeat_end = long_press_repeat_timeout_ms;

	// ignore button state while counter debounce ignore is running
	if (btn_state_now != buffer->btn_state_last && buffer->btn_ctr_smoothing == 0)
	{
		// start counter debounce ignore
		buffer->btn_ctr_smoothing = HM_TIMER_TICKS_FROM_MS(debounce_timeout_ms);
			
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
			// if counter long is still running -> it was a short click
			if (buffer->btn_ctr_long)
			{
				if (signal_short_click)
					signal_short_click();
			}
			// if counter repeat is already running -> we might be interested in the end of the long press
			if (detect_long_repeat_end && buffer->btn_ctr_long_rpt)
			{
				if (signal_long_press_end)
					signal_long_press_end();
			}
			// reset both
			buffer->btn_ctr_long = buffer->btn_ctr_long_rpt = 0;
		}
		
		// save button state
		buffer->btn_state_last = btn_state_now;
	}
	
	// if counter long is elapsed, start counter long repeat and signal long press (start)
	if (detect_long_press && buffer->btn_ctr_long == 1)
	{
		if (detect_long_repeat_end)
			buffer->btn_ctr_long_rpt = HM_TIMER_TICKS_FROM_MS(long_press_repeat_timeout_ms) + 1;
		if (signal_long_press_start)
			signal_long_press_start();
	}
	
	// if counter long repeat is elapsed, restart and signal long repeat
	if (detect_long_repeat_end && buffer->btn_ctr_long_rpt == 1)
	{
		buffer->btn_ctr_long_rpt = HM_TIMER_TICKS_FROM_MS(long_press_repeat_timeout_ms) + 1;
		if (signal_long_press_repeat)
			signal_long_press_repeat();
	}
	
	// decrement counters if in use
	if (buffer->btn_ctr_smoothing)
		buffer->btn_ctr_smoothing--;
	if (detect_long_press && buffer->btn_ctr_long)
		buffer->btn_ctr_long--;
	if (detect_long_repeat_end && buffer->btn_ctr_long_rpt)
		buffer->btn_ctr_long_rpt--;
}	
	

////////////////////////////////////////////////////////////////////////////////////////////////
// * Use for buttons that return "pressed" only during one half-wave.
// * Set any function pointer to NULL if you are not interested in the respective signal.
// * Set long_press_repeat_timeout_ms to 0 to leave out logic for repeats AND long press end.
// * Set long_press_timeout_ms to 0 to only receive button down.
// * By inlining and optimizing, the compiler will generate more or less slim code from this.
////////////////////////////////////////////////////////////////////////////////////////////////

static inline __attribute__((always_inline)) void hm_button_task_halfwave(
	uint8_t btn_slr_buffer_pos, bool btn_state_now, 
	uint16_t halfwave_timeout_ms, uint16_t long_press_timeout_ms, uint16_t long_press_repeat_timeout_ms, 
	void (*signal_short_click)(), void (*signal_long_press_start)(), void (*signal_long_press_repeat)(), 
	void (*signal_long_press_end)(), void (*signal_button_down)())
{
	hm_button_buffer_t *buffer = &(btn_buffer[btn_slr_buffer_pos]);
	bool detect_long_press = long_press_timeout_ms;
	bool detect_long_repeat_end = long_press_repeat_timeout_ms;

	// make sure half-wave counter is off whenever "pressed" is received
	if (btn_state_now && buffer->btn_ctr_smoothing)
		buffer->btn_ctr_smoothing = 0;
	
	if (btn_state_now != buffer->btn_state_last)
	{
		// button pressed
		if (btn_state_now)
		{
			if (detect_long_press)
				// start counter long
				buffer->btn_ctr_long = HM_TIMER_TICKS_FROM_MS(long_press_timeout_ms) + 1;
			if (signal_button_down)
				// signal button down
				signal_button_down();
			// save button state
			buffer->btn_state_last = btn_state_now;
		}
		else
		{
			//  half-wave counter
			switch (buffer->btn_ctr_smoothing)
			{
				case 0:	// off -> either button released or just negative half-wave -> start counter
					buffer->btn_ctr_smoothing = HM_TIMER_TICKS_FROM_MS(halfwave_timeout_ms);
					break;
					
				default: // elapsing -> stay still and wait ;-)
					break;

				case 1: // elapsed -> button really released
					if (detect_long_press)
					{
						// if counter long is still running -> it was a short click
						if (buffer->btn_ctr_long)
						{
							if (signal_short_click)
								signal_short_click();
						}
						// if counter repeat is already running -> we might be interested in the end of the long press
						if (detect_long_repeat_end && buffer->btn_ctr_long_rpt)
						{
							if (signal_long_press_end)
								signal_long_press_end();
						}
						// reset both
						buffer->btn_ctr_long = buffer->btn_ctr_long_rpt = 0;
					}
					// save button state
					buffer->btn_state_last = btn_state_now;
					break;
			}
		}			
	}
	
	// if counter long is elapsed, start counter long repeat and signal long press (start)
	if (detect_long_press && buffer->btn_ctr_long == 1)
	{
		if (detect_long_repeat_end)
			buffer->btn_ctr_long_rpt = HM_TIMER_TICKS_FROM_MS(long_press_repeat_timeout_ms) + 1;
		if (signal_long_press_start)
			signal_long_press_start();
	}
	
	// if counter long repeat is elapsed, restart and signal long repeat
	if (detect_long_repeat_end && buffer->btn_ctr_long_rpt == 1)
	{
		buffer->btn_ctr_long_rpt = HM_TIMER_TICKS_FROM_MS(long_press_repeat_timeout_ms) + 1;
		if (signal_long_press_repeat)
			signal_long_press_repeat();
	}
	
	// decrement counters if in use
	if (buffer->btn_ctr_smoothing)
		buffer->btn_ctr_smoothing--;
	if (detect_long_press && buffer->btn_ctr_long)
		buffer->btn_ctr_long--;
	if (detect_long_repeat_end && buffer->btn_ctr_long_rpt)
		buffer->btn_ctr_long_rpt--;
}	


#ifdef HM_BUTTONS_GIRA_BUFFER_SIZE

volatile uint8_t hm_buttons_gira_adc_values[HM_BUTTONS_GIRA_BUFFER_SIZE] = {255, 255, 255, 255, 255};	// make sure we have a few samples to start with state "open"

////////////////////////////////////////////////////////////////////////////////////////////////
// * Use for "Gira System 2000 Nebenstelle"
// * Set long_press_repeat_timeout_ms to 0 to leave out logic for repeats AND long press end.
// * Set long_press_timeout_ms to 0 to only receive button down.
// * Set signal_button_down to 1 only if "button_down" should be signalled.
// * By inlining and optimizing, the compiler will generate more or less slim code from this.
////////////////////////////////////////////////////////////////////////////////////////////////

static inline __attribute__((always_inline)) void hm_button_task_gira_nebenstelle(uint8_t btn_slr_buffer_pos, 
	uint16_t long_press_timeout_ms, uint16_t long_press_repeat_timeout_ms, 
	bool signal_button_down, void (*signal_event)(hm_button_signal_type_t, hm_button_gira_nebenstelle_state_t))
{
	hm_button_buffer_t *buffer = &(btn_buffer[btn_slr_buffer_pos]);
	hm_button_gira_nebenstelle_state_t btn_state_now;
	bool detect_long_press = long_press_timeout_ms;
	bool detect_long_repeat_end = long_press_repeat_timeout_ms;
	
	const uint8_t adc_limit_open_lower = 125;
	const uint8_t adc_limit_lower_upper = 85;
	const uint8_t adc_limit_upper_both = 47;
	const uint8_t adc_limit_both_shorted = 13;
	uint8_t gira_adc_value_max = 0;
	for (uint8_t i = 0; i < sizeof(hm_buttons_gira_adc_values); i++)
	{
		if (hm_buttons_gira_adc_values[i] > gira_adc_value_max)
			gira_adc_value_max = hm_buttons_gira_adc_values[i];
	}
	if (gira_adc_value_max > adc_limit_open_lower)
		btn_state_now = open;
	else if (gira_adc_value_max > adc_limit_lower_upper)
		btn_state_now = lower;
	else if (gira_adc_value_max > adc_limit_upper_both)
		btn_state_now = upper;
	else if (gira_adc_value_max > adc_limit_both_shorted)
		btn_state_now = both;
	else
		btn_state_now = shorted;

	if (btn_state_now != buffer->btn_state_last)
	{
		// button pressed
		if (btn_state_now != open)
		{
			if (detect_long_press)
				// start counter long
				buffer->btn_ctr_long = HM_TIMER_TICKS_FROM_MS(long_press_timeout_ms) + 1;
			if (signal_button_down)
				// signal button down
				signal_event(button_down, btn_state_now);
		}
		else
		{
			if (detect_long_press)
			{
				// if counter long is still running -> it was a short click
				if (buffer->btn_ctr_long)
				{
					signal_event(short_click, buffer->btn_state_last);
				}
				// if counter repeat is already running -> we might be interested in the end of the long press
				if (detect_long_repeat_end && buffer->btn_ctr_long_rpt)
				{
					signal_event(long_press_end, buffer->btn_state_last);
				}
				// reset both
				buffer->btn_ctr_long = buffer->btn_ctr_long_rpt = 0;
			}
		}			
		// save button state
		buffer->btn_state_last = btn_state_now;
	}
	
	// if counter long is elapsed, start counter long repeat and signal long press (start)
	if (detect_long_press && buffer->btn_ctr_long == 1)
	{
		if (detect_long_repeat_end)
			buffer->btn_ctr_long_rpt = HM_TIMER_TICKS_FROM_MS(long_press_repeat_timeout_ms) + 1;
		signal_event(long_press_start, btn_state_now);
	}
	
	// if counter long repeat is elapsed, restart and signal long repeat
	if (detect_long_repeat_end && buffer->btn_ctr_long_rpt == 1)
	{
		buffer->btn_ctr_long_rpt = HM_TIMER_TICKS_FROM_MS(long_press_repeat_timeout_ms) + 1;
		signal_event(long_press_repeat, btn_state_now);
	}
	
	// decrement counters if in use
	if (detect_long_press && buffer->btn_ctr_long)
		buffer->btn_ctr_long--;
	if (detect_long_repeat_end && buffer->btn_ctr_long_rpt)
		buffer->btn_ctr_long_rpt--;
}	

#endif /* HM_BUTTONS_GIRA_BUFFER_SIZE */


void hm_buttons_timer()
{
	HM_BUTTONS_TASK();
}
