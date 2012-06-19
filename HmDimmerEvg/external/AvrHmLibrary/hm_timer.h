/*
 * hm_timer.h
 *
 * Created: 15.06.2012 17:39:46
 *  Author: fstorm
 */ 


#ifndef HM_TIMER_H_
#define HM_TIMER_H_


#define HM_TIMER_TICKS_FROM_MS(ms) ((ms) / HM_TIMER_MS_PER_TICKS)


extern volatile uint8_t hm_timer_ticks;
extern volatile uint8_t hm_timer_secs;


void hm_timer_init();


static inline bool hm_timer_is_expired(uint8_t start_ticks, uint8_t timeout_ticks)
{
	uint8_t target_ticks = start_ticks + timeout_ticks;
	if (target_ticks >= HM_TIMER_MAX)
	{
		target_ticks -= HM_TIMER_MAX;
		return (target_ticks < hm_timer_ticks && hm_timer_ticks < start_ticks);
	}		

	return (hm_timer_ticks > target_ticks || hm_timer_ticks < start_ticks);
}


#endif /* HM_TIMER_H_ */