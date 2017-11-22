#include <stdio.h>
#include <stdlib.h>
#include "../header/reading.h"
#include "../header/writing.h"

static int loop=1;
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
    unsigned const char cmdTerm=0x01;
    Statistic packet;
    int error;
    packet.TermPacket=0;
    packet.pollPacket=0;
    char Data[]="6.72";
    uint16_t DLEN=strlen(Data);
    char pollData=0;
  //  signal(SIGINT,signalcatch);
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
                    if((error=sendPacket(common->fd,addresses, cmdTerm, Data,DLEN))>0)
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
            sleep(common->time);
            requestCounter++;
        }
        else
        {
            while((int)addresses<=common->numbOfDev)
            {
                if(common->sensors[(int)addresses-1].state)
                {
                    if((error=sendPacket(common->fd,addresses, heartBit,&pollData,DLEN))>0)
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
            sleep(common->time);
        }
        addresses=1;
    }
}
