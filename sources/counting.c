#include <stdio.h>
#include <stdlib.h>
#include <header/header.h>

/**
Moving hysteresis counting by the measured value with a delta.
*/
float moving_hysteresis(config conffile,float *temp)
{
   const float delta=conffile.Delta;
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
int mov_average(int *tempArray, float *data, int i, config conffile, queueData *tempData)
{

    *data -= tempArray[i] + tempData->data;
    tempArray[i] = tempData->data;
        return *data / conffile.members;
}
