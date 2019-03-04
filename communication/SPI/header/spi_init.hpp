#ifndef SPI_INIT_H_INCLUDED
#define SPI_INIT_H_INCLUDED

#ifndef CPP
#   define CPP 1
#endif
#include <string>
#include <stdint.h>
#include <syslog.h>
#define  SPI1   "/dev/spidev0.0"
#define  SPI2   "/dev/spidev0.1"
#define  BADSTRING  "NULL string"
#define  BAD_FD  0
#define  BAD_SPI "No SPI modul found"
using namespace std;

struct spi_Module
{
    int CS;
    uint8_t *TX_buffer;
    uint8_t *RX_buffer;


}SPI_Module;

class spi_Comm
{
    private:
        int fd;
        int init_SPI(string &s);
        string spi_1=SPI1;
        string spi_2=SPI2;
    public:
        spi_Comm();
        spi_Comm(string &s);
        ~spi_Comm();

};





#endif