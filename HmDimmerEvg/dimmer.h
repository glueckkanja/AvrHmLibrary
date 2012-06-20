#ifndef DIMMER_H_
#define DIMMER_H_

#include <stdint.h>
#include <stdbool.h>


#define DIM_CYCLE_DURATION_MS 5000


extern int8_t dim_level_percent;


void dimmer_init();
void dimmer_set(int8_t percent);


static inline __attribute__((always_inline)) void dimmer_change_ms(bool is_up, int16_t time_ms)
{
	uint16_t delta = 100 / (DIM_CYCLE_DURATION_MS / time_ms);
	
	if (is_up)
		dimmer_set(dim_level_percent + delta);
	else
		dimmer_set(dim_level_percent - delta);
}


#endif /* DIMMER_H_ */