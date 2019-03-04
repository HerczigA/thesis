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
    public:
        spi_Module();
        spi_Module(string &s);

};





#endif