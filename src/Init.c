#include <stdio.h>
#include <stdlib.h>
#include "../header/Init.h"
#include "../header/counting.h"
#include "../header/reading.h"
#include "../header/closing.h"

/*************should set back at the end termios***********/

int InitSerialPort(struct termios *old_term,struct termios *term,void *arg)
{
    Threadcommon *init=arg;
    char *serial[4];
    serial[0]="/dev/ttyUSB0";
    serial[1]="/dev/ttyAMA0";
    serial[2]="/dev/ttyS0";
    serial[3]="/dev/ttyS1";

    if(!(init && old_term && init->numbOfDev ))
        return -1;

    init->fd=open(serial[0],O_RDWR | O_NOCTTY | O_NDELAY );

    if(init->fd<0)
        init->fd=open(serial[1],O_RDWR |O_NOCTTY | O_NDELAY);

    if(init->fd<0)
        init->fd=open(serial[2],O_RDWR | O_NOCTTY | O_NDELAY);

    if(init->fd<0)
        init->fd=open(serial[3],O_RDWR | O_NOCTTY | O_NDELAY);

    if(init->fd<0)
        {
            printf("Invalid Filedescriptor\n"
                   "maybe don't connect any wire?");
            syslog(LOG_ERR,"%s",strerror(errno));
            return -1;
        }
    fcntl(init->fd,F_SETFL,O_RDWR);
    term=(struct termios*)malloc(sizeof(struct termios));
    if(!term)
        {
            syslog(LOG_ERR,"There is no memory for term");
            return -1;
        }

    tcgetattr(init->fd,old_term);
    term->c_cflag = CS8 | CLOCAL | CREAD ;
    term->c_iflag &=~(IXON | IXOFF | IXANY);
    term->c_lflag &= ~( ICANON | ECHO | ECHOE | ISIG);
    term->c_oflag =0;
    term->c_cc[VTIME]=0;
    term->c_cc[VMIN]=1;
    cfsetispeed(term,(speed_t)init->BAUD);
    cfsetospeed(term,(speed_t)init->BAUD);

    tcflush(init->fd, TCIOFLUSH);
    if(!tcsetattr(init->fd,TCSANOW,term))
        {
            free(term);
            syslog(LOG_INFO,"Serial port has succesfully initialized");
            return 0;
        }
    else
        {
            closeOnFAIL(init);
            free(term);
            syslog(LOG_ERR,"%s %d",strerror(errno),init->fd);
            close(init->fd);
            return -1;
        }

}

int ReadConfig(Threadcommon *arg)
{
    openlog(NULL,LOG_PID,LOG_LOCAL1);
    if(!arg)
        {
            syslog(LOG_ERR,"threadHandle is NULL");
            return -1;
        }
    char *buffer[MAXLINE];
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
    const char equalsign='=';
    const char tab='\t';
    char *temp=NULL;
    char *p=NULL;
    char *seged=NULL;
    FILE *fconfig=NULL;
    int i,j,len;
    i=0;
    fconfig=fopen(pathOfConfig,"r");
    if(!fconfig)
        {
            printf("There is no or mistaken path has given\n");
            syslog(LOG_ERR,"Wrong path\n");
            return -1;
        }
    /**Read from file line by line. Only reads which are ended by ';'.*/
    else
        {
            temp=(char*)malloc(MAXCHAR*sizeof(char));
            while(fgets(temp,MAXCHAR,fconfig))
                {
                    if(strchr(temp,';'))
                        {
                            buffer[i]=malloc(MAXCHAR*sizeof(char));
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
                                    free(buffer[i]);
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
                                    free(buffer[i]);
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
                                    free(buffer[i]);
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
                                    free(buffer[i]);
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
                                    free(buffer[i]);
                                }
                            if(strstr(buffer[i],"member"))
                                {
                                    if((p[len]='\n'))
                                        p[len-2]='\0';
                                    else
                                        p[len-1]='\0';
                                    arg->members=atoi(p);
                                    if(arg->members<MEMBERSMIN || arg->members>MEMBERSMAX|| !arg->members)
                                        arg->members=MEMBERSMIN;
                                    free(buffer[i]);
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
                    syslog(LOG_ERR,"There are no devices in config ");
                }
            if(!arg->samplingTime)
                arg->samplingTime=DEFTIME;
            if(!arg->time)
                arg->time=REQUESTTIME;
            j=arg->numbOfDev;
            if(j)
                {
                    arg->sensors=malloc(arg->numbOfDev*sizeof(Slaves));
                    if(!arg->sensors)
                        {
                            syslog(LOG_ERR,"No more memory for sensors");
                            return -1;
                        }
                    int sensnmb=0;
                    i=0;
                    int address;
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
                                    if(!arg->sensors[sensnmb].names)
                                        {
                                            syslog(LOG_ERR,"Cannot allocate memory");
                                            return -2;
                                        }
                                    strcpy(arg->sensors[sensnmb].names,seged);
                                    p=buffer[i];
                                    while(isdigit(*p))
                                        p++;
                                    *p='\0';
                                    address=atoi(buffer[i]);
                                    arg->sensors[sensnmb].address=(char)address;
                                    free(buffer[i]);
                                    sensnmb++;
                                    j--;
                                }
                            i++;
                        }
                    printf("Reading config file succesfully\n");
                    syslog(LOG_INFO,"Reading config file succesfully");
                    closelog();
                    p=NULL;
                    seged=NULL;
                    return 0;
                }
            else
                {
                    free(buffer[i]);
                    printf("There is no device number in configFile\nMaybe you gave it bad\n");
                    printf("Example in config list:numberOfDevice=5;\n"
                           "Or numberDevices=2;\n"
                           "Important to end the line with ';'.\n"
                           "Moreover don't use ':' instead of '='.\nThank You!\n ");
                    syslog(LOG_ERR,"no device, I am out");
                    closelog();
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
        {
        printf("Cannot initialize mutex and-or TAILQ\n");
        syslog(LOG_ERR,"Cannot initialize mutex and-or TAILQ");
        return -1;
        }

    else
    {
        printf("TAILQ_INIT and pthread_mutex_init is successfully\n");
        syslog(LOG_INFO,"TAILQ_INIT and pthread_mutex_init is successfully");
        return 0;
    }

}






