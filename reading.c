#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include "header/crc.h"
#include <time.h>
#include "header/header.h"
#inlude <sys/queue.h>
#define LOGPATH "/herczig/Dokumentumok/errorlog.txt"

/**
Reading from the serial port. To check the incoming packet, use the Motorola protocol
*/

 void  *readingFromSerial(int fd)
{

    incoming_data *receivingData=NULL;
    unsigned char data,i=0;
    uint16_t packetCrc,calculateCrc;
    calculateCrc=packetCrc=0;

    PacketState State=EmptyState;

    FILE * errorfile=fopen(LOGPATH,"w");

     if (fd <=0 )
         fprintf(errorfile,"cannot open filedescription\n");


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
                            fprintf(errorfile,"Too much 0x55 has arrived\n Reading Restart!\n");
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
                        statistic.packetError++;
                        fprintf(errorfile,"After 0x55 did not receive proper data(0xFF)\n");
                        break;
                    }

                case motoFF:
                    if(data==1)
                        State=moto1;
                    continue;
                    else
                    {
                        statistic.packetError++;
                        fprintf(errorfile,"After 0xFF did not receive proper data(0x01)\n");
                        break;
                    }


                case moto1:
                    if(data)
                        {
                            State= address;
                            statistic.packet++;
                        }
                    else
                        break;

                case address:
                    if (!receivingData)
                    {
                        calculateCrc = addCrcByte(calculateCrc, data);
                        receivingData=reserve(data);
                        if (!receivingData)
                            fprintf(errorfile,"cannot reserved memory to receivingData\n");
                        State = command;
                        continue;
                    }
                    else
                       statistic.overrun++;         //Packets are incoming too fast, should take bigger hold time between readings
                        break;

                case command :
                        calculateCrc = addCrcByte(calculateCrc,data);
                        receivingData->cmd = data;
                        State = DLenLow;
                        continue;

                case DLenLow :
                        calculateCrc = addCrcByte(calculateCrc, data);
                        receivingData->dlen = (data & FF);
                        State = stDLenHi;
                        continue;

                case DLenHigh :
                        calculateCrc = addCrcByte(calculateCrc, data);
                        receivingData->dlen|= (data & FF) << BYTE;

                    if (receivingData->dlen > 0)
                        {
                        if (receivingData->dlen <= LIMIT)
                            {
                            receivingData->data =malloc(sizeof (receivingData->dlen));
                                if(!receivingData->data)
                                    {
                                    fprintf(errorfile,"No enough memory");
                                    statistic.packetError++;
                                    break;
                                    }
                            State = Data;
                            }
                        else
                            {
                                fprintf("Too big the datalength \n");
                                break;
                            }
                        }
                    else
                        statistic.emptyPacket++;
                        fprintf(logfile,"Slave Keep Alive:%c\n",receivingData->addresss);
                        State =  CrcLow;
                        continue;

                case Data :
                        calculateCrc = addCrcByte(calculateCrc, data);
                        *((receivingData->data)+statistic.dataindex) = data;
                        State = CrcLow;  ???????????
                        continue;

                case CrcLow :
                        packetCrc = (c & FF);
                        State = CrcHigh;
                        continue;

                case CrcHigh;
                        packetCrc |=( c & FF) << BYTE;              //?????????
                    if (compareCRC(packetCrc, calculateCrc))
                    {
                        incomingPacket= receivingData;
                        receivingData = NULL;
                        statistic.validPacket++;
                        State = EmptyState;
                    }
                    break;
         }
			if(incomingPacket)
			{
				pthread_mutex_lock(incomingPacket->mutex);
				TAILQ_INSERT_TAIL(&head,incomingPacket,entries)
				pthread_mutex_unlock(incoming_data->mutex);
			}
            State = EmptyState;
            if (receivingData)
            {
                if (receivingData->data)
                    free(receivingData->data);
                    free(receivingData);
                    receivingData = NULL;
            }
            statistic.packetError++;
            sleep(1000);    // alvoido
        }

        free(receivingData);
        fclose(errorfile);

 }

 incoming_data *reserve(char data)
 {
     incoming_data *temp;
        temp=malloc(sizeof(incoming_data));
        if (!temp)
            return 0;
     temp->address=data;
     temp->dLen = 0;
     temp->data = NULL;
           return temp;
 }

void sendRequest( struct config conffile)
 {

     int addresses=0;
     int requestType=1;
	 int requestCounter=0;
     char cmdPing=0;
     char cmdTerm=1;


     while(1)
     {

        if(requestCounter==30)
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
        sleep(conffile.samplingTime)           //lekerdezesiido
     }

}


int sendPacket(int fd, unsigned char address, unsigned char cmd, unsigned char *data, int dLen)
{
     int i;
     uint16_t crc=0;
     unsigned char      moto55=0x55,
                        motoff = FF,
                        moto1 = 1,
                        datalength;

     if (fd < 0 || (dLen > 0 && !data))
         return 0;

     for (i=0;i<5;i++)
         write(fd,0x55,1);

     write(fd,FF,1);
     write(fd,0x01,1);


        crc = addCrcByte(crc, address);
     write (fd,&address,1);
        crc = addCrcByte(crc, cmd);
     write(fd,&cmd,1);
     datalength= dLen & FF;
        crc = addCrcByte(crc,datalength);
     write(fd, &datalength, 1);
     datalength |= (dLen << BYTE) & FF;
        crc = addCrcByte(crc, datalength);
     write(fd, &datalength, 1);
     if (dLen > 0)
         for (i=0;i<dLen;i++,data++) {
             crc = addCrcByte(crc, *data);
             write (fd,data,1);
         }
        crc=crc & FF;
        crc=addCrcByte(crc,crc);
     write(fd,&crc,1);
        crc|=(crc <<BYTE) & FF;
     write(fd,&crc,1);

     return 1;
}
