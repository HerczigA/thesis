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
    struct termios old_term;
    config Configfile= {0};
    pthread_t reading_thread, requesting_thread,processor_thread;
    Threadcommon threadHandle;

    ReadConfig(&Configfile);
    if(Initalization(&old_term,&fd,Configfile))
        return -1;

    queueInit(&threadHandle);
    pthread_create(&requesting_thread,NULL,(void*)sendRequest,&Configfile);
    pthread_create(&reading_thread,NULL,(void*)readingFromSerial,&threadHandle);
    pthread_create(&processor_thread,NULL,(void*)takeoutFromQueue,&threadHandle);
    pthread_join(controlling_thread,NULL);
    pthread_join(reading_thread,NULL);
    pthread_join(processor_thread,NULL);

    return 0;
}
