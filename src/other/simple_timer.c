#include <other/simple_timer.h>
#include <time.h>

void simple_timer_start(simple_timer_t* timer)
{
    timer->start = (uint64_t)clock();
}

void simple_timer_stop(simple_timer_t* timer)
{
    timer->stop = (uint64_t)clock();
}

double simple_timer_get_time(simple_timer_t* timer)
{
    return (double)(timer->stop - timer->start) / CLOCKS_PER_SEC;
}

double simple_timer_measure(void (*func)())
{
    uint64_t start = (uint64_t)clock();
    
    func();

    uint64_t stop = (uint64_t)clock();
    return (double)(stop - start) / CLOCKS_PER_SEC;
}