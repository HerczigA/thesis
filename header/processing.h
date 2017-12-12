#ifndef MEASURING_H_INCLUDED
#define MEASURING_H_INCLUDED
#include <unistd.h>
#include <syslog.h>
#include <sys/queue.h>
#include <string.h>
#define MAXFAIL 50
#define ZEROFLOAT 0.0

typedef struct m_average
{
    float summary;
    float *k_element;
    float act_min_val;
    float measuredValue;
    float act_max_value;
} movAverage;

void takeoutFromQueue(void *arg);

#endif // MEASURING_H_INCLUDED
