#include <stdio.h>
#include <stdlib.h>
#include "../header/Init.h"
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
    int tempaddress;
    movAverage *devices=NULL;
    float temp;
    int loop=1;
    float finalResult;
    int i=0,j;
    QueueData *tempPacket=NULL;
    devices=(movAverage*)malloc(common->numbOfDev*sizeof(movAverage));
    if(!devices)
        {
            syslog(LOG_ERR,"Can not allocate memory for devices struct\n");
            exit(EXIT_FAILURE);
        }
     /**Malloc for the devices by number of member*/
    while(i<common->numbOfDev)
        {
            devices[i].k_element=(float*)malloc(common->members*sizeof(float));
            if(!devices[i].k_element)
                {
                    syslog(LOG_ERR,"Can not allocate memory for elements of devices\n");
                    exit(EXIT_FAILURE);
                }
            i++;
        }
    i=0;
    /**Init the allocated elements*/
    while(i<common->numbOfDev)
        {
            for(j=0; j<common->members; j++)
                devices[i].k_element[j]=0.0;
            devices[i].summary=0.0;
            i++;
        }
    i=0;
    while(loop)
        {
            if(!TAILQ_EMPTY(&common->head))
                {
                    pthread_mutex_lock(&common->mutex);
                    tempPacket=(QueueData *)TAILQ_FIRST(&common->head);
                    TAILQ_REMOVE(&common->head,tempPacket,entries);
                    pthread_mutex_unlock(&common->mutex);
                    for(tempaddress=0; tempaddress!=(int)tempPacket->address-1;tempaddress++)
                        ;
                    tempaddress--;
                    devices[tempaddress].k_element[ZERO]=atof(tempPacket->data);
                    temp=mov_average(&devices[tempaddress],common->members);
                    finalResult=moving_hysteresis(common->Delta,temp);
                    printf("Measured temperature from %s with moving average and moving hysteresis :%.2f°C\n",
                           common->sensors[(int)tempPacket->address-1].names,finalResult);
                    syslog(LOG_INFO,"Measured temperature from %d address of device with moving average and moving hysteresis :%.2f°C"
                           ,tempPacket->address,finalResult);
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
    while(i<common->numbOfDev)
        {
            for(j=0; j< (int)common->members; j++)
                free(&devices[i].k_element[j]);
            free(&devices[i]);
            i++;
        }
}
