#ifndef READING_H_INCLUDED
#define READING_H_INCLUDED
#define LIMIT 1024
#define FF 0xFF


typedef enum
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
		int packetError=0;
		int packet=0;
		int validPacket=0;
		int overrun=0;
        int emptyPacket=0;

	};


int readingFromSerial(int fd,char *logfile);
void sendRequest( struct config conffile);
int sendPacket(int fd, unsigned char address, unsigned char cmd, unsigned char *data, int dLen);
#endif // READING_H_INCLUDED
