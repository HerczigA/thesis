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
Moving average with 3 tagNumber in default
*/
float mov_average(movAverage *temp,Threadcommon *Tag,int address)
{
    float result,sum;
    result=sum=0.0;
    int i,j,k;
    int tagNumber;
    tagNumber=Tag->sensors[address].movAve_tag_number;
    k=tagNumber-1;
    /**tagNumber are odd */
    if(tagNumber%2)
    {
        for(i=0; i<tagNumber; i++)
            sum+=temp->k_element[i];
        for(j=0; j<tagNumber-1; j++)
        {
            temp->k_element[k]=temp->k_element[k-1];
            k--;
        }
        result=sum/(float)tagNumber;
    }
    else
    {
        for(i=0; i<tagNumber; i++)
            sum+=(temp->k_element[i]);
        for(j=0; j<tagNumber-1; j++)
        {
            temp->k_element[k]=temp->k_element[k-1];
            k--;
        }
        sum=sum/(float)tagNumber;
        result=(sum+temp->summary)/((float)tagNumber/2.0);
        temp->summary=sum;
    }
    return result;
}
