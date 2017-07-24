#include <stdio.h>
#include <stdlib.h>
#include "../header/header.h"
#define TIMELINE 9
#define ZERO 0
/**
Moving hysteresis counting by the measured value with a delta.
*/
float moving_hysteresis(threadArg *arg,float *temp)
{
    const float delta=arg->conf->Delta;
    float temp_min=ZERO;
    float temp_max=delta;
    float result;

    if(*temp<=temp_max)
    {
        if(*temp>=temp_min)
        {
            result=*temp;
            return result;
        }
        else

        {
            temp_min=*temp;
            result=temp_min+delta;
            return  result;
        }
    }
    else
    {
        temp_max=*temp;
        temp_min=temp_max-delta;
        result=temp_min;
        return  result;

    }

}



/**
Moving average with 3 members in default
*/
int mov_average()
{

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
