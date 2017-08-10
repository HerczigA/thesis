#ifndef COUNTING_H_INCLUDED
#define COUNTING_H_INCLUDED
#include "measuring.h"



float moving_hysteresis(Threadcommon *arg,float temp);
float mov_average(movAverage *temp,Threadcommon *arg,int number);
char *timeToString(char *buffer);

#endif // COUNTING_H_INCLUDED
