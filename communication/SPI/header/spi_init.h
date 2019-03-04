#ifndef SPI_INIT_H_INCLUDED
#define SPI_INIT_H_INCLUDED

#include <stdint.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


typedef struct packet
{
    uint8_t address;
    uint8_t cmd;
    uint8_t length;
    char *data;

}Packet;

typedef struct spiPackage
{
    int CS;
    Packet frame;


}SPI_Package;


#endif