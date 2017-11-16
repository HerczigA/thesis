#define _POSIX_C_SOURCE 199506L
#include <stdio.h>
#include <pthread.h>
#include "../header/crc.h"
#include "../header/header.h"
#include "../header/reading.h"
#include "../header/counting.h"
#include "../header/measuring.h"
#include "../header/test.h"
#include <signal.h>



void threads(pthread_t *t1,pthread_t *t2,pthread_t *t3,Threadcommon *arg)
{
    Threadcommon *common=arg;
    if(pthread_create(t1,NULL,(void*)sendRequest,common))
        {
            syslog(LOG_ERR,"%s",strerror(errno));
            return;

        }
    if(pthread_create(t2,NULL,(void*)readingFromSerial,common))
        {
            syslog(LOG_ERR,"%s",strerror(errno));
            return;

        }
    if(pthread_create(t3,NULL,(void*)takeoutFromQueue,common))
        {
            syslog(LOG_ERR,"%s",strerror(errno));
            return;

        }
    pthread_kill(*t1,0);
    pthread_kill(*t2,0);
    pthread_join(*t1,NULL);
    pthread_join(*t2,NULL);
    pthread_join(*t3,NULL);

}
