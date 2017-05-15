#include <stdio.h>
#include <stdlib.h>



/**
Moving hysteresis counting by the measured value with a delta.
*/
int moving_hysteresis(const int delta,int value)
{
    int temp_min=0;
    int temp_max=delta;
    int result;

    if(value<=temp_max)
        if(value=>temp_min)
        {
            result=temp_min;
                return result;
        }
        else

        {
            temp_min=value;
            temp_max=temp_min+delta;
            result=temp_min;
                return  result;
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
Moving average with 3 members
*/
int mov_average(int values[])
{



}
