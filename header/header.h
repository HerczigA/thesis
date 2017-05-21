#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
#include <sys/queue.h>
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
void ReadConfig(struct config fileConfig);
int Initalization(struct termios *old_term, struct termios *term,int *filedesp,struct config fileConfig,char *lf);

typedef struct incoming_data
{
    /** packet item data */
	char address;
    char cmd;
    uint16_t dlen;
    float *data;
    pthread_mutex_t mutex;
    /**  packet item use a TAILQ. This is for TAILQ entry */
    TAILQ_ENTRY(incoming_data) entries;
};




struct tailhead;



struct config
{
    int time;
    int numbOfDev;
    char *BAUD;
    int samplingTime;
    int Delta;
    int members;
};


int moving_hysteresis();
int takeoutFromQueue();

#endif // HEADER_H_INCLUDED
