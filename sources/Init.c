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
    char *serial[3];
    serial[0]="/dev/ttyS0";
    serial[1]="/dev/ttyS1";
    serial[2]="/dev/ttyS2";
    openlog(NULL,LOG_PID,LOG_LOCAL1);

    arg->fd=open(serial[0],O_RDWR|O_CREAT|O_TRUNC);
    if(arg->fd<0)
        arg->fd=open(serial[1],O_RDWR|O_CREAT|O_TRUNC);

    if(arg->fd<0)
        arg->fd=open(serial[2],O_RDWR|O_CREAT|O_TRUNC);

    if(arg->fd<0)
        {
            errornum=(int)arg->fd;
            syslog(LOG_ERR,"%s\n",strerror(errornum));
            return -1;
        }
    term=(struct termios*)malloc(sizeof(struct termios));
    if(!term)
        {
            errornum=(int)term;
            syslog(LOG_ERR,"%s\n",strerror(errornum));
            return -2;
        }
    tcgetattr(arg->fd,old_term);
    term->c_cflag = CS8 | CLOCAL | CREAD ;
    term->c_iflag =0;
    term->c_oflag =0;
    cfsetispeed(term,arg->BAUD);
    cfsetospeed(term,arg->BAUD);

    tcflush(arg->fd, TCIOFLUSH);
    errornum=tcsetattr(arg->fd,TCSANOW,term);
    if(!errornum)
        {
            closelog();
            return 0;
        }
    else
        {
            syslog(LOG_ERR,"%s\n",strerror(errornum));
            closelog();
            return 1;
        }

}

void ReadConfig(Threadcommon *arg)
{
    if(!arg)
        return;
    char *buffer[MAXLINE];
    char *temp;
    char *masod=NULL;
    char *p=NULL;
    int errnum;
    int i=ZERO;
    int len;
    const char equalsign='=';
    const char tab='\t';
    openlog(NULL,LOG_PID,LOG_LOCAL1);

    arg->BAUD=ZERO;
    arg->Delta=ZERO;
    arg->members=ZERO;
    arg->numbOfDev=ZERO;
    arg->samplingTime=ZERO;
    arg->time=ZERO;

    i=configlist(buffer);
    while(--i)
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
                                    else
                                        arg->sensors=malloc(arg->numbOfDev*sizeof(arg->sensors));

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
                                    arg->members=atof(p);
                                    if(arg->members!=MEMBERSMIN || arg->members!=MEMBERSMAX)
                                        arg->members=MEMBERSMIN;

                                    continue;

                                case 'a':           //addresses list
                                    arg->sensors=malloc(arg->numbOfDev*sizeof(arg->sensors));
                                    while(i!=arg->numbOfDev)
                                        {
                                            fgets(buffer,MAXLINE,fconfig);
                                            p=strrchr(buffer,tab);
                                            masod=++p;
                                            p=p+2;
                                            *p='\0';
                                            arg->sensors[i].state=atoi(masod);
                                            p=strchr(buffer,tab);
                                            while(!(isalpha(*p)))
                                                p++;
                                            masod=p;
                                            while(isalpha(*p))
                                                p++;
                                            *p='\0';
                                            len=strlen(masod);
                                            arg->sensors[i].names=malloc(len*sizeof(char));
                                            strcpy(arg->sensors[i].names,masod);
                                            p=strchr(buffer,tab);
                                            *p='\0';
                                            arg->sensors[i].address=atoi(buffer);
                                            i++;

                                        }
                                    }
                        }
                    else
                        continue;
                }

        syslog(LOG_ERR,"%s\n",strerror(errnum));
            if(!arg->BAUD)
                arg->BAUD=DEFBAUD;
            if(!arg->Delta)
                arg->Delta=DELTAMIN;
            if(!arg->members)
                arg->members=MEMBERSMIN;
            if(!arg->numbOfDev)
                {
                    arg->numbOfDev=DEVMIN;
                    syslog(LOG_ERR,"There are no devices in config \n");
                }
            if(!arg->samplingTime)
                arg->samplingTime=DEFTIME;
            if(!arg->time)
                arg->time=REQUESTTIME;




    fclose(fconfig);
    closelog();


}





/**Initialize in-way and out-way queue and mutexes*/
int queueInit(Threadcommon *arg)
{

    if(!(arg))
        return -1;

    TAILQ_INIT(&arg->head);
    pthread_mutex_init(&arg->mutex,NULL);
    return 0;
}






void setBackTermios(Threadcommon *fileconf,struct termios *old,struct termios *term)
{
    if(!(old&&term))
        exit(-1);
    tcsetattr(fileconf->fd,TCSANOW,old);
    while(fileconf->numbOfDev--)
        {
            free(fileconf->sensors[fileconf->numbOfDev].names);
            free(&fileconf->sensors[fileconf->numbOfDev]);
        }
    free(term);
}



int configlist(char **buffer)
{
    char *temp=NULL;
    FILE *fconfig;
    int i=0;
    fconfig=fopen(pathOfConfig,"r");
    temp=malloc(MAXCHAR*sizeof(char));

    while(fgets(temp,MAXCHAR,fconfig))
    {
        if(strchr(temp,';'))
            {
                buffer[i]=malloc((strlen(temp)-1)*sizeof(char));
                temp[strlen(temp)-1]='\0';
                strcpy(buffer[i],temp);
                i++;
            }

    }

    i--;
    free(temp);
    return i;
}





