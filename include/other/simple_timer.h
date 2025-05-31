#ifndef OTHERS_SIMPLE_TIMER_H_
#define OTHERS_SIMPLE_TIMER_H_

#include <stdint.h>

typedef struct {
    uint64_t start;
    uint64_t stop;
} simple_timer_t;

void simple_timer_start(simple_timer_t* timer);
void simple_timer_stop(simple_timer_t* timer);

double simple_timer_get_time(simple_timer_t* timer);
double simple_timer_measure(void (*func)());

#endif