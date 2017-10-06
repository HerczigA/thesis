#ifndef COUNTING_H_INCLUDED
#define COUNTING_H_INCLUDED
#include "measuring.h"
#define ZERO 0


float moving_hysteresis(float Delta,float temp);
float mov_average(movAverage *temp,int members);
char *timeToString(char *buffer);

#endif // COUNTING_H_INCLUDED
