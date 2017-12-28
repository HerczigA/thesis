#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include "../header/reading.h"
#include "../header/writing.h"
#include "../header/sending.h"
#define MAXREQUEST 30


volatile static bool loop=true;

void sendRequest(void *arg)
{

    if(!arg)
        {
            syslog(LOG_ERR,"Threadhandle is NULL");
            return;
        }
    Threadcommon *common=arg;
    unsigned char addresses=1;
    int requestType;
    int requestCounter=0;
    unsigned const char heartBit=PING;
    unsigned const char termBit=TERMCMD;
    Statistic packet;
    struct sigaction sig;
    sig.sa_handler=signalcatch;
    sig.sa_restorer=NULL;
    sig.sa_flags=0;

    int error;
    packet.send_TermPacket=0;
    packet.send_PollPacket=0;
    //char Data[]="6.72";
    //uint16_t DLEN=strlen(Data);
    char termData=0;
    char pollData=0;
    int len=0;

    sigaction(SIGINT,&sig,NULL);
    while(loop)
        {
            if(requestCounter==MAXREQUEST)
                requestCounter=0;
            requestType = requestCounter % 3;
            if(!requestType)
                {
                    while((int)addresses<=common->numbOfDev && common->loop)
                        {
                            sleep(common->sensors[(int)addresses-1].time);
                            if((error=sendPacket(common->fd,addresses, termBit, &termData,len))>0)
                                packet.send_TermPacket++;
                            else
                                {
                                    syslog(LOG_ERR,"ERROR at writing:%s",strerror(error));
                                    return;
                                }
                            addresses++;
                        }
                    requestCounter++;
                }
            else
                {
                    while((int)addresses<=common->numbOfDev)
                        {
                            sleep(common->pollTime);
                            if((error=sendPacket(common->fd,addresses, heartBit,&pollData,len))>0)
                                {
                                    pthread_mutex_lock(&common->watchdog_mutex);
                                    common->sensors[(int)addresses-1].watchdog++;
                                    if(common->sensors[(int)addresses-1].watchdog>=WATCHDOGMAX)
                                        {
                                            common->sensors[(int)addresses-1].state=0;
                                            printf("%s state is 0 \n don't got answers after %d tries\n"
                                                   ,common->sensors[(int)addresses-1].names,common->sensors[(int)addresses-1].watchdog);
                                            syslog(LOG_WARNING,"%s state is 0",common->sensors[(int)addresses-1].names);
                                        }
                                    pthread_mutex_unlock(&common->watchdog_mutex);
                                    packet.send_PollPacket++;
                                }
                            else
                                {
                                    syslog(LOG_ERR,"ERROR at writing:%s",strerror(error));
                                    return;
                                }
                            addresses++;
                        }
                    requestCounter++;
                }
            syslog(LOG_NOTICE,"Asking Term packet transmitted :%d",packet.send_TermPacket);
            syslog(LOG_NOTICE,"Asking Polling packet transmitted :%d",packet.send_PollPacket);
            addresses=1;
        }
    printf("sending thread is end\n");
    syslog(LOG_ERR,"sending thread is end");
    common->loop=false;
}


void signalcatch(int sig)
{

    loop=false;
    printf("signal received, I am exit\n");
    syslog(LOG_ERR,"Signal received:%d",sig);

}
