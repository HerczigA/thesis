#ifndef READING_H_INCLUDED
#define READING_H_INCLUDED
#define LIMIT 1024
#define FF 0xFF
#include "header.h"


typedef enum PacketState
{
    /*! Default condition*/
    EmptyState,

    /*! min. one 0x55 received*/
    moto55,

    /*!after n*0x55, 0xFF received */
    motoFF,

    /*! after 0xFF, 0x01 received*/
    moto1,

    /*! Address */
    address,

    /*! Command*/
    command,

    /*! Low byte of datalength*/
    DLenLow,

    /*! High byte of datalength*/
    DLenHigh,

    /*! Databyte */
    Data,

    /*! low byte of crc  Packet*/
    CrcLow,

    /*! high byte of crc  Packet*/
    CrcHigh

} PacketState;

typedef struct statistic
{
    int packetError;
    int packet;
    int validPacket;
    int overrun;
    int emptyPacket;

} Statistic;


void  readingFromSerial(Config *fileConfig);
queueData *reserve(char data);
void sendRequest(Config *fileConfig);
int sendPacket(int *fd, unsigned char address, unsigned char cmd, unsigned char *data, uint16_t dLen);
#endif // READING_H_INCLUDED
