#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <termios.h>
#define NUM_THREADS 3
#include "header/crc.h"
#include "header/header.h"
#include "header/reading.h"

int main()
{
    int fd;
    struct termios old_term,*term;
    config Configfile={0};
    queueData dataPacketIn, dataPacketOut;
    pthread_t reading_thread, controlling_thread,processor_thread;
    threadArg *thrdArg=(threadArg*)malloc(sizeof(threadArg));
    ReadConfig(&Configfile);

    if(Initalization(&old_term,term,&fd,Configfile,lf))
        return 1;
    queueInit(&dataPacket,&dataPacketOut);




    pthread_create(&controlling_thread,NULL,);
    pthread_create(&reading_thread,NULL,readingFromSerial,&fd);
    pthread_create(&processor_thread,NULL,takeoutFromQueue,);

    pthread_join(controlling_thread,NULL);
    pthread_join(reading_thread,NULL);
    pthread_join(processor_thread,NULL);
    free(thrdArg);
    return 0;
}
