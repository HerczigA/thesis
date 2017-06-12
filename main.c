#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <termios.h>
#include "header/crc.h"
#include "header/header.h"
#include "header/reading.h"
#include "header/counting.h"


int main()
{
    int fd;
    typedef struct threadArg
{
    queueData *Packet;
    int *fd;
    config *conf;
};

    struct termios old_term,*term;
    config Configfile={0};
    queueData /*dataPacketIn, */dataPacketOut;
    pthread_t reading_thread, controlling_thread,processor_thread;
    threadArg *thrdArg=(threadArg*)malloc(sizeof(threadArg));
    //thrdArg->Packet=&dataPacketIn;
    thrdArg->conf=&Configfile;
    thrdArg->fd=&fd;

    ReadConfig(&Configfile);
    if(Initalization(&old_term,term,&fd,Configfile))
        return 1;
    queueInit(&thrdArg->Packet,&dataPacketOut);

    pthread_create(&controlling_thread,NULL,sendRequest,thrdArg);
    pthread_create(&reading_thread,NULL,readingFromSerial,thrdArg);
    pthread_create(&processor_thread,NULL,takeoutFromQueue,thrdArg);

    pthread_join(controlling_thread,NULL);
    pthread_join(reading_thread,NULL);
    pthread_join(processor_thread,NULL);

    free(thrdArg);

    return 0;
}
