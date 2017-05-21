#include <stdio.h>
#include <stdlib.h>
#include <header/header.h>


/**
Moving hysteresis counting by the measured value with a delta.
*/
incoming_data *moving_hysteresis(struct config.Delta,struct incoming_data->data)
{
    int delta=config.Delta;
    int temp_min=0;
    int temp_max=delta;
    int value=incoming_data->data;
    incoming_data *result;
    result=malloc(sizeof(incoming_data));

    if(value<=temp_max)
    {
        if(value=>temp_min)
        {
            result->data=temp_min;
                return result->data;
        }
        else

        {
            temp_min=value;
            temp_max=temp_min+delta;
            result->data=temp_min;
                return  result.data;
        }
    }
    else
    {
            temp_max=value;
            temp_min=temp_max-delta;
            result->data=temp_min;
                return  result->data;

    }

}

/**
Moving average with 3 members in default
*/
int mov_average()
{



}
