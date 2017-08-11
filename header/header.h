#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
#include <sys/queue.h>
#include <stdint.h>
#include <termios.h>
#include "../header/reading.h"
#include <pthread.h>
#define MAXTIME 1000000
#define DEFTIME 100
#define REQUESTTIME 1000
#define DevMin 1
#define DevMax 99
#define DefBaud 9600
#define DELTAMAX 50
#define DELTAMIN 1
#define MEMBERSMIN 3.0
#define MEMBERSMAX 4.0
#define TOPMEASURES 15


typedef struct communication
{
    pthread_mutex_t mutex;
    TAILQ_HEAD(tailhead,queueData) head;
    int fd;
    int time;
    int numbOfDev;
    int BAUD;
    int samplingTime;
    int Delta;
    float members;
} Threadcommon;



void ReadConfig(Threadcommon *arg);
int InitSerialPort(struct termios *old_term,struct termios *term,Threadcommon *arg);
int queueInit(Threadcommon *arg);
void setBackTermios(Threadcommon *arg,struct termios *old,struct termios *term);



#endif // HEADER_H_INCLUDED
