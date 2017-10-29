#include <stdio.h>
#include <stdlib.h>
#include "../header/header.h"
#include "../header/crc.h"
#include "../header/reading.h"
#include "../header/reading.h"
/**
Reading from the serial port. To check the incoming packet, use the Motorola protocol
*/

void  readingFromSerial(void *arg)
{
    QueueData *receivingData=NULL;
    unsigned char data,j=0;
    int i=0;
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

        while(1)
        {
            read(common->fd,&data,ONE);
            printf("%d\n",data);
            switch (State)
                {

                case EmptyState:
                    if (data == 0x55)
                        State= moto55;
                    continue;

                case moto55:
                    if (data == 0x55)
                        {
                            i++;
                            if(i==5)
                                {
                                    syslog(LOG_ERR,"Too much 0x55 received.Received numbers of data[%d]:%d\n",i,i);
                                    Packetstatistic.rError++;
                                    break;
                                }
                            continue;
                        }
                    if (data== FF)
                        {
                            State=motoFF;
                            continue;
                        }
                    else
                        {
                            Packetstatistic.packetError++;
                            syslog(LOG_ERR,"After 0x55 did not receive proper data(0xFF)");
                            break;
                        }

                case motoFF:
                    if(data==1)
                        {
                            State=moto1;
                            continue;
                        }
                    else
                        {
                            Packetstatistic.packetError++;
                            syslog(LOG_ERR,"After 0xFF did not receive proper data(0x01)");
                            break;
                        }


                case moto1:
                    if(data)
                        {
                            State= address;
                            Packetstatistic.packet++;
                        }
                    else
                        break;

                case address:
                    calculateCrc=0;
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
                    receivingData->dlen = data;
                    State = DLenHigh;
                    continue;

                case DLenHigh :
                    calculateCrc = addCRC(calculateCrc, data);
                    receivingData->dlen|= (data << BYTE) ;

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
                    *(receivingData->data) = data;
                    State = CrcLow;
                    continue;

                case CrcLow :
                    packetCrc = data;
                    State = CrcHigh;
                    continue;

                case CrcHigh:
                    packetCrc |=( data<< BYTE);
                    if (compareCRC(packetCrc, calculateCrc))
                        {
                            if(receivingData->cmd==1)           //cmdTerm =1, not polling
                                {
                                    pthread_mutex_lock(&common->mutex);
                                    TAILQ_INSERT_TAIL(&common->head,receivingData,entries);
                                    pthread_mutex_unlock(&common->mutex);
                                    Packetstatistic.validPacket++;
                                }
                            else
                                {
                                    Packetstatistic.pollPacket++;
                                    syslog(LOG_NOTICE,"Slave Keep Alive:%c",receivingData->address);
                                }

                        }
                    break;
                }


            free(receivingData);
            State = EmptyState;
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



int sendPacket(int fd, unsigned char address, unsigned char cmd,unsigned char *data, char dLen)
{
    Statistic packet;
    packet.wError=0;

    if ( !data || fd<0 || dLen<0  )
        {
            syslog(LOG_ERR,"%s fd=%d,data=%p  Writing Error:%d",strerror(errno), fd,data,++packet.wError);
            return -1;
        }

    int i;
    uint16_t crc=0;
    uint16_t temp;

    char motorola55=0x55,motorolaFF=0XFF,motorola1=0x01;

    for (i=0; i<MAXU; i++)
        write(fd,&motorola55,ONE);

    write(fd,&motorolaFF,ONE);
    write(fd,&motorola1,ONE);
    crc = addCRC(crc, address);
    write (fd,&address,ONE);
    crc = addCRC(crc, cmd);
    write(fd,&cmd,ONE);
    temp= dLen;
    crc = addCRC(crc,temp);
    write(fd,&temp,ONE);
    temp = (dLen >> BYTE);
    crc = addCRC(crc, temp);
    write(fd, &temp, ONE);

    if(dLen>0)
        {
            for (i=0; i<dLen; i++,data++)
                {
                    crc = addCRC(crc, *data);
                    write(fd,data,ONE);
                }
        }


    write(fd,&crc,ONE);
    crc=(crc >>BYTE);
    write(fd,&crc,ONE);


    return 1;
}

void sendRequest(void *arg)
{

    Threadcommon *common=arg;
    int addresses=1;
    unsigned char requestType;
    unsigned char requestCounter=0;
    const char cmdPing=0;
    const char cmdTerm=1;
    Statistic packet;
    packet.TermPacket=0;
    packet.pollPacket=0;
    unsigned char data=0;
    while(1)
        {

            if(requestCounter==MAXREQUEST)
                requestCounter=0;


            requestType = requestCounter % 3;
            ++requestCounter;

            if(!requestType)
                {
                    while(addresses<=common->numbOfDev)
                        {
                            if(common->sensors[addresses].state)
                                {
                                    if(sendPacket(common->fd,addresses, cmdTerm, &data,0)>0)
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

                    requestCounter++;
                    sleep(common->time);

                }
            else
                {
                    while(addresses<=common->numbOfDev)
                        {
                            if(common->sensors[addresses].state)
                                {
                                    if(sendPacket(common->fd,addresses, cmdPing, &data,0)>0)
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


            addresses=0;
        }
}

