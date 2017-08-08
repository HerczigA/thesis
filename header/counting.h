#ifndef COUNTING_H_INCLUDED
#define COUNTING_H_INCLUDED
#define lf "/herczig/Dokumentumok/log"

typedef struct m_average
{
  float k_prev;
  float k;
  float k_next;
  float k_fourth;

}movAverage;



float moving_hysteresis(Config *confile,float temp);
float mov_average(movAverage *temp);
float mov_averageTwo(movAverage *temp);
char *timeToString(char *buffer);

#endif // COUNTING_H_INCLUDED
