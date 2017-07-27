#ifndef CRC_H_INCLUDED
#define CRC_H_INCLUDED

#include <stdint.h>
#define POLYNOMIAL 0xd8

#define TOPBIT (1 << (WIDTH-1))
#define BYTE 8


typedef uint16_t Crc;

uint16_t addCRC(uint16_t packetCrc, uint16_t countedCrc);

int compareCRC(uint16_t crc1, uint16_t crc2);



#endif // CRC_H_INCLUDED
