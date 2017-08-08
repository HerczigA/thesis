#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/queue.h>
#include <time.h>
#include <string.h>
#include "header/header.h"
#include "header/counting.h"
#include "../header/reading.h"

int takeoutFromQueue(void *arg)
{
    Threadcommon *common=arg;



    float *temp;
    float finalResult;
    int i=0;
    float data=0;
    queueData *tempPacket;
    time_t now;
    time(&now);
    char *time;

    time=(char*)malloc(TIMELINE*sizeof(char));
    time=timeToString(time);
    FILE * log_file=fopen(time,"w");

    while(!TAILQ_EMPTY(&common->head))
    {
        pthread_mutex_lock(&common->mutex);
        tempPacket=TAILQ_FIRST(&common->head);
        TAILQ_REMOVE(&common->head,tempPacket,entries);
        pthread_mutex_unlock(&common->mutex);

        if(tempPacket->data)
        {
            *temp=mov_average(movAverArray, &data, i, arg,tempPacket);
            i++;
            if (i>= arg->conf.members)
                i= 0;
            finalResult=moving_hysteresis(conffile,temp);
            fprintf(log_file,"Measured temperature from %s address of device with moving average and moving hysteresis :%d\t %s\n",
                    tempPacket->address,finalResult,ctime(&now));

        }
        sleep();
    }

    fclose(log_file);
    free(tempPacket);
    free(time);
}

