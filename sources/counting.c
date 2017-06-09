#include <stdio.h>
#include <stdlib.h>
#include <header/header.h>
#define TIMELINE 9
/**
Moving hysteresis counting by the measured value with a delta.
*/
float moving_hysteresis(threadArg *arg,float *temp)
{
   const float delta=arg->conf->Delta;
    float temp_min=ZERO;
    float temp_max=delta;
    float value=*temp;
    float result;
    result=(float*)malloc(sizeof(float));

    if(value<=temp_max)
    {
        if(value=>temp_min)
        {
            result=value;
                return result;
        }
        else

        {
            temp_min=value;
            result=temp_min;
                return  result;
        }
    }
    else
    {
            temp_max=value;
            temp_min=temp_max-delta;
            result=temp_min;
                return  result;

    }

}



/**
Moving average with 3 members in default
*/
int mov_average(int *tempArray, float *data, int i, threadArg *arg, queueData *tempData)
{

    *data -= tempArray[i] + tempData->data;
    tempArray[i] = tempData->data;
        return *data / arg->conf->members;
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
