#ifndef COUNTING_H_INCLUDED
#define COUNTING_H_INCLUDED
#define ZERO 0

float moving_hysteresis(config conffile,float *temp);
int mov_average(int *tempArray, float *data, int i, config conffile, queueData *tempData);

#endif // COUNTING_H_INCLUDED
