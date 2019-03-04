#include <string>
#include <stdint.h>
#include "spi_init.hpp"

using namespace std;

spi_Module::spi_Module()
{
    int ret;
    ret=init_SPI(spi_1);
    if(ret<0)
    {
        ret=init_SPI(spi_2);
        if(ret<0)
            
    }
}

spi_Module::spi_Module(string &s)
{
    
}



int spi_Module::init_SPI(string &s)
{
    fd=open(s.c_str(),O_RDWR | O_NOCTTY | O_NDELAY );

    return fd;
}