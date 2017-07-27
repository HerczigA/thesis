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

int takeoutFromQueue(threadArg *arg)
{
    int movAverArray[arg->conf.members]= {0};
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

    while(!TAILQ_EMPTY(&InHd))
    {
        pthread_mutex_lock(arg->Packet->mutex);     //second ->?
        tempPacket=TAILQ_FIRST(&InHd);
        TAILQ_REMOVE(&InHd,tempPacket,entries);
        pthread_mutex_unlock(arg->Packet->mutex);

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
        sleep(arg->conf.samplingTime);
    }

    fclose(log_file);
    free(tempPacket);
    free(time);
}

