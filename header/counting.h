#ifndef COUNTING_H_INCLUDED
#define COUNTING_H_INCLUDED
#define lf "/herczig/Dokumentumok/log"

float moving_hysteresis(threadArg *arg,float *temp);
int mov_average(int *tempArray, float *data, int i, threadArg *arg, queueData *tempData);
char *timeToString(char *buffer);

#endif // COUNTING_H_INCLUDED
