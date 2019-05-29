#include "../header/spi_init.hpp"

using namespace std;

spi_Comm::spi_Comm()
{
    int ret;
    try{
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
    catch(string msg)
    {
        cout<<msg<<endl;
    }

    this->SPI_unit.CLK=MAXCLK;
    this->SPI_unit.mode=0;
    this->SPI_unit.CS=1;
    this->SPI_unit.endianess=MSB;
    
    try{
        if(ioctl(spi_fd,SPI_IOC_WR_MODE,&SPI_unit.mode)<0)
            throw SPI_MODE_ERR;
        }

    catch(string msg)
    {
        cout<<msg<<endl;
    }
    try{
        if(ioctl(spi_fd,SPI_IOC_WR_MAX_SPEED_HZ,&SPI_unit.CLK)<0)        
            throw SPI_CLK_ERR;
            //SPI_IOC_WR_LSB_FIRST  SPI_IOC_RD_LSB_FIRST
        }
    
    catch(string msg)
    {
        cout<<msg<<endl;
    }

    try{
        if (ioctl (spi_fd, SPI_IOC_WR_LSB_FIRST, &SPI_unit.endianess) < 0)
            throw SPI_ENDIAN_WR_ERR;
        }

    catch(string msg)
    {
        cout<<msg<<endl;
    }
    
    try{
        if (ioctl (spi_fd, SPI_IOC_RD_LSB_FIRST, &&SPI_unit.endianess) < 0)
            throw SPI_ENDIAN_RD_ERR;   
        }
    
    catch(string msg)
    {
        cout<<msg<<endl;
    }
}


spi_Comm::spi_Comm(string &s, uint32_t CLK, uint8_t mode, uint8_t endian)
{
    int ret;
    try
    {
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
    catch(string msg)
    {
        cout<<msg<<endl;
    }

    this->SPI_unit.CLK=CLK;
    this->SPI_unit.mode =mode & 3;
    this->SPI_unit.CS=1;
    this->SPI_unit.endianess=endian;
    
    try{
        if(ioctl(spi_fd,SPI_IOC_WR_MODE,&SPI_unit.mode)<0)
            throw SPI_MODE_ERR;
        }
    
    catch(string msg)
    {
        cout<<msg<<endl;
    }
    
    try{
        if(ioctl(spi_fd,SPI_IOC_WR_MAX_SPEED_HZ,&SPI_unit.CLK)<0)        
            throw SPI_CLK_ERR;
        }
    
    catch(string msg)
    {
        cout<<msg<<endl;
    }
    //SPI_IOC_WR_LSB_FIRST  SPI_IOC_RD_LSB_FIRST
    try{
        if (ioctl (spi_fd, SPI_IOC_WR_LSB_FIRST, &SPI_unit.endianess) < 0)
            throw SPI_ENDIAN_WR_ERR;
        }
    
    catch(string msg)
    {
        cout<<msg<<endl;
    }
    
    try{
        if (ioctl (spi_fd, SPI_IOC_RD_LSB_FIRST, &SPI_unit.endianess) < 0)
            throw SPI_ENDIAN_RD_ERR;        
        }
   
   catch(string msg)
    {
        cout<<msg<<endl;
    }
}



int spi_Comm::init_SPI(string &s)
{
    spi_fd=open(s.c_str(),O_RDWR | O_NOCTTY | O_NDELAY );

    return fd;
}



spi_Comm::~spi_Comm()
{
    close(fd);
}

int spi_Comm::PackageAssembly(uint8_t address, uint8_t cmd, uint8_t len)
{
    this->spi_Package.spi_address=address;
    this->spi_Package.spi_cmd=cmd;
    this->spi_Package.spi_len=len;
    int i=0;
    while(i<len)
    {
        this->spi_Package.data[i]=/*data parameter/new for package data/ crc*/;
        i++;
    }

}

int spi_Comm::sending(uint8_t address, uint8_t cmd, uint8_t len)
{
    PackageAssembly(uint8_t address, uint8_t cmd, uint8_t len);

}