#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/queue.h>
#include <time.h>
#include "../header/header.h"
#include "../header/crc.h"
#include "../header/reading.h"
#define ERRORPATH "/home/herczig/Dokumentumok/errorlog.txt"
#define LOGPATH "/home/herczig/Dokumentumok/Packet_log.txt"
#define MAXREQUEST 30
#define ONEBYTE 1
/**
Reading from the serial port. To check the incoming packet, use the Motorola protocol
*/

void  *readingFromSerial(threadArg *arg)
{
    queueData *receivingData=NULL;
    unsigned char data,i=0;
    Crc packetCrc,calculateCrc;
    calculateCrc=packetCrc=0;
    statistic Packetstatistic= {0};
    PacketState State=EmptyState;

    time_t now;
    time(&now);

    FILE * LogFile=fopen(LOGPATH,"w");
    FILE * errorfile=fopen(ERRORPATH,"w");

    if (*(arg->fd) <0 )
    {
        fprintf(errorfile,"%s \t \t Cannot open filedescription\n",ctime(&now));
        return;
    }

    while(read(fd,&data,1))
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
                calculateCrc = addCrcByte(calculateCrc, data);
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
            calculateCrc = addCrcByte(calculateCrc,data);
            receivingData->cmd = data;
            State = DLenLow;
            continue;

        case DLenLow :
            calculateCrc = addCrcByte(calculateCrc, data);
            receivingData->dlen = data & FF;
            State = DLenHigh;
            continue;

        case DLenHigh :
            calculateCrc = addCrcByte(calculateCrc, data);
            receivingData->dlen|= (data & FF) << BYTE;

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
                Packetstatistic.emptyPacket++;
                State =  CrcLow;
                continue;
            }
        case Data :
            calculateCrc = addCrcByte(calculateCrc, data);
            *(receivingData->data) = data;
            State = CrcLow;
            continue;

        case CrcLow :
            packetCrc = data & FF;
            State = CrcHigh;
            continue;

        case CrcHigh:
            packetCrc |=( data & FF) << BYTE;
            if (compareCRC(packetCrc, calculateCrc))
            {
                if(receivingData->cmd==1 && *(receivingData->data))           //cmdTerm not polling
                {
                    arg->Packet=receivingData;
                    free(receivingData->data);
                    free(receivingData);
                    Packetstatistic.validPacket++;
                    State = EmptyState;
                }
                else
                    fprintf(LogFile,"Slave Keep Alive:%c\t\t%s\n",receivingData->address,ctime(&now));
            }
            break;
        }
        if(arg->Packet)
        {
            pthread_mutex_lock(&(arg->Packet->mutex));
            TAILQ_INSERT_TAIL(&InHd,arg->Packet,entries);
            pthread_mutex_unlock(&(arg->Packet->&mutex));
        }
        State = EmptyState;
        if (receivingData)
        {
            if (receivingData->data)
                free(receivingData->data);
            free(receivingData);
            receivingData = NULL;
        }
        fprintf(LogFile,"\ŧ\t%s\n Packetstatistic\n packetError=%d\t",Packetstatistic.packetError);
        fprintf(LogFile,"packet=%d\t",Packetstatistic.packet);
        fprintf(LogFile,"validPacket=%d\t",Packetstatistic.validPacket);
        fprintf(LogFile,"overrun=%d\t",Packetstatistic.overrun;
                fprintf(LogFile,"emptyPacket=%d\t\t%s\n",Packetstatistic.emptyPacket,ctime(&now));


                sleep(arg->conf->samplingTime);    // alvoido
    }

    fclose(LogFile);
    fclose(errorfile);

}

queueData *reserve(char data)
{
    queueData *temp;
    temp=(queueData *)malloc(sizeof(queueData));
    if (!temp)
        return 0;
    temp->address=data;
    temp->dLen = 0;
    temp->data = NULL;
    return temp;
}

void sendRequest(threadArg *arg)
{

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
            while(addresses<=arg->conf->numbOfDev)
            {
                sendPacket(arg->fd,addresses, cmdTerm, NULL,0);
                addresses++;

            }

            requestCounter++;

        }
        else
        {
            while(addresses<=arg->conf->numbOfDev)
            {
                sendPacket(arg->fd,addresses, cmdPing, NULL,0);
                addresses++;

            }

            requestCounter++;
        }


        addresses=0;
        sleep(arg->conf->time);
    }

}


int sendPacket(int *fd, unsigned char address, unsigned char cmd, unsigned char *data, uint16_t dLen)
{
    if (fd < 0 || (!data && dlen))
        return 0;
    int fildesp=*fd;
    int i;
    uint16_t crc=0;
    uint16_t datalength;
    char temp=0;
    char motorola55=0x55,motorolaFF=0XFF,motorola1=0x01;

    for (i=0; i<5; i++)
        write(fildesp,&motorola55,ONEBYTE);

    write(fildesp,&motorolaFF,ONEBYTE);
    write(fildesp,&motorola1,ONEBYTE);

    crc = addCrcByte(crc, address);
    write (fildesp,&address,ONEBYTE);

    crc = addCrcByte(crc, cmd);
    write(fildesp,&cmd,ONEBYTE);

    datalength= dLen & FF;
    crc = addCrcByte(crc,datalength);
    write(fildesp, &datalength, ONEBYTE);
    datalength |= (dLen << BYTE) & FF;
    crc = addCrcByte(crc, datalength);
    write(fildesp, &datalength, ONEBYTE);


    switch (datalength):

        case datalength>0:
         for (i=0; i<datalength; i++,data++)
            {
            crc = addCrcByte(crc, *data);
            write (fildesp,data,ONEBYTE);
            }

        case !datalength:
        data=&temp;
        crc = addCrcByte(crc, *data);
        write (fildesp,data,ONEBYTE);

        case datalength<0:
        return 0;

    crc=crc & FF;
    crc=addCrcByte(crc,crc);
    write(fildesp,&crc,ONEBYTE);
    crc|=(crc <<BYTE) & FF;
    write(fildesp,&crc,ONEBYTE);

    return 1;
}
