#ifndef COUNTING_H_INCLUDED
#define COUNTING_H_INCLUDED
#define ZERO 0
#define lf "/herczig/Dokumentumok/log"

float moving_hysteresis(config conffile,float *temp);
int mov_average(int *tempArray, float *data, int i, config conffile, queueData *tempData);
char *timeToString(char *buffer);

#endif // COUNTING_H_INCLUDED
