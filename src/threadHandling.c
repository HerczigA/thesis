#include <stdio.h>
#include <pthread.h>
#include "../header/crc.h"
#include "../header/Init.h"
#include "../header/reading.h"
#include "../header/counting.h"
#include "../header/processing.h"
#include "../header/sending.h"




void threadsHandling(pthread_t *thread_request,pthread_t *thread_read,pthread_t *thread_measure,Threadcommon *arg)
{
    Threadcommon *common=arg;
    if(pthread_create(thread_request,NULL,(void*)sendRequest,common))
        {
            syslog(LOG_ERR,"%s",strerror(errno));
            return ;
        }
   /* if(pthread_create(thread_read,NULL,(void*)readingFromSerial,common))
        {
            syslog(LOG_ERR,"%s",strerror(errno));
            return ;

        }*/
    if(pthread_create(thread_measure,NULL,(void*)takeoutFromQueue,common))
        {
            syslog(LOG_ERR,"%s",strerror(errno));
            return;

        }
    pthread_join(*thread_request,NULL);
    //pthread_join(*thread_read,NULL);
    pthread_join(*thread_measure,NULL);
}
