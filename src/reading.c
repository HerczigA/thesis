#include <stdio.h>
#include <stdlib.h>
#include "../header/Init.h"
#include "../header/crc.h"
#include "../header/reading.h"
#include <signal.h>
/**
Reading from the serial port. To check the incoming packet, use the Motorola protocol
*/
static int loop=1;
void  readingFromSerial(void *arg)
{
    QueueData *receivingData=NULL,
               *toQueueuPacket=NULL;
    unsigned char data;
    int i=0;
    int dataIndex;
    Crc packetCrc,calculateCrc;
    calculateCrc=packetCrc=0;
    Statistic Packetstatistic= {0};
    packetState State=EmptyState;
    Threadcommon *common=arg;
    if (!common || common->fd <0 )
    {
        syslog(LOG_ERR,"%s\n",strerror(errno));
        Packetstatistic.rError++;
        return;
    }
    signal(SIGINT,signalcatch);

    while(read(common->fd,&data,ONE)!=-1 && loop)
    {
        switch (State)
        {
        case EmptyState:
            if (data == 0x55)
            {
                State= moto55;
                i=0;
            }
            continue;
        case moto55:
            if (data == 0x55)
            {
                i++;
                if(i==5)
                {
                    syslog(LOG_ERR,"Too much 0x55 received.");
                    Packetstatistic.rError++;
                    break;
                }
                continue;
            }
            if (data== FF)
            {
                State=moto1;
                continue;
            }
            else
            {
                Packetstatistic.packetError++;
                syslog(LOG_ERR,"After 0x55 did not receive proper data(%d)",data);
                break;
            }
        case moto1:
            if(data==1)
            {
                calculateCrc=0;
                State= address;
                Packetstatistic.packet++;
                continue;
            }
            else
                break;
        case address:
            if (!receivingData)
            {
                calculateCrc = addCRC(calculateCrc, data);
                receivingData=reserve(data);
                if (!receivingData)
                {
                    syslog(LOG_ERR,"Cannot reserved memory to receivingData");
                    break;
                }
                State = command;
                continue;
            }
            else
                Packetstatistic.overrun++;         //Packets are incoming too fast, should take bigger hold time between readings
            break;
        case command :
            calculateCrc = addCRC(calculateCrc,data);
            receivingData->cmd = data;
            State = DLenLow;
            continue;
        case DLenLow :
            calculateCrc = addCRC(calculateCrc, data);
            receivingData->dlen = (data & 0xff);
            State = DLenHigh;
            continue;
        case DLenHigh :
            calculateCrc = addCRC(calculateCrc, data);
            receivingData->dlen |= (data& 0xff) << BYTE ;
            dataIndex=0;
            if (receivingData->dlen > 0)
            {
                if (receivingData->dlen <= LIMIT)
                {
                    receivingData->data =(char*)malloc((receivingData->dlen)*sizeof(char));
                    if(!receivingData->data)
                    {
                        syslog(LOG_ERR,"No enough memory");
                        Packetstatistic.packetError++;
                        break;
                    }
                    State = Data;
                    continue;
                }
                else
                {
                    syslog(LOG_ERR,"Too big the datalength");
                    break;
                }
            }
            else
            {
                State =  CrcLow;
                continue;
            }
        case Data :
            calculateCrc = addCRC(calculateCrc, data);
            *((receivingData->data)+dataIndex) = data;
            if(++dataIndex>=receivingData->dlen)
                State = CrcLow;
            else
                State = Data;
            continue;
        case CrcLow :
            packetCrc = (data & 0xff);
            State = CrcHigh;
            continue;
        case CrcHigh:
            packetCrc |= ( data & 0xff)<< BYTE;
            if (compareCRC(packetCrc, calculateCrc))
            {
                if(receivingData->cmd==1 && receivingData->data)           //cmdTerm =1, not polling
                {
                    toQueueuPacket=receivingData;
                    pthread_mutex_lock(&common->mutex);
                    TAILQ_INSERT_TAIL(&common->head,toQueueuPacket,entries);
                    pthread_mutex_unlock(&common->mutex);
                    receivingData=NULL;
                    State=EmptyState;
                    Packetstatistic.validPacket++;
                }
                else if (receivingData->cmd==PING)
                {
                    Packetstatistic.pollPacket++;
                    Packetstatistic.validPacket++;
                    syslog(LOG_NOTICE,"%s Keep Alive",common->sensors[(int)receivingData->address-1].names);
                }
                else
                    syslog(LOG_ERR,"ERROR Packet");
            }
            break;
        }
        State=EmptyState;
        if(receivingData)
        {
            if(receivingData->data)
                free(receivingData->data);
            free(receivingData);
            receivingData=NULL;
        }
        syslog(LOG_NOTICE,"Packetstatistic: packetError=%d"
               " packet=%d"
               " validPacket=%d"
               " overrun=%d"
               " emptyPacket=%d"
               " Error=%d"
               ,Packetstatistic.packetError
               ,Packetstatistic.packet
               ,Packetstatistic.validPacket
               ,Packetstatistic.overrun
               ,Packetstatistic.pollPacket
               ,Packetstatistic.rError);


    }
    if(receivingData)
    {
        if(receivingData->data)
            free(receivingData->data);
        free(receivingData);
        receivingData=NULL;
    }

    syslog(LOG_ERR,"Reading thread finished");
}

QueueData *reserve(char data)
{
    QueueData *temp;
    temp=(QueueData *)malloc(sizeof(QueueData));
    if (!temp)
        return NULL;
    temp->address=data;
    temp->cmd=0;
    temp->dlen = 0;
    temp->data = NULL;
    return temp;
}


void signalcatch(int sig)
{
    syslog(LOG_INFO,"signal received: %d",sig);
    loop=0;

}

