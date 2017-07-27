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
    struct termios old_term,*term;
    config Configfile= {0};
    queueData dataPacketOut,dataPacketIn;
    pthread_t reading_thread, controlling_thread,processor_thread;

    ReadConfig(&Configfile);
    if(Initalization(&old_term,term,&fd,Configfile))
        return 1;
    queueInit(&dataPacketIn,&dataPacketOut);





    pthread_create(&controlling_thread,NULL,sendRequest,thrdArg);
    pthread_create(&reading_thread,NULL,readingFromSerial,thrdArg);
    pthread_create(&processor_thread,NULL,takeoutFromQueue,thrdArg);
    pthread_join(controlling_thread,NULL);
    pthread_join(reading_thread,NULL);
    pthread_join(processor_thread,NULL);



    return 0;
}
