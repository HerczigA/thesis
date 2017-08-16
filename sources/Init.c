#include <stdio.h>
#include <stdlib.h>
#include "../header/header.h"
#include "../header/counting.h"
#include "../header/reading.h"

/*************should set back at the end termios***********/

int InitSerialPort(struct termios *old_term,struct termios *term,Threadcommon *arg)
{
     //RPI init and PIN out need to def RX and TX/
     wiringPiSetup();
     pinMode(RX,INPUT);      //Rx=Pin number
     pinMode(TX,OUTPUT);     //Tx=Pin number
     //*********************************************
    int errornum;
    time_t now;
    time(&now);
    char *serial[3];
    serial[0]="/dev/ttyS0";
    serial[1]="/dev/ttyS1";
    serial[2]="/dev/ttyS2";

    FILE * errorfile=fopen(ERRORPATH,"a");
    arg->fd=open(serial[0],O_RDWR|O_CREAT|O_TRUNC);

    if(arg->fd<0)
        arg->fd=open(serial[1],O_RDWR|O_CREAT|O_TRUNC);

    if(arg->fd<0)
        arg->fd=open(serial[2],O_RDWR|O_CREAT|O_TRUNC);

    if(arg->fd<0)
    {
        errornum=(int)arg->fd;
        fprintf(errorfile,"%s\t%s\n",strerror(errornum),ctime(&now));
        return 1;
    }
    term=(struct termios*)malloc(sizeof(struct termios));
    if(!term)
    {
        errornum=(int)term;
        fprintf(errorfile,"%s\t\t%s\n",strerror(errornum),ctime(&now));
        return -1;
    }
    tcgetattr(arg->fd,old_term);
    term->c_cflag = CS8 | CLOCAL | CREAD ;
    term->c_iflag =0;
    term->c_oflag =0;
    cfsetispeed(term,arg->BAUD);
    cfsetospeed(term,arg->BAUD);

    tcflush(arg->fd, TCIOFLUSH);
    if(!tcsetattr(arg->fd,TCSANOW,term))
    {

        fclose(errorfile);
        return 0;
    }
    else
    {

        fprintf(errorfile,"RS-485 config error:\t%s\n",ctime(&now));
        fclose(errorfile);
        return 1;
    }


}

void ReadConfig(Threadcommon *arg)
{
    if(!arg)
        return;
    time_t now;
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
                    arg->time=atoi(p);
                    if(arg->time<REQUESTTIME || arg->time>MAXTIME)
                        arg->time=REQUESTTIME;

                    continue;

                case 'n':       //Number of devices
                    p=strchr(buffer,equalsign);
                    p++;
                    p[strlen(p)-1]='\0';
                    arg->numbOfDev=atoi(p);
                    if(arg->numbOfDev<DEVMIN || arg->numbOfDev>DEVMAX)
                        arg->numbOfDev=DEVMIN;

                    continue;

                case 's':       //SamplingTime
                    p=strchr(buffer,equalsign);
                    p++;
                    p[strlen(p)-1]='\0';
                    arg->samplingTime=atoi(p);
                    if(arg->samplingTime<DEFTIME || arg->samplingTime>MAXTIME)
                        arg->samplingTime=DEFTIME;

                    continue;

                case 'B':       //Baudrate

                    p=strchr(buffer,equalsign);
                    p++;
                    p[strlen(p)-1]='\0';
                    arg->BAUD=atoi(p);
                    if(!(arg->BAUD==9600 || arg->BAUD==38400 || arg->BAUD==57600 || arg->BAUD==115200 ))
                        arg->BAUD=DEFBAUD;

                    continue;

                case 'D':           //Delta for moving histeresys
                    p=strchr(buffer,equalsign);
                    p++;
                    p[strlen(p)-1]='\0';
                    arg->Delta=atoi(p);
                    if(arg->Delta<DELTAMIN || arg->Delta>DELTAMAX)
                        arg->Delta=DELTAMIN;

                    continue;

                case 'm':           //members to flowchart
                    p=strchr(buffer,equalsign);
                    p++;
                    p[strlen(p)-1]='\0';
                    errnum=atof(p);
                    arg->members=atof(p);
                    if(arg->members!=MEMBERSMIN || arg->members!=MEMBERSMAX)
                        arg->members=MEMBERSMIN;

                    continue;

                }
            }
            else
                continue;
        }
        if(!arg->BAUD)
            arg->BAUD=DEFBAUD;
        if(!arg->Delta)
            arg->Delta=DELTAMIN;
        if(!arg->members)
            arg->members=MEMBERSMIN;
        if(!arg->numbOfDev)
            arg->numbOfDev=DEVMIN;
        if(!arg->samplingTime)
            arg->samplingTime=DEFTIME;
        if(!arg->time)
            arg->time=REQUESTTIME;
    }
    else
        fprintf(errorfile,"Cannot open Threadcommon file because %s\t%s\n",strerror(errnum),ctime(&now));

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






void setBackTermios(Threadcommon *fileconf,struct termios *old,struct termios *term)
{
    if(!(old&&term))
        exit(-1);
    tcsetattr(fileconf->fd,TCSANOW,old);
    free(term);
}







