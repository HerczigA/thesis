#include <stdio.h>
#include <stdlib.h>
#include "../header/reading.h"
#include "../header/writing.h"
#define MILTIME 10
#define MAXREQUEST 30


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
    while(1)
        {
            if(requestCounter==MAXREQUEST)
                requestCounter=0;
            requestType = requestCounter % 3;
            if(!requestType)
                {
                    while((int)addresses<=common->numbOfDev)
                        {
                            if(common->sensors[(int)addresses-1].state)
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
                                }
                            addresses++;
                        }
                    requestCounter++;
                }
            else
                {
                    while((int)addresses<=common->numbOfDev)
                        {
                            if(common->sensors[(int)addresses-1].state)
                                {
                                    sleep(common->pollTime);
                                    if((error=sendPacket(common->fd,addresses, heartBit,&pollData,len))>0)
                                        {
                                            packet.pollPacket++;
                                            syslog(LOG_NOTICE,"Asking Polling packet transmitted :%d",packet.pollPacket);

                                        }
                                    else
                                        {
                                            syslog(LOG_ERR,"ERROR at writing:%s",strerror(error));
                                            return;
                                        }
                                }
                            addresses++;
                        }
                    requestCounter++;
                }
            addresses=1;
        }
}
