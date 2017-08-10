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
#include <pthread.h>
#include "../header/header.h"
#include "../header/counting.h"
#include "../header/reading.h"
//#include <wiringPi.h>
#define MAXLINE 512
#define ERRORPATH "/home/herczig/Dokumentumok/errorlog.txt"
#define lf "/home/herczig/Dokumentumok/log.txt"
#define pathOfConfig "/home/herczig/thesis/config.txt"
/*************should set back at the end termios***********/

int InitSerialPort(struct termios *old_term,struct termios *term,Config *fileConfig)
{
   /* /**RPI init and PIN out need to def RX and TX/
    wiringPiSetup();
    pinMode(Rx,INPUT);      //Rx=Pin number
    pinMode(Tx,OUTPUT);     //Tx=Pin number
    /**********************************************/
    int errornum;
    time_t now;
    time(&now);
    char *serial[3];
    serial[0]=="/dev/ttyS0";
    serial[1]=="/dev/ttyS1";
    serial[2]=="/dev/ttyS2";

    FILE * errorfile=fopen(ERRORPATH,"a");

    if(!(fileConfig->fd))
    {
        fprintf(errorfile,"filedescriptor is NULL \t%s\n",ctime(&now));
        return 1;
    }

    else
        fileConfig->fd=open(serial[0],O_RDWR|O_CREAT|O_TRUNC);

    if(fileConfig->fd<0)
        fileConfig->fd=open(serial[1],O_RDWR|O_CREAT|O_TRUNC);

    if(fileConfig->fd<0)
        fileConfig->fd=open(serial[2],O_RDWR|O_CREAT|O_TRUNC);

    if(fileConfig->fd<0)
    {
        errornum=(int)fileConfig->fd;
        fprintf(errorfile,"%s\t%s\n",strerror(errornum),ctime(&now));
        return 1;
    }
    term=(struct termios*)malloc(sizeof(struct termios));
    if(!term)
    {
        errornum=term;
        fprintf(errorfile,"%s\t\t%s\n",strerror(errornum),ctime(&now));
        return -1;
    }
    tcgetattr(fileConfig->fd,old_term);
    term->c_cflag = CS8 | CLOCAL | CREAD ;
    term->c_iflag =0;
    term->c_oflag =0;
    cfsetispeed(term,fileConfig->BAUD);
    cfsetospeed(term,fileConfig->BAUD);

    tcflush(fileConfig->fd, TCIOFLUSH);
    if(!tcsetattr(fileConfig->fd,TCSANOW,term))
    {
        free(fileConfig->BAUD);
        fclose(errorfile);
        return 0;
    }
    else
    {
        free(fileConfig->BAUD);
        fprintf(errorfile,"RS-485 config error:\t%s\n",ctime(&now));
        fclose(errorfile);
        return 1;
    }


}

void ReadConfig(Config *fileConfig)
{
    if(!fileConfig)
        return;
    time_t now;
    int len;
    time(&now);
    char buffer[MAXLINE];
    char *p=NULL;
    int errnum;
    char equalsign='=';



    FILE *fconfig, *errorfile;

    errorfile=fopen(ERRORPATH,"a");
    fconfig=fopen(pathOfConfig,"r");
    errnum=(int)fconfig;
    if(fconfig)
    {

        while(fgets(buffer,MAXLINE,fconfig))
        {

            if(*buffer!='\n')      //minimum the second function
            {

                switch(*buffer)
                {
                case 'R':       //Delay time for sending request
                    p=strchr(buffer,equalsign);
                    p++;
                    p[strlen(p)-1]='\0';
                    fileConfig->time=atoi(p);
                    if(fileConfig->time<REQUESTTIME || fileConfig->time>MAXTIME)
                        fileConfig->time=REQUESTTIME;

                    continue;

                case 'n':       //Number of devices
                    p=strchr(buffer,equalsign);
                    p++;
                    p[strlen(p)-1]='\0';
                    fileConfig->numbOfDev=atoi(p);
                    if(fileConfig->numbOfDev<DevMin || fileConfig->numbOfDev>DevMax)
                        fileConfig->numbOfDev=DevMin;

                    continue;

                case 's':       //SamplingTime
                    p=strchr(buffer,equalsign);
                    p++;
                    p[strlen(p)-1]='\0';
                    fileConfig->samplingTime=atoi(p);
                    if(fileConfig->samplingTime<DEFTIME || fileConfig->samplingTime>MAXTIME)
                        fileConfig->samplingTime=DEFTIME;

                    continue;

                case 'B':       //Baudrate

                    p=strchr(buffer,equalsign);
                    p++;
                    p[strlen(p)-1]='\0';
                    len=strlen(p);
                    fileConfig->BAUD=(char*)malloc(len*sizeof(char)+1);
                    strcpy(fileConfig->BAUD,p);
                    if(strcmp("B9600",p) && strcmp("B38400",p) && strcmp("B57600",p) && strcmp("B115200",p))    //nem kerek
                        fileConfig->BAUD=DefBaud;

                    continue;

                case 'D':           //Delta for moving histeresys
                    p=strchr(buffer,equalsign);
                    p++;
                    p[strlen(p)-1]='\0';
                    fileConfig->Delta=atoi(p);
                    if(fileConfig->Delta<DELTAMIN || fileConfig->Delta>DELTAMAX)
                        fileConfig->Delta=DELTAMIN;

                    continue;

                case 'm':           //members to flowchart
                    p=strchr(buffer,equalsign);
                    p++;
                    p[strlen(p)-1]='\0';
                    errnum=atof(p);
                    fileConfig->members=atof(p);
                    if(fileConfig->members!=MEMBERSMIN || fileConfig->members!=MEMBERSMAX)
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
            fileConfig->samplingTime=DEFTIME;
        if(!fileConfig->time)
            fileConfig->time=REQUESTTIME;
    }
    else
        fprintf(errorfile,"Cannot open config file because %s\t%s\n",strerror(errnum),ctime(&now));

    fclose(fconfig);


}




/**Initialize in-way and out-way queue and mutexes*/
int queueInit(Threadcommon *arg)
{

    if(!(arg))
        return -1;

    //TAILQ_HEAD(&arg->head,queueData) InHead;
    TAILQ_INIT(&arg->head);
    pthread_mutex_init(&arg->mutex,NULL);
    return 0;
}

void giveNumbOfDev_To_Threadcommon(Threadcommon *arg,Config fileconf)
{
    arg->numbofDev=fileconf.numbOfDev;
    arg->members=fileconf.members;
    arg->Delta=fileconf.Delta;
    arg->samplingTime=fileconf.samplingTime;
}




void setBackTermios(Config *fileconf,struct termios *old,struct termios *term)
{
    if(!(old&&term))
        exit(-1);
    tcsetattr(fileconf->fd,TCSANOW,old);
    free(term);
}







