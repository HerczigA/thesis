#include <stdio.h>
#include <stdlib.h>
#include "../header/header.h"
#include "../header/counting.h"
#include "../header/reading.h"

void takeoutFromQueue(void *arg)
{
    Threadcommon *common=arg;
    if(!common)
        {
            syslog(LOG_ERR,"NULL pointer received\n");
            exit(EXIT_FAILURE);
        }
    movAverage *devices=NULL;
    float temp;
    float finalResult;
    int i=0;
    QueueData *tempPacket=NULL;
    devices=(movAverage*)malloc(common->numbOfDev*sizeof(movAverage));
    if(!devices)
        {
            syslog(LOG_ERR,"Can not allocate memory for devices struct\n");
            exit(EXIT_FAILURE);
        }

    while (i<common->numbOfDev)
        {
            devices[i].k=0;
            devices[i].k_prev=0;
            devices[i].k_next=0;
            devices[i].k_fourth=0;
            devices[i].summary=0;
            i++;
        }


    while(ON)
        {
            if(!TAILQ_EMPTY(&common->head))
                {
                    pthread_mutex_lock(&common->mutex);
                    tempPacket=(QueueData *)TAILQ_FIRST(&common->head);
                    TAILQ_REMOVE(&common->head,tempPacket,entries);
                    pthread_mutex_unlock(&common->mutex);
                    devices[(int)tempPacket->address].k_next=atof(tempPacket->data);

                    temp=mov_average(&devices[(int)tempPacket->address],common->members);
                    finalResult=moving_hysteresis(common->Delta,temp);

                    syslog(LOG_INFO,"Measured temperature from %d address of device with moving average and moving hysteresis :%.2f°C\n",
                           tempPacket->address,finalResult);
                    free(tempPacket->data);
                    free(tempPacket);
                    sleep(common->samplingTime);
                }
            else
                {

                    syslog(LOG_NOTICE,"QUEUE is EMPTY\n");
                    sleep(common->time);
                }

        }


    free(devices);

}

