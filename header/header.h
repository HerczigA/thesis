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
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/queue.h>
#include <string.h>
#include <pthread.h>
#include <wiringPi.h>
#include <syslog.h>
#include <ctype.h>
#define RX 15
#define TX 16
#define DEVMIN 0
#define DEVMAX 99
#define DEFBAUD 9600
#define DELTAMAX 50
#define DELTAMIN 1
#define MEMBERSMIN 3.0
#define MEMBERSMAX 4.0
#define TOPMEASURES 15
#define MAXLINE 512
#define pathOfConfig "/home/herczig/Dokumentumok/thesis/thesis/util/config.txt"

typedef struct devices
{
    int address;
    char *names;
    int state;

}Slaves;


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
    Slaves *sensors;

} Threadcommon;



void ReadConfig(Threadcommon *arg);
int InitSerialPort(struct termios *old_term,struct termios *term,Threadcommon *arg);
int queueInit(Threadcommon *arg);
void setBackTermios(Threadcommon *arg,struct termios *old,struct termios *term);



#endif // HEADER_H_INCLUDED
