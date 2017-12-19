#ifndef CRC_H_INCLUDED
#define CRC_H_INCLUDED

#include <stdint.h>
#define POLYNOMIAL 0xd8
#define BYTE 8
#define WIDTH (BYTE * sizeof(uint16_t))
#define TOPBIT (1 << (WIDTH-1))



typedef uint16_t Crc;

uint16_t addCRC(uint16_t packetCrc,unsigned char countedCrc);

int compareCRC(uint16_t crc1, uint16_t crc2);



#endif // CRC_H_INCLUDED
