#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
#include <sys/queue.h>
#include <stdint.h>
#include "../header/reading.h"
#include <pthread.h>
#define MAXTIME 1000000
#define DEFTIME 100
#define REQUESTTIME 1000
#define DevMin 1
#define DevMax 99
#define DefBaud "B9600"
#define DELTAMAX 50
#define DELTAMIN 1
#define MEMBERSMIN 3.0
#define MEMBERSMAX 4.0
#define TOPMEASURES 15



typedef struct config
{
    int fd;
    int time;
    int numbOfDev;
    char *BAUD;
    int samplingTime;
    int Delta;
    float members;
} Config;



void ReadConfig(Config *fileConfig);
int InitSerialPort(struct termios *old_term,struct termios *term,Config *fileConfig);
int queueInit(Threadcommon *arg);
void setBackTermios(Config *fileconf,struct termios *old,struct termios *term);
void giveNumbOfDev_To_Threadcommon(Threadcommon *arg,Config fileconf);


#endif // HEADER_H_INCLUDED
