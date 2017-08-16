#ifndef COUNTING_H_INCLUDED
#define COUNTING_H_INCLUDED
#include "measuring.h"
#define TIMELINE 9
#define ZERO 0


float moving_hysteresis(Threadcommon *arg,float temp);
float mov_average(movAverage *temp,int members);
char *timeToString(char *buffer);

#endif // COUNTING_H_INCLUDED
