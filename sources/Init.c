#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/queue.h>
#include <time.h>
#include <string.h>
#include "header/header.h"
#include "header/counting.h"
#include <wiringPi.h>
#define MAXLINE 512
#define ERRORPATH "/home/herczig/Dokumentumok/errorlog.txt"
#define lf "/home/herczig/Dokumentumok/log.txt"
#define pathOfConfig "/home/herczig/thesis/config.txt"

int Initalization(struct termios *old_term, struct termios *term,int *fd,config fileConfig)
{
    /**RPI init and PIN out need to def RX and TX*/
    wiringPiSetup();
    pinMode(Rx,INPUT);      //Rx=Pin number
    pinMode(Tx,OUTPUT);     //Tx=Pin number
    /**********************************************/
    time_t now;
    time(&now);
    char *serial[3];
        serial[0]=="/dev/ttyS0";
        serial[1]=="/dev/ttyS1";
        serial[2]=="/dev/ttyS2";

    FILE * errorfile=fopen(ERRORPATH,"w");

    if(!fd)
    {
        fprintf(errorfile,"filedescriptor is NULL \n",ctime(&now));
        return 1;
    }

    else
        *fd=open(serial[0],O_RDWR|O_CREATE|O_TRUNC);

    if(*fd<0)
        *fd=open(serial[1],O_RDWR|O_CREATE|O_TRUNC);

    if(*fd<0)
        *fd=open(serial[2],O_RDWR|O_CREATE|O_TRUNC);

    if(*fd<0)
    {
        fprintf(errorfile,"Cannot open the filedescriptor \n",ctime(&now));
        return 1;
    }

    tcgetattr(*fd,old_term);
    term=(struct termios*)malloc(sizeof(struct termios));
    if(!term)
    {
        fprintf(errorfile,"NO enough memory to allocate term\n",ctime(&now));
        return 1;
    }
    term->c_cflag = CS8 | CLOCAL | CREAD ;
    term->c_iflag =0;
    term->c_oflag =0;
    cfsetispeed(term,fileConfig.BAUD);
    cfsetospeed(term,fileConfig.BAUD);

    tcflush(*fd, TCIOFLUSH);
    if(!tcsetattr(*fd,TCSANOW,term))
    {
        free(fileConfig.BAUD);
        fclose(errorfile);
        return 0;
    }
    else
    {
        free(fileConfig.BAUD);
        fprintf(errorfile,"RS-485 config error:\n",ctime(&now));
        fclose(errorfile);
        return 1;
    }


}

void ReadConfig(config *fileConfig)
{
    if(!fileConfig)
        return;

    char *buffer;
    char *p=NULL;
    char equalsign='=';



    FILE * fconfig,errorfile;

    errorfile=fopen(ERRORPATH,"w");
    fconfig=fopen(pathOfConfig,"r");

    buffer=(char*)malloc(sizeof(MAXLINE));
    if(!buffer)
    {
        fprintf(errorfile,"Can not allocate memory to buffer\n",ctime(&now));
        return 1;
    }
    if(fconfig)
    {

        while(fgets(buffer,MAXLINE,fconfig))
        {

            if(*buffer!='\n')      //minimum the second function
            {

                switch(*buffer)
                {
                case 'R':       //Request Time  or TIMEOUT?
                    p=strchr(buffer,equalsign);
                    p++;
                    fileConfig.time=atoi(p);
                    if(fileConfig->time<REQUESTTIME || fileConfig->time>MAXTIME)
                        fileConfig->time=REQUESTTIME;

                    continue;

                case 'n':       //Number of devices
                    p=strchr(buffer,equalsign);
                    p++;
                    fileConfig->numbO*fdev=atoi(p);
                    if(fileConfig->numbO*fdev<DevMin || fileConfig->numbO*fdev>DevMax)
                        fileConfig->numbO*fdev=DevMin;

                    continue;

                case 's':       //SamplingTime
                    p=strchr(buffer,equalsign);
                    p++;
                    fileConfig->samplingTime=atoi(p);
                    if(fileConfig->samplingTime<DEFTIME || fileConfig->samplingTime>MAXTIME)
                        fileConfig->samplingTime=DEFTIME;

                    continue;

                case 'B':       //Baudrate
                    int len;
                    p=strchr(buffer,equalsign);
                    p++;
                    len=strlen(p);
                    fileConfig->BAUD=(char*)malloc(len*sizeof(char));
                    strcpy(fileConfig->BAUD,p);
                    if(strcmp("B9600",p) && strcmp("B38400",p) && strcmp("B57600",p) && strcmp("B115200",p))
                        fileConfig->BAUD=DefBaud;

                    continue;

                case 'D':           //Delta for moving histeresys
                    p=strchr(buffer,equalsign);
                    p++;
                    fileConfig->Delta=atoi(p);
                    if(fileConfig->Delta<DELTAMIN || fileConfig->Delta>DELTAMAX)
                        fileConfig->Delta=DELTAMIN;

                    continue;

                case 'm':           //members to flowchart
                    p=strchr(buffer,equalsign);
                    p++;
                    fileConfig->members=atoi(p);
                    if(fileConfig->members<MEMBERSMIN || fileConfig->members>MEMBERSMAX)
                        fileConfig->members=MEMBERSMIN;

                    continue;

                }
            }
            else
                continue;
        }
        if(!fileConfig->BAUD)
        {
            fileConfig->BAUD=(char*)malloc(strlen(DefBaud)*sizeof(char));
            strcpy(fileConfig->BAUD,DefBaud);
        }
        if(!fileConfig->Delta)
            fileConfig->Delta=DELTAMIN;
        if(!fileConfig->members)
            fileConfig->members=MEMBERSMIN;
        if(!fileConfig->numbOfDev)
            fileConfig->numbOfDev=DevMin;
        if(!fileConfig->samplingTime)
            fileConfig->samplingTime=DEFSAMPTIME;
        if(!fileConfig->time)
            fileConfig->time=REQUESTTIME;
    }
    else
        fprintf(fconfig,"Cannot open config file\n",ctime(&now));



    free(buffer);
    fclose(fconfig);


}
/**Initialize in-way and out-way queue and mutexes*/
int queueInit(threadArg *arg,queueData *outData)
{
    if(!(arg&&outData))
        return 1;

    TAILQ_HEAD(tailhead, queueData) OutHd;
    TAILQ_INIT(&OutHd);
    TAILQ_HEAD(tailhead, queueData) InHd;
    TAILQ_INIT(&InHd);

    pthread_mutex_init(arg->mutex,NULL);
    pthread_mutex_init(outData->mutex,NULL);

    return 0;
}










