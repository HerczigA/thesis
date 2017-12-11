#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/Init.h"
#include "../header/reading.h"
#include "../header/processing.h"
/**
Moving hysteresis counting by the measured value with a delta.
*/
float moving_hysteresis(float Delta,movAverage *temp)
{
    float result;

    if(temp->measuredValue<=temp->act_max_value)
        {
            if(temp->measuredValue >= temp->act_min_val)
                {
                    result=temp->act_min_val;
                    return result;
                }
            else
                {
                    temp->act_min_val=temp->measuredValue;
                    temp->act_max_value=temp->act_min_val+Delta;
                    result=temp->act_min_val;
                    return result;
                }
        }
    else
        {
            temp->act_max_value=temp->measuredValue;
            temp->act_min_val=temp->act_max_value-Delta;
            result=temp->act_min_val;
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
    int i,j;
    /**Members are odd */
    if(members%2)
        {
            for(i=0; i<members; i++)
                sum+=temp->k_element[i];
            for(j=0; j<members-1; j++)
                temp->k_element[j+1]=temp->k_element[j];
            result=sum/(float)members;
        }
    else
        {
            for(i=0; i<members; i++)
                sum+=(temp->k_element[i]);
            for(j=0; j<members-1; j++)
                temp->k_element[j+1]=temp->k_element[j];
            sum=sum/(float)members;
            result=(sum+temp->summary)/((float)members/2.0);
            temp->summary=sum;
        }
    return result;
}
