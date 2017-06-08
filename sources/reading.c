#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include "header/crc.h"
#include <time.h>
#include "header/header.h"
#inlude <sys/queue.h>
#include <time.h>
#define ERRORPATH "/home/herczig/Dokumentumok/errorlog.txt"
#define LOGPATH "/home/herczig/Dokumentumok/Packet_log.txt"
#define MAXREQUEST 30
/**
Reading from the serial port. To check the incoming packet, use the Motorola protocol
*/

 void  *readingFromSerial(int *filedescripton)
{
    int fd=*filedescripton;
    queueData *receivingData=NULL;
    queueData *UsefulPacket;
    unsigned char data,i=0;
    uint16_t packetCrc,calculateCrc;
    calculateCrc=packetCrc=0;
    statistic Packetstatistic;
    PacketState State=EmptyState;
    time_t now;

    time(&now);

    FILE * LogFile=fopen(LOGPATH,"w");
    FILE * errorfile=fopen(ERRORPATH,"w");        //?ERROR and log sould be separately

     if (fd <=0 )
         {
            fprintf(errorfile,"cannot open filedescription\n\t\t %s\n",ctime(&now));
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
                            fprintf(errorfile,"Too much 0x55 has arrived\n Reading Restart!\t\t%s\n",ctime(&now));
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
                        fprintf(errorfile,"After 0x55 did not receive proper data(0xFF)\t\t%s\n",ctime(&now));
                        break;
                    }

                case motoFF:
                    if(data==1)
                        State=moto1;
                    continue;
                    else
                    {
                        Packetstatistic.packetError++;
                        fprintf(errorfile,"After 0xFF did not receive proper data(0x01)\t\t%s\n",ctime(&now));
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
                            fprintf(errorfile,"cannot reserved memory to receivingData\t\t%s\n",ctime(&now));
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
                        Packetstatistic.emptyPacket++;
                        fprintf(LogFile,"Slave Keep Alive:%c\t\t%s\n",receivingData->addresss,ctime(&now));
                        State =  CrcLow;
                        continue;

                case Data :
                        calculateCrc = addCrcByte(calculateCrc, data);
                        receivingData->(*data) = data;
                        State = CrcLow;
                        continue;

                case CrcLow :
                        packetCrc = data & FF;
                        State = CrcHigh;
                        continue;

                case CrcHigh;
                        packetCrc |=( data & FF) << BYTE;
                    if (compareCRC(packetCrc, calculateCrc))
                    {
                        UsefulPacket= receivingData;
                        receivingData = NULL;
                        Packetstatistic.validPacket++;
                        State = EmptyState;
                    }
                    break;
            }
			if(UsefulPacket)
			{
				pthread_mutex_lock(UsefulPacket.mutex);
				TAILQ_INSERT_TAIL(&head,UsefulPacket,entries);
				pthread_mutex_unlock(UsefulPacket.mutex);
			}
            State = EmptyState;
            if (receivingData)
            {
                if (receivingData->data)
                    free(receivingData->data);
                    free(receivingData);
                    receivingData = NULL;
            }
            fprintf(LogFile,"Packetstatistic\n packetError=%d\t",Packetstatistic.packetError);
            fprintf(LogFile,"packet=%d\t",Packetstatistic.packet);
            fprintf(LogFile,"validPacket=%d\t",Packetstatistic.validPacket);
            fprintf(LogFile,"overrun=%d\t",Packetstatistic.overrun;
            fprintf(LogFile,"emptyPacket=%d\t\t%s\n",Packetstatistic.emptyPacket,ctime(&now));


            sleep(/*fileconfig.time*/);    // alvoido
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

void sendRequest(config conffile,int *fileHandle)
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
             while(addresses<=conffile.numbOfDev)
             {
                sendPacket(fileHandle,addresses, cmdTerm, NULL,0);
                addresses++;

             }

             requestCounter++;

        }
        else
        {
            while(addresses<=conffile.numbOfDev)
            {
                sendPacket(fileHandle,addresses, cmdPing, NULL,0);
                addresses++;

            }

            requestCounter++;
        }


        addresses=0;
        sleep(conffile.samplingTime)
     }

}


int sendPacket(int *fd, unsigned char address, unsigned char cmd, unsigned char *data, uint16_t dLen)
{
     if (!(fd < 0 ))
         return 0;
     int fildesp=*fd;
     int i;
     uint16_t crc=0;
     uint16_t datalength;
     char motorola55=0x55,motorolaFF=0XFF,motorola1=0x01;

     queueData *Packet;

     for (i=0;i<5;i++)
         write(fd,&motorola55,1);

     write(fildesp,&motorolaFF,1);
     write(fildesp,&motorola1,1);

        crc = addCrcByte(crc, address);
     write (fildesp,&address,1);

        crc = addCrcByte(crc, cmd);
     write(fildesp,&cmd,1);

     datalength= dLen & FF;
        crc = addCrcByte(crc,datalength);
     write(fildesp, &datalength, 1);
     datalength |= (dLen << BYTE) & FF;
        crc = addCrcByte(crc, datalength);
     write(fildesp, &datalength, 1);


     switch (datalength):

     case datalength>0:
         for (i=0;i<datalength;i++,data++)
            {
             crc = addCrcByte(crc, *data);
             write (fildesp,data,1);
            }

     case !datalength:
        *data=0;
        crc = addCrcByte(crc, *data);
        write (fildesp,data,1);

     case datalength<0:
         return 0;

        crc=crc & FF;
        crc=addCrcByte(crc,crc);
     write(fildesp,&crc,1);
        crc|=(crc <<BYTE) & FF;
     write(fildesp,&crc,1);

     return 1;
}
