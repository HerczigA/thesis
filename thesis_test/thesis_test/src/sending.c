#include <stdio.h>
#include <stdlib.h>
#include "../header/reading.h"
#include "../header/writing.h"
#define MILTIME 10
#define MAXREQUEST 30
#include <signal.h>
#include "../header/sending.h"
 volatile static int loop=1;

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
    int error;
    packet.TermPacket=0;
    packet.pollPacket=0;
    //char Data[]="6.72";
    //uint16_t DLEN=strlen(Data);
    char termData=0;
    char pollData=0;
    int len=0;
    signal(SIGINT,signalcatch);

    while(loop)
        {
            if(requestCounter==MAXREQUEST)
                requestCounter=0;
            requestType = requestCounter % 3;
            if(!requestType)
                {
                    while((int)addresses<=common->numbOfDev && common->loop)
                        {
                            sleep(common->sensors[(int)addresses-1].time * MILTIME);
                            if((error=sendPacket(common->fd,addresses, termBit, &termData,len))>0)
                                {
                                    packet.TermPacket++;
                                    syslog(LOG_NOTICE,"Asking Term packet transmitted :%d",packet.TermPacket);
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
                                    packet.pollPacket++;
                                    syslog(LOG_NOTICE,"Asking Polling packet transmitted :%d",packet.pollPacket);
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
            addresses=1;
        }
      printf("%d\n",loop);
}


void signalcatch(int sig)
{
loop=0;
printf("signal received, I am exit\t %d\n",loop);
syslog(LOG_ERR,"Signal received");

}
