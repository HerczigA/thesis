#ifndef MEASURING_H_INCLUDED
#define MEASURING_H_INCLUDED
#define TIMELINE 9
typedef struct m_average
{
  float k_prev;
  float k;
  float k_next;
  float k_fourth;
  float summary;

}movAverage;


int takeoutFromQueue(void *arg);

#endif // MEASURING_H_INCLUDED
