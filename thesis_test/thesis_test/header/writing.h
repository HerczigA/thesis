#ifndef WRITING_H_INCLUDED
#define WRITING_H_INCLUDED

int sendPacket(int fd, unsigned char address, unsigned char cmd,char *data, uint16_t dLen);

#endif // WRITING_H_INCLUDED
