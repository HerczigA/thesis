#include <stdio.h>
#include <stdlib.h>
#include "header/crc.h"
#include "header/header.h"
#include "header/reading.h"
#include "header/counting.h"
#include "header/measuring.h"

int main()
{
    struct termios old_term,*term;
    term=NULL;
    pthread_t reading_thread, requesting_thread,processor_thread;
    Threadcommon threadHandle;

    ReadConfig(&threadHandle);
    if(InitSerialPort(&old_term,term,&threadHandle))
        return -1;
    queueInit(&threadHandle);
    pthread_create(&requesting_thread,NULL,(void*)sendRequest,&threadHandle);
    pthread_create(&reading_thread,NULL,(void*)readingFromSerial,&threadHandle);
    pthread_create(&processor_thread,NULL,(void*)takeoutFromQueue,&threadHandle);
    pthread_join(requesting_thread,NULL);
    pthread_join(reading_thread,NULL);
    pthread_join(processor_thread,NULL);

    setBackTermios(&threadHandle,&old_term,term);
    return 0;
}
