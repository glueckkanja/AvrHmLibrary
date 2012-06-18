#ifndef DIMMER_H_
#define DIMMER_H_

#include <stdint.h>
#include <stdbool.h>

extern int8_t dim_level_percent;

void dimmer_init();
void dimmer_set(int8_t percent);
void dimmer_change_ms(bool is_up, int16_t time_ms);
void dimmer_check_keys();


#endif /* DIMMER_H_ */