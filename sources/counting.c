#include <stdio.h>
#include <stdlib.h>
#include <header/header.h>


/**
Moving hysteresis counting by the measured value with a delta.
*/
float *moving_hysteresis(struct config conffile,queueData *Packet)
{
   const float delta=conffile.Delta;
    float temp_min=ZERO;
    float temp_max=delta;
    float value=Packet->data;
    float *result;
    result=malloc(sizeof(float));

    if(value<=temp_max)
    {
        if(value=>temp_min)
        {
            *result=temp_min;
                return result;
        }
        else

        {
            temp_min=value;
            //temp_max=temp_min+delta;            ??????????
            *result=temp_min;
                return  result;
        }
    }
    else
    {
            temp_max=value;
            temp_min=temp_max-delta;
            *result=temp_min;
                return  result;

    }

}

float *arrayFilling(float value[][numbdev],float *temp,queueData *Packet)
{
    float tmpArray....soon

}



/**
Moving average with 3 members in default
*/
int mov_average()
{



}
