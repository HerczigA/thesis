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
    unsigned char data,i=0;
    Crc packetCrc,calculateCrc;
    calculateCrc=packetCrc=0;
    Statistic Packetstatistic= {0};
    PacketState State=EmptyState;
    Threadcommon *common=arg;

    time_t now;
    time(&now);

    FILE * LogFile=fopen(LOGPATH,"w");
    FILE * errorfile=fopen(ERRORPATH,"w");

    if (common->fd <0 )
    {
        fprintf(errorfile,"%s \t \t Cannot open filedescriptor\n",ctime(&now));
        return;
    }

    while(read(common->fd,&data,ONEBYTE))
    {
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
                if(i==6)
                {
                    fprintf(errorfile,"%s\t\t Too much 0x55 has arrived\n Reading Restart!\n",ctime(&now));
                    break;
                }
                continue;
            }
            if (data == FF)
            {
                State=motoFF;
                continue;
            }
            else
            {
                Packetstatistic.packetError++;
                fprintf(errorfile,"%s\t\tAfter 0x55 did not receive proper data(0xFF)\n",ctime(&now));
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
                fprintf(errorfile,"%s\t\tAfter 0xFF did not receive proper data(0x01)\n",ctime(&now));
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
            if (!receivingData)
            {
                calculateCrc = addCRC(calculateCrc, data);
                receivingData=reserve(data);
                if (!receivingData)
                {
                    fprintf(errorfile,"%s\t\t Cannot reserved memory to receivingData\n",ctime(&now));
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
                    receivingData->data =(float *)malloc(sizeof (receivingData->dlen));
                    if(!receivingData->data)
                    {
                        fprintf(errorfile,"No enough memory\t\t%s\n",ctime(&now));
                        Packetstatistic.packetError++;
                        break;
                    }
                    State = Data;
                }
                else
                {
                    fprintf(errorfile,"Too big the datalength\t\t%s\n",ctime(&now));
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
                if(receivingData->cmd==1 && *(receivingData->data))           //cmdTerm =1, not polling
                {
                    pthread_mutex_lock(&common->mutex);
                    TAILQ_INSERT_TAIL(&common->head,receivingData,entries);
                    pthread_mutex_unlock(&common->mutex);
                    Packetstatistic.validPacket++;
                }
                else
                {
                    Packetstatistic.emptyPacket++;
                    fprintf(LogFile,"Slave Keep Alive:%c\t\t%s\n",receivingData->address,ctime(&now));
                }

            }
            break;
        }

        receivingData=NULL;
        State = EmptyState;
        fprintf(LogFile,"\n Packetstatistic\n packetError=%d\t\t%s\n",Packetstatistic.packetError,ctime(&now));
        fprintf(LogFile,"packet=%d\t\t%s\n",Packetstatistic.packet,ctime(&now));
        fprintf(LogFile,"validPacket=%d\t\t%s\n",Packetstatistic.validPacket,ctime(&now));
        fprintf(LogFile,"overrun=%d\t\t%s\n",Packetstatistic.overrun,ctime(&now));
        fprintf(LogFile,"emptyPacket=%d\t\t%s\n",Packetstatistic.emptyPacket,ctime(&now));


        sleep(SAMPTIME);    // alvoido
    }

    fclose(LogFile);
    fclose(errorfile);

}

QueueData *reserve(char data)
{
    QueueData *temp;
    temp=(QueueData *)malloc(sizeof(QueueData));
    if (!temp)
        return 0;
    temp->address=data;
    temp->dlen = 0;
    temp->data = NULL;
    return temp;
}



int sendPacket(int *fd, unsigned char address, unsigned char cmd, unsigned char *data, uint16_t dLen)
{
    if (fd < 0 || (!data && dLen))
        return 0;
    int i;
    uint16_t crc=0;
    uint16_t datalength;

    char motorola55=0x55,motorolaFF=0XFF,motorola1=0x01;

    for (i=0; i<5; i++)
        write(*fd,&motorola55,ONEBYTE);

    write(*fd,&motorolaFF,ONEBYTE);
    write(*fd,&motorola1,ONEBYTE);

    crc = addCRC(crc, address);
    write (*fd,&address,ONEBYTE);

    crc = addCRC(crc, cmd);
    write(*fd,&cmd,ONEBYTE);

    datalength= dLen;
    crc = addCRC(crc,datalength);
    write(*fd, &datalength, ONEBYTE);
    datalength |= (dLen << BYTE);
    crc = addCRC(crc, datalength);
    write(*fd, &datalength, ONEBYTE);

    if(datalength>0)
    {
        for (i=0; i<datalength; i++,data++)
        {
            crc = addCRC(crc, *data);
            write(*fd,data,ONEBYTE);
        }
    }
    else if(!datalength)
    {
        *data=ZERO;
        crc = addCRC(crc, *data);
        write(*fd,data,ONEBYTE);

    }
    else
        return 0;

    crc=addCRC(crc,crc);
    write(*fd,&crc,ONEBYTE);
    crc|=(crc <<BYTE);
    write(*fd,&crc,ONEBYTE);

    return 1;
}

void sendRequest(void *arg)
{

    Threadcommon *common=arg;
    int addresses=0;
    unsigned char requestType;
    unsigned char requestCounter=0;
    const char cmdPing=0;
    const char cmdTerm=1;

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
                sendPacket(&common->fd,addresses, cmdTerm, NULL,0);
                addresses++;

            }

            requestCounter++;

        }
        else
        {
            while(addresses<=common->numbOfDev)
            {
                sendPacket(&common->fd,addresses, cmdPing, NULL,0);
                addresses++;

            }

            requestCounter++;
        }


        addresses=0;
        sleep(common->time);
    }

}

