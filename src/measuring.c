#include <stdio.h>
#include <stdlib.h>
#include "../header/header.h"
#include "../header/counting.h"
#include "../header/reading.h"

void takeoutFromQueue(void *arg)
{
    Threadcommon *common=arg;
    movAverage *devices=NULL;
    float temp;
    float finalResult;
    int i=1;
    openlog(NULL,LOG_PID,LOG_LOCAL1);
    QueueData *tempPacket=NULL;
    devices=(movAverage*)malloc(common->numbOfDev*sizeof(movAverage));
    if(!devices)
        {
            syslog(LOG_ERR,"Can not allocate memory for devices struct\n");
            exit(EXIT_FAILURE);
        }

    while (i<=common->numbOfDev)
        {
            devices[i].k=0;
            devices[i].k_prev=0;
            devices[i].k_next=0;
            devices[i].k_fourth=0;
            devices[i].summary=0;
            i++;
        }
    while(!TAILQ_EMPTY(&common->head))
        {
            pthread_mutex_lock(&common->mutex);
            tempPacket=(QueueData *)TAILQ_FIRST(&common->head);
            TAILQ_REMOVE(&common->head,tempPacket,entries);
            pthread_mutex_unlock(&common->mutex);

            devices[tempPacket->address].k_next=*(tempPacket->data);

            temp=mov_average(&devices[tempPacket->address],common->members);
            finalResult=moving_hysteresis(common,temp);

            syslog(LOG_INFO,"Measured temperature from %d address of device with moving average and moving hysteresis :%.2f\n",
                   tempPacket->address,finalResult);
            sleep(common->samplingTime);
        }

    closelog();
    free(devices);

}

