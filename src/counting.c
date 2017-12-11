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
    float result=0.0;
    float sum;
    int i,j;
    int tagNumber;
    if(Tag->sensors[address].movAve_tag_number)
        tagNumber=Tag->sensors[address].movAve_tag_number;
    else
        tagNumber=Tag->members;
    /**tagNumber are odd */
    if(tagNumber%2)
        {
            for(i=0; i<tagNumber; i++)
                sum+=temp->k_element[i];
            for(j=0; j<tagNumber-1; j++)
                temp->k_element[j+1]=temp->k_element[j];
            result=sum/(float)tagNumber;
        }
    else
        {
            for(i=0; i<tagNumber; i++)
                sum+=(temp->k_element[i]);
            for(j=0; j<tagNumber-1; j++)
                temp->k_element[j+1]=temp->k_element[j];
            sum=sum/(float)tagNumber;
            result=(sum+temp->summary)/((float)tagNumber/2.0);
            temp->summary=sum;
        }
    return result;
}
