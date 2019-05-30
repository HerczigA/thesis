#ifndef SPI_INIT_H_INCLUDED
#define SPI_INIT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <string>
#include <stdint.h>
#include <syslog.h>

#ifdef __cplusplus
};
#endif

#define  BAD_FD  0

#define  MAXCLK    500000   //MAX SPEED 500kHz

#define MSB 0

const std::string SPI1="/dev/spidev0.0";
const std::string SPI2="/dev/spidev0.1";
const std::string BADSTRING="NULL string";
const std::string BAD_SPI="No SPI modul found";
const std::string SPI_MODE_ERR="Can not set SPI mode";
const std::string SPI_CLK_ERR="Can not set SPI CLK";
const std::string SPI_ENDIAN_WR_ERR="Can not set SPI WR endiness";
const std::string SPI_ENDIAN_RD_ERR="Can not set SPI RD endiness";
using namespace std;

struct SPI_Module
{

    /*
        BCM2835_SPI_CLOCK_DIVIDER_65536 = 0,       ///< 65536 = 262.144us = 3.814697260kHz (total H+L clock period) 
		BCM2835_SPI_CLOCK_DIVIDER_32768 = 32768,   ///< 32768 = 131.072us = 7.629394531kHz
		BCM2835_SPI_CLOCK_DIVIDER_16384 = 16384,   ///< 16384 = 65.536us = 15.25878906kHz
		BCM2835_SPI_CLOCK_DIVIDER_8192  = 8192,    ///< 8192 = 32.768us = 30/51757813kHz
		BCM2835_SPI_CLOCK_DIVIDER_4096  = 4096,    ///< 4096 = 16.384us = 61.03515625kHz
		BCM2835_SPI_CLOCK_DIVIDER_2048  = 2048,    ///< 2048 = 8.192us = 122.0703125kHz
		BCM2835_SPI_CLOCK_DIVIDER_1024  = 1024,    ///< 1024 = 4.096us = 244.140625kHz
		BCM2835_SPI_CLOCK_DIVIDER_512   = 512,     ///< 512 = 2.048us = 488.28125kHz
		BCM2835_SPI_CLOCK_DIVIDER_256   = 256,     ///< 256 = 1.024us = 976.5625MHz
		BCM2835_SPI_CLOCK_DIVIDER_128   = 128,     ///< 128 = 512ns = = 1.953125MHz
		BCM2835_SPI_CLOCK_DIVIDER_64    = 64,      ///< 64 = 256ns = 3.90625MHz
		BCM2835_SPI_CLOCK_DIVIDER_32    = 32,      ///< 32 = 128ns = 7.8125MHz
		BCM2835_SPI_CLOCK_DIVIDER_16    = 16,      ///< 16 = 64ns = 15.625MHz
		BCM2835_SPI_CLOCK_DIVIDER_8     = 8,       ///< 8 = 32ns = 31.25MHz
		BCM2835_SPI_CLOCK_DIVIDER_4     = 4,       ///< 4 = 16ns = 62.5MHz
		BCM2835_SPI_CLOCK_DIVIDER_2     = 2,       ///< 2 = 8ns = 125MHz, fastest you can get
		BCM2835_SPI_CLOCK_DIVIDER_1     = 1,       ///< 1 = 262.144us = 3.814697260kHz, same as 0/65536
	    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_128);
        SPI_CPOL    - Clock polarity
        SPI_CPHA    - Clock phase
        SPI_CS_HIGH - Chip Select active high
        SPI_NO_CS   - 1 device per bus, no Chip Select
        SPI_3WIRE   - Bidirectional mode, data in and out pin shared
        https://github.com/spotify/linux/blob/master/include/linux/spi/spidev.h
        mode = 0,  // CPOL = 0, CPHA = 0, Clock idle low, data is clocked in on rising edge, output data (change) on falling edge
		mode = 1,  // CPOL = 0, CPHA = 1, Clock idle low, data is clocked in on falling edge, output data (change) on rising edge
		mode = 2,  // CPOL = 1, CPHA = 0, Clock idle high, data is clocked in on falling edge, output data (change) on rising edge
		mode = 3,  // CPOL = 1, CPHA = 1, Clock idle high, data is clocked in on rising, edge output data (change) on falling edge
    */
    int CS;
    char TX_buffer[8];
    char RX_buffer[8];  //char
    uint32_t CLK;
    uint8_t mode;
    uint8_t endianess;  //0 MSB other LSB

};

struct package
{
    uint8_t address;
    uint8_t cmd;
    uint8_t len;
    uint8_t *data;
    uint16_t crc;
}Package;

enum cmd
{
    heartbit=0,
    term,
    humidity,
    

};

class spi_Comm
{
    private:
        SPI_Module SPI_unit;
        int spi_fd;
        int init_SPI(string &s);
        string spi_1=SPI1;
        string spi_2=SPI2;
        int PackageAssembly();
        int PackageDismantling();

    public:
        spi_Comm();
        spi_Comm(string &s, uint32_t CLK, uint8_t mode, uint8_t endian);
        int sending();
        int reading();

        
        ~spi_Comm();

};





#endif