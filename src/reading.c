#include <stdio.h>
#include <stdlib.h>
#include "../header/header.h"
#include "../header/crc.h"
#include "../header/reading.h"
#include "../header/reading.h"
#include <signal.h>
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

    if (!common || common->fd <0 )
        {
            syslog(LOG_ERR,"%s\n",strerror(errno));
            Packetstatistic.rError++;
            return;
        }
    while(read(common->fd,&data,ONE)!=-1)
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
                            printf("ADDRESS CRC:%d\n",calculateCrc);
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
                                    receivingData->data =(float *)malloc(receivingData->dlen*sizeof(float));
                                    if(!receivingData->data)
                                        {
                                            syslog(LOG_ERR,"No enough memory");
                                            Packetstatistic.packetError++;
                                            break;
                                        }
                                    State = Data;
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
                            else if (receivingData->cmd==0x69)
                                {
                                    Packetstatistic.pollPacket++;
                                    syslog(LOG_NOTICE,"Slave Keep Alive: %d",(int)receivingData->address);
                                    Packetstatistic.validPacket++;
                                    free(receivingData);
                                    receivingData=NULL;
                                    State=EmptyState;
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

            syslog(LOG_NOTICE,"Packetstatistic packetError=%d",Packetstatistic.packetError);
            syslog(LOG_NOTICE,"packet=%d",Packetstatistic.packet);
            syslog(LOG_NOTICE,"validPacket=%d",Packetstatistic.validPacket);
            syslog(LOG_NOTICE,"overrun=%d",Packetstatistic.overrun);
            syslog(LOG_NOTICE,"emptyPacket=%d",Packetstatistic.pollPacket);
            syslog(LOG_NOTICE,"Error=%d",Packetstatistic.rError);

            sleep(common->samplingTime);    // alvoido
        }


}

QueueData *reserve(char data)
{
    QueueData *temp;
    temp=(QueueData *)malloc(sizeof(QueueData));
    if (!temp)
        return NULL;
    temp->address=data;
    temp->dlen = 0;
    temp->data = NULL;
    return temp;
}



int sendPacket(int fd, unsigned char address, unsigned char cmd,unsigned char *data, uint16_t dLen)
{
    Statistic packet;
    packet.wError=0;

    if ( !data || fd <0 || dLen <0 )
        {
            syslog(LOG_ERR,"%s fd=%d,data=%p  Writing Error:%d",strerror(errno), fd,data,++packet.wError);
            return -1;
        }
    char *buff=(char*)malloc((dLen+13)*sizeof(char));
    if(!buff)
        {
            syslog(LOG_ERR,"No enough memory");
            return -1;
        }
    int i=0;
    int dataElement=11;
    uint16_t crc=0;
    unsigned char len1,len2,crc1,crc2;

    crc = addCRC(crc, address);
    crc = addCRC(crc, cmd);
    len1= dLen & 0xff;
    crc = addCRC(crc,len1);
    len2 = (dLen >> BYTE) & 0xff;
    crc = addCRC(crc, len2);
    if(dLen>0)
        {
            int j;
            for (j=0; j<dLen; j++,data++)
                {
                    buff[dataElement]=*data;
                    crc = addCRC(crc, *data);
                    dataElement++;
                }
        }
    crc1=crc & 0xff;
    crc2=(crc>>BYTE) & 0xff;
    while(i!=5)
        {
            buff[i]=0x55;
            i++;
        }
    buff[5]=0xFF;
    buff[6]=0x01;
    buff[7]=address;
    buff[8]=cmd;
    buff[9]=len1;
    buff[10]=len2;
    buff[dataElement]=crc1;
    dataElement++;
    buff[dataElement]=crc2;
    dataElement++;
    i=write(fd,buff,dataElement);
    if(i!=dataElement)
        {
            syslog(LOG_ERR,"%s:%d",strerror(errno),i);
            free(buff);
            return -1;
        }
    else
        {
            syslog(LOG_INFO,"%d",i);
            free(buff);
            return 1;
        }
}

void sendRequest(void *arg)
{

    Threadcommon *common=arg;
    char addresses=1;
    int requestType;
    int requestCounter=0;
    unsigned const char heartBit=0x69;
    unsigned const char cmdTerm=1;
    uint16_t DLEN=0;
    Statistic packet;
    packet.TermPacket=0;
    packet.pollPacket=0;
    unsigned char data=0;
    while(1)
        {

            if(requestCounter==MAXREQUEST)
                requestCounter=0;


            requestType = requestCounter % 3;

            if(!requestType)
                {
                    while((int)addresses<=common->numbOfDev)
                        {
                            if(common->sensors[(int)addresses].state)
                                {
                                    if(sendPacket(common->fd,addresses, cmdTerm, &data,DLEN)>0)
                                        {
                                            packet.TermPacket++;
                                            syslog(LOG_NOTICE,"Asking Term packet transmitted :%d",packet.TermPacket);

                                        }
                                    else
                                        {

                                            syslog(LOG_ERR,"Shit happened:%s",strerror(errno));
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
                            if(common->sensors[(int)addresses].state)
                                {
                                    if(sendPacket(common->fd,addresses, heartBit, &data,DLEN)>0)
                                        {
                                            packet.pollPacket++;
                                            syslog(LOG_NOTICE,"Asking Polling packet transmitted :%d",packet.pollPacket);

                                        }
                                    else
                                        {
                                            syslog(LOG_ERR,"Shit happened:%s",strerror(errno));
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

