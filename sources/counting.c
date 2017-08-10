#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/header.h"
#include "../header/reading.h"
#include "../header/measuring.h"
#define TIMELINE 9
#define ZERO 0
/**
Moving hysteresis counting by the measured value with a delta.
*/
float moving_hysteresis(Threadcommon *arg,float temp)
{
    const float delta=arg->Delta;
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
            temp_min=temp;
            result=temp_min+delta;
            return  result;
        }
    }
    else
    {
        temp_max=temp;
        temp_min=temp_max-delta;
        result=temp_min;
        return  result;

    }

}



/**
Moving average with 3 members in default
*/
float mov_average(movAverage *temp,Threadcommon *arg,int number)
{
    float result;
    float sum;
    if(arg->members ==3.0)
    {
            sum=temp->k+temp->k_next+temp->k_prev;


    temp[number]->k_prev=temp[number]->k;
    temp[number]->k=temp[number]->k_next;
    result=sum/arg->member;

    }
    else
    {
        sum=(temp[number]->k+temp[number]->k_next+temp[number]->k_prev+temp[number]->k_fourth)/arg->member;
        result=(sum+temp[number]->summary)/(arg->member/2.0);
        temp[number]->k_prev=temp[number]->k;
        temp[number]->k=temp[number]->k_fourth;
        temp[number]->k_fourth=temp[number]->k_next;
        temp[number]->summary=sum;
    }
    return result;
}

char *timeToString(char *buffer)
{
    time_t now=time(NULL);
    char filename[]="_log.txt";
    struct tm *timedate;
    timedate=localtime(&now);
    strftime(buffer,TIMELINE,"%x",timedate);
    strcat(buffer,filename);

    return buffer;
}
