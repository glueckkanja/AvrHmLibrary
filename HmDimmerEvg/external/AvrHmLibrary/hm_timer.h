/*
 * hm_timer.h
 *
 * Created: 15.06.2012 17:39:46
 *  Author: fstorm
 */ 


#ifndef HM_TIMER_H_
#define HM_TIMER_H_


#define HM_TIMER_TICKS_FROM_MS(ms) ((ms) / HM_TIMER_MS_PER_TICK)


extern volatile uint8_t hm_timer_ticks;
extern volatile uint8_t hm_timer_secs;


void hm_timer_init();


static inline __attribute__((always_inline)) bool hm_timer_is_expired_ticks_ms(uint8_t start_ticks, uint16_t timeout_ms)
{
	uint8_t target_ticks = start_ticks + (uint8_t)HM_TIMER_TICKS_FROM_MS(timeout_ms);
	if (target_ticks >= HM_TIMER_TICKS_MAX)
	{
		target_ticks -= HM_TIMER_TICKS_MAX;
		return (target_ticks < hm_timer_ticks && hm_timer_ticks < start_ticks);
	}		

	return (hm_timer_ticks > target_ticks || hm_timer_ticks < start_ticks);
}

static inline __attribute__((always_inline)) bool hm_timer_is_expired_secs(uint8_t start_secs, uint8_t timeout_secs)
{
	uint8_t target_secs = start_secs + timeout_secs;
	if (target_secs >= HM_TIMER_SECS_MAX)
	{
		target_secs -= HM_TIMER_SECS_MAX;
		return (target_secs < hm_timer_secs && hm_timer_secs < start_secs);
	}		

	return (hm_timer_secs > target_secs || hm_timer_secs < start_secs);
}


#endif /* HM_TIMER_H_ */