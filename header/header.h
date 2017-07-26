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



typedef struct queueData
{
    /** packet item data */
    char address;
    char cmd;               // kell a cmd és a dlen?
    uint16_t dlen;
    float *data;
    pthread_mutex_t mutex;
    /**  packet item use a TAILQ. This is for TAILQ entry */
    TAILQ_ENTRY(queueData) entries;
} QueueData;




struct tailhead
{
} InHd, OutHd;



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

typedef struct threadArg
{
    queueData *Packet;
    int *fd;
    config *conf;
} ThreadArg;


void ReadConfig(Config *fileConfig);
int Initalization(struct termios *old_term, struct termios *term,int *filedesp,Config fileConfig);
int queueInit(QueueData *inData,QueueData *outData);
int takeoutFromQueue(threadArg *arg);

#endif // HEADER_H_INCLUDED
