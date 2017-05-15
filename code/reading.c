#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header/crc.h"
#include <time.h>
#include "header/header.h"
#inlude <sys/queue.h>
#define LOGPATH "/herczig/Dokumentumok/log.txt"

/**
Reading from the serial port. To check the incoming packet, use the Motorola protocol
*/

 void /*incoming_data*/ *readingFromSerial(int fd)
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
                        //statistic.dataindex= 0;

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
                        State =  CrcLow;
                        continue;

                case Data :
                        calculateCrc = addCrcByte(calculateCrc, data);
                        *((receivingData->data)+statistic.dataindex) = data;
                 //   if (++dIndex >= receivingData->dlen)   // lesz még adat
                        State = CrcLow;  ???????????
                        continue;

                case CrcLow :
                        packetCrc = (c & FF);
                        State = CrcHigh;
                        continue;

                case CrcHigh;
                        packetCrc |=( c & FF) << BYTE;
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

void sendRequest() {
     int requestType=0;
	 int requestCounter=0;
	 requestType = (requestType + 1) % (cmGetPressure+1); // a következő parancs kiszámítása

     ++requestCounter;  // kérések számának növelése

     switch (requestType) {
         case 0: sendPacket(fileHandle, clientAddress, cmPing, NULL, 0);
             return;
         case 1: sendPacket(fileHandle, clientAddress, cmGetTerm, NULL, 0);
             return;
     }
}

void polling(char clientAddr, int fd) {
     char key;
     clientAddress = clientAddr;
     fileHandle = fd;
     while (1) {
         sendRequest();  // a következő kérés kiküldése


 //        refreshServerStatus();  // status sor frissítás

         if (isReceivePacket())  // új csomag ellenőrzés
             processReceived();  // van új csomag, fel kell dolgozni
     }
}

int sendPacket(int fd, unsigned char address, unsigned char cmd, unsigned char *data, int dLen) {
     int i;
     uint16_t crc;
     unsigned char preamble=PREAMBLE_CHAR,
          ff = 0xff,one = 1,
          sc;
     if (fd < 0 || (dLen > 0 && !data))
         return 0;

     for (i=0;i<5;i++)
         writePort(fd,0x55,1); // n*0x55 kiküldés

     writePort(fd,0xFF,1);         // 0xff kiküldés
     writePort(fd,0x01,1);        // 1 kiküldés

     crc = 0;                     // CRC kalkuláció a címtől indul
     crc = addCrcByte(crc, address);
     writePort (fd,&address,1);   // cim kiküldés
     crc = addCrcByte(crc, cmd);
     writePort(fd,&cmd,1);        // Prancs kiküldés
     sc = dLen & 0xff;            // adathossz alsó byte
     crc = addCrcByte(crc, sc);
     writePort(fd, &sc, 1);       // adathossz alsó byte kiküldés
     sc = (dLen >> 8) & 0xff;     // adathossz felső byte
     crc = addCrcByte(crc, sc);
     writePort(fd, &sc, 1);       // adathossz felső byte kiküldés
     if (dLen > 0)                // ha van adat
         for (i=0;i<dLen;i++,data++) {
             crc = addCrcByte(crc, *data);
             writePort (fd,data,1);  // *data kiküldés
         }

     return 1;
}
