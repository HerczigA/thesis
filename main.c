#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "./header/config.h"
#include "./header/crc.h"
#include "./header/Init.h"
#include "./header/reading.h"
#include "./header/counting.h"
#include "./header/processing.h"
#include "./header/threadHandling.h"
#include "./header/sending.h"
#include "./header/closing.h"

int main()
{
    struct termios old_term,*term=NULL;
    pthread_t reading_thread, requesting_thread,processor_thread;
    Threadcommon threadHandle;
    threadHandle.sensors=NULL;
    if(config(&threadHandle))
        return -1;
    if(InitSerialPort(&old_term,term,&threadHandle))
        return -1;
    if(queueInit(&threadHandle))
        return -1;

    threadsHandling(&requesting_thread,&reading_thread,&processor_thread,&threadHandle);
    setBackTermios(&threadHandle,&old_term);

    return 0;
}

