#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/header.h"
#include "../header/reading.h"
#include "../header/measuring.h"
#define TIMELINE 9
/**
Moving hysteresis counting by the measured value with a delta.
*/
float moving_hysteresis(float Delta,float temp)
{
    float delta=Delta;
    float temp_min=ZERO;
    float temp_max=delta;
    float result;

    if(temp<=temp_max)
        {
            if(temp>=temp_min)
                {
                    result=temp;
                    return result;
                }
            else

                {
                    result=temp+delta;
                    return result;
                }
        }
    else
        {

            result=temp-delta;
            return  result;

        }

}



/**
Moving average with 3 members in default
*/
float mov_average(movAverage *temp,int members)
{
    float result=0.0;
    float sum;
    if(members==3)
        {
            sum=temp->k+temp->k_next+temp->k_prev;


            temp->k_prev=temp->k;
            temp->k=temp->k_next;
            result=sum/(float)members;

        }
    else
        {
            sum=(temp->k+temp->k_next+temp->k_prev+temp->k_fourth)/(float)members;
            result=(sum+temp->summary)/((float)members/2.0);
            temp->k_prev=temp->k;
            temp->k=temp->k_fourth;
            temp->k_fourth=temp->k_next;
            temp->summary=sum;
        }
    return result;
}

char *timeToString(char *buffer)
{
    if (!buffer)
        return NULL;
    time_t now=time(NULL);
    char filename[]="_log.txt";
    struct tm *timedate;
    timedate=localtime(&now);
    strftime(buffer,TIMELINE,"%x",timedate);
    strcat(buffer,filename);

    return buffer;
}
