#ifndef COUNTING_H_INCLUDED
#define COUNTING_H_INCLUDED
#include "processing.h"
#include "Init.h"
#define ZERO 0


float moving_hysteresis(float Delta,movAverage *temp);

float mov_average(movAverage *temp,Threadcommon *Tag,int address);

char *timeToString(char *buffer);

#endif // COUNTING_H_INCLUDED
