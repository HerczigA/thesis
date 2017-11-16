#include <stdio.h>
#include <stdlib.h>
#include "../header/header.h"
#include "../header/counting.h"
#include "../header/reading.h"


void takeoutFromQueue(void *arg)
{

    signal(SIGINT,signalcatch);
    Threadcommon *common=arg;
    if(!common)
        {
            syslog(LOG_ERR,"NULL pointer received\n");
            exit(EXIT_FAILURE);
        }
    movAverage *devices=NULL;
    float temp;
    int loop=1;
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
            devices[i].k=0.0;
            devices[i].k_prev=0.0;
            devices[i].k_next=0.0;
            devices[i].k_fourth=0.0;
            devices[i].summary=0.0;
            i++;
        }


    while(loop)
        {
            if(!TAILQ_EMPTY(&common->head))
                {
                    pthread_mutex_lock(&common->mutex);
                    tempPacket=(QueueData *)TAILQ_FIRST(&common->head);
                    TAILQ_REMOVE(&common->head,tempPacket,entries);
                    pthread_mutex_unlock(&common->mutex);
                    devices[((int)tempPacket->address)-1].k_next=atof(tempPacket->data);
                    temp=mov_average(&devices[((int)tempPacket->address-1)],common->members);
                    finalResult=moving_hysteresis(common->Delta,temp);

                    syslog(LOG_INFO,"Measured temperature from %d address of device with moving average and moving hysteresis :%.2f°C\n",tempPacket->address,finalResult);
                    free(tempPacket->data);
                    free(tempPacket);
                    tempPacket=NULL;
                    sleep(common->samplingTime);
                    loop=1;
                }
            else
                {
                    loop++;
                    if(loop==MAXFAIL)
                        {
                            loop=0;
                            syslog(LOG_ERR,"measuring finished\n");
                        }

                    syslog(LOG_NOTICE,"QUEUE is EMPTY\n");
                    sleep(common->samplingTime);
                }

        }

    free(devices);

}
