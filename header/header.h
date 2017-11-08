#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
#include <sys/queue.h>
#include <stdint.h>
#include <termios.h>
#include "../header/reading.h"
#include <pthread.h>
#define DEFTIME 10
#define DEFMAXTIME 60
#define REQUESTTIME 60
#define MAXTIME 10800
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <syslog.h>
#include <ctype.h>
#include <errno.h>
#define ZERO 0
#define DEVMIN 0
#define DEVMAX 99
#define DEFBAUD 9600
#define DELTAMAX 50.0
#define DELTAMIN 1.0
#define MEMBERSMIN 3.0
#define MEMBERSMAX 4.0
#define TOPMEASURES 15
#define MAXLINE 100
#define MAXCHAR 128
//#define pathOfConfig "/home/herczig/thesis/util/config.txt"
//#define pathOfConfig "/home/herczig/Dokumentumok/thesis/thesis/util/config.txt"
#define pathOfConfig "/home/pi/thesis/util/config.txt"

typedef struct devices
{
    char address;
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
    float Delta;
    int members;
    Slaves *sensors;

} Threadcommon;


int configlist(char **buffer,Threadcommon *arg);
int ReadConfig(Threadcommon *arg);
int InitSerialPort(struct termios *old_term,struct termios *term,void *arg);
int queueInit(Threadcommon *arg);
void setBackTermios(Threadcommon *arg,struct termios *old);



#endif // HEADER_H_INCLUDED
