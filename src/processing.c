#include <stdio.h>
#include <stdlib.h>
#include "../header/Init.h"
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
    int tempaddress;
    movAverage *devices=NULL;
    float finalResult;
    int i=0,j;
    QueueData *tempPacket=NULL;
    devices=(movAverage*)malloc(common->numbOfDev*sizeof(movAverage));
    if(!devices)
        {
            syslog(LOG_ERR,"Can not allocate memory for devices struct\n");
            pthread_exit(NULL);
        }
    /**Malloc for the devices by number of member*/
    while(i<common->numbOfDev)
        {
            devices[i].k_element=(float*)malloc(common->sensors[i].movAve_tag_number*sizeof(float));
            if(!devices[i].k_element)
                {
                    free(devices);
                    syslog(LOG_ERR,"Can not allocate memory for elements of devices\n");
                    pthread_exit(NULL);
                }
            i++;
        }
    i=0;
    /**Init the allocated elements*/
    while(i<common->numbOfDev)
        {
            for(j=0; j<common->sensors[i].movAve_tag_number; j++)
                devices[i].k_element[j]=ZEROFLOAT;
            devices[i].summary=ZEROFLOAT;
            devices[i].act_min_val=ZEROFLOAT;
            devices[i].act_max_value=ZEROFLOAT+common->Delta;
            i++;
        }
    i=0;
    while(common->loop)
        {
            if(!TAILQ_EMPTY(&common->head))
                {
                    pthread_mutex_lock(&common->temperature_mutex);
                    tempPacket=TAILQ_FIRST(&common->head);
                    TAILQ_REMOVE(&common->head,tempPacket,entries);
                    pthread_mutex_unlock(&common->temperature_mutex);
                    for(tempaddress=0; tempaddress!=(int)tempPacket->address-1; tempaddress++)
                        ;
                        tempaddress--;
                    devices[tempaddress].k_element[ZERO]=atof(tempPacket->data);

                    devices[tempaddress].measuredValue=mov_average(&devices[tempaddress],common,tempaddress);

                    finalResult=moving_hysteresis(common->Delta,&devices[tempaddress]);
                    printf("Measured temperature from %s with moving average and moving hysteresis :%.2f°C\n",
                           common->sensors[(int)tempPacket->address-1].names,finalResult);
                    syslog(LOG_INFO,"Measured temperature from %d address of device with moving average and moving hysteresis :%.2f°C"
                           ,tempPacket->address,finalResult);
                    free(tempPacket->data);
                    free(tempPacket);
                    tempPacket=NULL;
                    sleep(common->samplingTime);
                }
        }
    while(i<common->numbOfDev)
        {
            free(devices[i].k_element);
            i++;
        }
    free(devices);
    printf("processing thread is end\n");
    syslog(LOG_ERR,"processing thread is end");
}
