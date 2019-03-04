#ifndef SPI_INIT_H_INCLUDED
#define SPI_INIT_H_INCLUDED

#ifndef CPP
#   define CPP 1
#endif
#include <string>

using namespace std;

class spi_Module
{
    private:
        int fd;
        int init_SPI(string &s);
        int CS;
        string spi_1="/dev/spidev0.0";
        string spi_2="/dev/spidev0.1";
    public:
        spi_Module();
        spi_Module(string &s);

};





#endif