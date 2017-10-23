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
    Threadcommon *init=arg;
    char *serial[4];
    serial[0]="/dev/ttyAMA0";
    serial[1]="/dev/ttyS0";
    serial[1]="/dev/ttyS1";
    serial[2]="/dev/ttyS2";

    if(!(init && old_term && init->numbOfDev ))
        return -1;

    init->fd=open(serial[0],O_RDWR|O_CREAT|O_TRUNC | O_NOCTTY);

        if(init->fd<0)
	    init->fd=open(serial[1],O_RDWR|O_CREAT|O_TRUNC | O_NOCTTY);

	else if(init->fd<0)
    	    init->fd=open(serial[2],O_RDWR|O_CREAT|O_TRUNC | O_NOCTTY);

	else if(init->fd<0)
            init->fd=open(serial[3],O_RDWR|O_CREAT|O_TRUNC | O_NOCTTY);

	else if(init->fd<0)
        {
            syslog(LOG_ERR,"%s\n",strerror(errno));
            return -1;
        }
    term=(struct termios*)malloc(sizeof(struct termios));
    if(!term)
        {
            syslog(LOG_ERR,"There is no memory for term\n");
            return -1;
        }
    tcgetattr(init->fd,old_term);
    term->c_cflag = CS8 | CLOCAL | CREAD ;
    term->c_iflag = IGNPAR;
    term->c_oflag =0;
    term->c_lflag=0;
    term->c_cc[VTIME]=0;
    term->c_cc[VMIN]=1;
    cfsetispeed(term,(speed_t)init->BAUD);
    cfsetospeed(term,(speed_t)init->BAUD);

    tcflush(init->fd, TCIOFLUSH);
    if(!tcsetattr(init->fd,TCSANOW,term))
        {
            syslog(LOG_INFO,"Serial port has succesfully initialized\n");
            printf("%d\n",init->fd);

            return 0;
        }
    else
        {
            syslog(LOG_ERR,"JAAAAAAAAAAAA%s\n",strerror(errno));
            close(init->fd);
            return -1;
        }

}

int ReadConfig(Threadcommon *arg)
{

    if(!arg)
        return -1;

    char *buffer[MAXLINE];
    openlog(NULL,LOG_PID,LOG_LOCAL1);
    arg->BAUD=ZERO;
    arg->Delta=ZERO;
    arg->members=ZERO;
    arg->numbOfDev=ZERO;
    arg->samplingTime=ZERO;
    arg->time=ZERO;

    if(configlist(buffer,arg))
        return -1;
    else
        return 0;

}
int configlist(char **buffer,Threadcommon *arg)
{

    if(!buffer)
        return -1 ;
    const char equalsign='=';
    const char tab='\t';
    char *temp=NULL;
    char *p=NULL;
    char *seged=NULL;
    FILE *fconfig=NULL;
    int i,j,len;
    i=0;
    fconfig=fopen(pathOfConfig,"r");
    // assert(fconfig!=NULL);
    if(!fconfig)
        {
            syslog(LOG_ERR,"Wrong path : %s\n",pathOfConfig);
            return -1;
        }
    /**Read from file line by line. Only reads which are ended by ';'.*/
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
            /**First of all check the array of pointers by the parameters. Checking by '='.*/
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
                }
            if(!arg->samplingTime)
                arg->samplingTime=DEFTIME;
            if(!arg->time)
                arg->time=REQUESTTIME;
            j=arg->numbOfDev;
            syslog(LOG_NOTICE,"BAUDRATE=%d\n",arg->BAUD);
            syslog(LOG_NOTICE,"Delta=%.2f\n",arg->Delta);
            syslog(LOG_NOTICE,"Member=%.2f\n",arg->members);
            syslog(LOG_NOTICE,"sampling Time=%d\n",arg->samplingTime);
            syslog(LOG_NOTICE,"Requesttime=%d\n",arg->time);
            if(j)
                {
                    arg->sensors=malloc(arg->numbOfDev*sizeof(arg->sensors));
                    if(!arg->sensors)
                        {
                            syslog(LOG_ERR,"No more memory for sensors\n");
                            return -1;
                        }

                    int sensnmb=0;
                    i=0;
                    syslog(LOG_NOTICE,"Addresses         Names             State\n");
                    while(j)
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
                                    strcpy(arg->sensors[sensnmb].names,seged);
                                    printf("Names:%s\n",arg->sensors[sensnmb].names);
                                    p=buffer[i];
                                    while(isdigit(*p))
                                        p++;
                                    *p='\0';
                                    arg->sensors[sensnmb].address=atoi(buffer[i]);
                                    syslog(LOG_NOTICE,"%d               %s                %d\n",arg->sensors[sensnmb].address,arg->sensors[sensnmb].names,arg->sensors[sensnmb].state);
                                    sensnmb++;
                                    j--;
                                }
                            i++;

                        }
                    syslog(LOG_INFO,"Reading config file succesfully\n");
                    p=NULL;
                    seged=NULL;
                    return 0;
                }
            else
                {
                    syslog(LOG_ERR,"No Number of Devices!\n");
                    p=NULL;
                    seged=NULL;
                    return -1;
                }


        }

}


/**Initialize in-way and out-way queue and mutexes*/
int queueInit(Threadcommon *arg)
{

    if(!(arg && &arg->head))
        return -1;

    TAILQ_INIT(&arg->head);
    if(pthread_mutex_init(&arg->mutex,NULL))
        return -1;
    else
        return 0;
}


void setBackTermios(Threadcommon *fileconf,struct termios *old,struct termios *term)
{
    openlog(NULL,LOG_PID,LOG_LOCAL1);
    if(!(old&&term&&fileconf))
        {
            syslog(LOG_ERR,"there is a NULL in argumentum list.\n");
            return;
        }
    tcflush(fileconf->fd,TCIOFLUSH);
    tcsetattr(fileconf->fd,TCSANOW,old);
    while(fileconf->numbOfDev--)
        {
            free(fileconf->sensors[fileconf->numbOfDev].names);
            free(&fileconf->sensors[fileconf->numbOfDev]);
        }
    close(fileconf->fd);
    free(term);
    closelog();
}








