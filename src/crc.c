/*!

Source from Zidarics Zoltan a.k.a Zamek
\version $Id: crc.c v 1.4 2009-05-08 06:16:09 zamek Exp $

*/

#include <stdio.h>
#include <stdlib.h>
#include "../header/reading.h"
#include "../header/crc.h"


uint16_t addCRC(uint16_t packetCrc, unsigned char countedCrc)
{
    char bit;
    packetCrc^= ( countedCrc<< (WIDTH-BYTE));
    for (bit=BYTE; bit >0; --bit)
        if (packetCrc& TOPBIT)
            packetCrc= (packetCrc<< 1) ^ POLYNOMIAL;
        else
            packetCrc<<= 1;
    return packetCrc;
}



int compareCRC(uint16_t crc1, uint16_t crc2)
{
    return crc1==crc2 ? 1:0;
}

