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
    struct config Configfile;
    char lf[]="/herczig/Dokumentumok/log.txt";
    //float *temp;

    queueData *dataPacket;

    ReadConfig(Configfile);
    float values[TOPMEASURES][Configfile.numbOfDev]={{0}};

    if(Initalization(&old_term,term,&fd,Configfile,lf))
        return 1;
    queueInit(dataPacket);


    pthread_t reading_thread, controlling_thread,processor_thread;

    pthread_create(&controlling_thread,NULL,);
    pthread_create(&reading_thread,NULL,readingFromSerial,&fd);
    pthread_create(&processor_thread,NULL,takeoutFromQueue);







    return 0;
}
