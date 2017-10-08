#include <stdio.h>
#include <stdlib.h>
#include "../header/header.h"
#include "../header/counting.h"
#include "../header/reading.h"
#include <assert.h>

/*************should set back at the end termios***********/

int InitSerialPort(struct termios *old_term,struct termios *term,void *arg)
{
    //RPI init and PIN out need to def RX and TX/
    wiringPiSetup();
    pinMode(RX,INPUT);      //Rx=Pin number
    pinMode(TX,OUTPUT);     //Tx=Pin number
    //*********************************************
    int errornum;
    Threadcommon *init=arg;
    char *serial[3];
    serial[0]="/dev/ttyS0";
    serial[1]="/dev/ttyS1";
    serial[2]="/dev/ttyS2";

    openlog(NULL,LOG_PID,LOG_LOCAL1);
    if(!init || !init->numbOfDev)
        return -1;

    init->fd=open(serial[0],O_RDWR|O_CREAT|O_TRUNC);
    if(init->fd<0)
        init->fd=open(serial[1],O_RDWR|O_CREAT|O_TRUNC);

    if(init->fd<0)
        init->fd=open(serial[2],O_RDWR|O_CREAT|O_TRUNC);

    if(init->fd<0)
        {
            errornum=(int)init->fd;
            syslog(LOG_ERR,"%s\n",strerror(errornum));
            return 1;
        }
    term=(struct termios*)malloc(sizeof(struct termios));
    if(!term)
        {
            syslog(LOG_ERR,"There is no memory for term\n");
            return 1;
        }
    tcgetattr(init->fd,old_term);
    term->c_cflag = CS8 | CLOCAL | CREAD ;
    term->c_iflag =0;
    term->c_oflag =0;
    cfsetispeed(term,init->BAUD);
    cfsetospeed(term,init->BAUD);

    tcflush(init->fd, TCIOFLUSH);
    errornum=tcsetattr(init->fd,TCSANOW,term);
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
    assert(arg);
    if(!arg)
        return;

    char *buffer[MAXLINE];

    arg->BAUD=ZERO;
    arg->Delta=ZERO;
    arg->members=ZERO;
    arg->numbOfDev=ZERO;
    arg->samplingTime=ZERO;
    arg->time=ZERO;

    configlist(buffer,arg);

}
void configlist(char **buffer,Threadcommon *arg)
{
    if(!buffer)
        return ;
    const char equalsign='=';
    const char tab='\t';
    char *temp=NULL;
    char *p=NULL;
    char *seged=NULL;
    FILE *fconfig=NULL;
    int i,j,len;
    i=j=0;
    openlog(NULL,LOG_PID,LOG_LOCAL1);
    fconfig=fopen(pathOfConfig,"r");
    if(!fconfig)
        {
            syslog(LOG_ERR,"Wrong path\n");
            return;
        }
    else
        {
            temp=malloc(MAXCHAR*sizeof(char));

            while(fgets(temp,MAXCHAR,fconfig))
                {
                    if(strchr(temp,';'))
                        {
                            buffer[i]=malloc((strlen(temp))*sizeof(char));
                            strcpy(buffer[i],temp);
                            i++;
                        }

                }
            fclose(fconfig);
            free(temp);
            i=0;
            while(!(strchr(buffer[i],tab)))
                {
                    if((p=strchr(buffer[i],equalsign)))
                        {
                            len=strlen(p)-1;
                            p++;
                            if(strstr(buffer[i],"Request"))
                                {
                                    if((p[len]='\n'))
                                        p[len-2]='\0';
                                    else
                                        p[len-1]='\0';
                                    arg->time=atoi(p);
                                    if(arg->time<REQUESTTIME || arg->time>MAXTIME || !arg->time)
                                        arg->time=REQUESTTIME;

                                }
                            if(strstr(buffer[i],"Device"))
                                {

                                    if((p[len]='\n'))
                                        p[len-2]='\0';
                                    else
                                        p[len-1]='\0';

                                    arg->numbOfDev=atoi(p);
                                    if(arg->numbOfDev<DEVMIN|| arg->numbOfDev>DEVMAX|| !arg->numbOfDev)
                                        arg->numbOfDev=DEVMIN;
                                }
                            if(strstr(buffer[i],"sampling"))
                                {

                                    if((p[len]='\n'))
                                        p[len-2]='\0';
                                    else
                                        p[len-1]='\0';

                                    arg->samplingTime=atoi(p);
                                    if(arg->samplingTime<DEFTIME|| arg->samplingTime>MAXTIME|| !arg->samplingTime)
                                        arg->samplingTime=DEFTIME;
                                }
                            if(strstr(buffer[i],"Baud"))
                                {

                                    if((p[len]='\n'))
                                        p[len-2]='\0';
                                    else
                                        p[len-1]='\0';
                                    arg->BAUD=atoi(p);
                                    if(!(arg->BAUD==9600 || arg->BAUD==38400 || arg->BAUD==57600 || arg->BAUD==115200 ))
                                        arg->BAUD=DEFBAUD;

                                }
                            if(strstr(buffer[i],"Delta"))
                                {

                                    if((p[len]='\n'))
                                        p[len-2]='\0';
                                    else
                                        p[len-1]='\0';

                                    arg->Delta=atof(p);
                                    if(arg->Delta<DELTAMIN || arg->Delta>DELTAMAX|| !arg->Delta)
                                        arg->Delta=DELTAMIN;
                                }
                            if(strstr(buffer[i],"member"))
                                {

                                    if((p[len]='\n'))
                                        p[len-2]='\0';
                                    else
                                        p[len-1]='\0';

                                    arg->members=atof(p);
                                    if(arg->members<MEMBERSMIN || arg->members>MEMBERSMAX|| !arg->members)
                                        arg->members=MEMBERSMIN;

                                }
                            i++;
                        }
                }
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
                    closelog();
                }
            if(!arg->samplingTime)
                arg->samplingTime=DEFTIME;
            if(!arg->time)
                arg->time=REQUESTTIME;

            if(arg->numbOfDev)
                {
                    arg->sensors=malloc(arg->numbOfDev*sizeof(arg->sensors));
                    //
                    assert(arg->sensors);
                    if(!arg->sensors)
                    {
                        syslog(LOG_ERR,"No more memory for sensors\n");
                        return;
                    }


                    int sensnmb=0;
                    i=0;
                    while(--arg->numbOfDev)
                        {
                            if((p=strrchr(buffer[i],tab)))
                                {
                                    p++;
                                    seged=buffer[i];
                                    len=strlen(p)-1;
                                    if(p[len]=='\n')
                                        {
                                            p[len-1]='\0';
                                            arg->sensors[sensnmb].state=atoi(p);
                                        }
                                    else if(p[len]==';')
                                        {
                                            p[len]='\0';
                                            arg->sensors[sensnmb].state=atoi(p);
                                        }
                                    while(!(isalpha(*seged)))
                                        seged++;
                                    p=seged;
                                    while(isalpha(*p))
                                        p++;
                                    *p='\0';
                                    arg->sensors[sensnmb].names=malloc((p-seged)*sizeof(char));
                                    strcpy(arg->sensors[sensnmb].names,p);
                                    p=buffer[i];
                                    while(isdigit(*p))
                                        p++;
                                    *p='\0';
                                    arg->sensors[sensnmb].address=atoi(buffer[i]);
                                    i++;
                                    sensnmb++;


                                }
                        }
                }
                closelog();
            p=NULL;
            seged=NULL;

        }

}





/**Initialize in-way and out-way queue and mutexes*/
int queueInit(Threadcommon *arg)
{
    assert(arg);
    if(!arg)
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








