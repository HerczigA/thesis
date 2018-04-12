#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
#include <sys/queue.h>
#include <stdint.h>
#include <termios.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../header/reading.h"
#define WATCHDOGMAX 3


typedef struct devices
{
    char address;
    char *names;
    int state;
    int time;
    int movAve_tag_number;
    int watchdog;
} Slaves;

typedef struct communication
{
    pthread_mutex_t temperature_mutex,watchdog_mutex;
    TAILQ_HEAD(tailhead,queueData) head;
    int fd;
    int pollTime;
    int numbOfDev;
    int BAUD;
    int samplingTime;
    float Delta;
    int members;
    volatile bool loop;
    Slaves *sensors;
} Threadcommon;

int InitSerialPort(struct termios *old_term,struct termios *term,void *arg);

int queueInit(Threadcommon *arg);

#endif // HEADER_H_INCLUDED
