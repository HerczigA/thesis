
#include "spi_init.hpp"

using namespace std;

spi_Comm::spi_Comm()
{
    int ret;
    ret=init_SPI(spi_1);
    if(ret<BAD_FD)
    {
        ret=init_SPI(spi_2);
        if(ret<BAD_FD)
            {
                throw BAD_SPI;
            }
    }


}

spi_Comm::spi_Comm(string &s)
{
    int ret;
    if(!s)
    {
        throw BADSTRING;
    }
    else
    {
        ret=init_SPI(s);
        if(ret<BAD_FD)
        {
            ret=init_SPI(spi_1);
            if(ret<BAD_FD)
            {
                ret=init_SPI(spi_2);
                if(ret<BAD_FD)
                    throw BAD_SPI;
            }
        }   
    }

}



int spi_Comm::init_SPI(string &s)
{
    fd=open(s.c_str(),O_RDWR | O_NOCTTY | O_NDELAY );

    return fd;
}



spi_Comm::~spi_Comm()
{
    close(fd);
}