#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
#include <sys/queue.h>
#include <stdint.h>
#include <pthread.h>
#define MAXTIME 1000000
#define DEFTIME 100
#define REQUESTTIME 1000
#define DevMin 1
#define DevMax 99
#define DefBaud "B9600"
#define DELTAMAX 50
#define DELTAMIN 1
#define MEMBERSMIN 3
#define MEMBERSMAX 5
#define TOPMEASURES 15



typedef struct config
{
    int fd;
    int time;
    int numbOfDev;
    char *BAUD;
    int samplingTime;
    int Delta;
    int members;
} Config;



void ReadConfig(Config *fileConfig);
int InitSerialPort(struct termios *old_term, struct termios *term,int *filedesp,Config fileConfig);
int queueInit(QueueData *inData,QueueData *outData);


#endif // HEADER_H_INCLUDED
