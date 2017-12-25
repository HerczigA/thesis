#include <stdio.h>
#include <stdlib.h>
#include "../header/Init.h"
#include "../header/crc.h"
#include "../header/reading.h"
#include "../header/closing.h"
/**
Reading from the serial port. To check the incoming packet, use the Motorola protocol
*/

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
    if (!common || common->fd <0)
        {
            syslog(LOG_ERR,"%s\n",strerror(errno));
            pthread_exit(NULL);
        }
    while(read(common->fd,&data,ONE)!=-1 && common->loop)
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
                                    Packetstatistic.packetError++;
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
                                    pthread_mutex_lock(&common->temperature_mutex);
                                    TAILQ_INSERT_TAIL(&common->head,toQueueuPacket,entries);
                                    pthread_mutex_unlock(&common->temperature_mutex);
                                    receivingData=NULL;
                                    State=EmptyState;
                                    Packetstatistic.received_TermPacket++;
                                    Packetstatistic.validPacket++;
                                }
                            else if (receivingData->cmd==PING)
                                {
                                    Packetstatistic.received_PollPacket++;
                                    Packetstatistic.validPacket++;
                                    pthread_mutex_lock(&common->watchdog_mutex);
                                    common->sensors[(int)receivingData->address-1].watchdog--;
                                    if(common->sensors[(int)receivingData->address-1].watchdog>=WATCHDOGMAX)
                                        {
                                            common->sensors[(int)receivingData->address-1].watchdog=0;
                                            common->sensors[(int)receivingData->address-1].state=1;
                                        }
                                    pthread_mutex_unlock(&common->watchdog_mutex);
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
                   " emptyPacket=%d\n"
                   ,Packetstatistic.packetError
                   ,Packetstatistic.packet
                   ,Packetstatistic.validPacket
                   ,Packetstatistic.overrun
                   ,Packetstatistic.received_PollPacket);


        }
    if(receivingData)
        {
            if(receivingData->data)
                free(receivingData->data);
            free(receivingData);
            receivingData=NULL;
        }
    printf("Reading thread finished\n");
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


