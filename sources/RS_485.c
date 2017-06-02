#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/queue.h>
#include <time.h>
#include "header/header.h"
#include "header/counting.h"
#define MAXLINE 1000
#define ERRORPATH "/home/herczig/Dokumentumok/errorlog.txt"
#define lf "/herczig/Dokumentumok/log.txt"

int Initalization(struct termios *old_term, struct termios *term,int *filedesp,config fileConfig)
{

    time_t now;
    time(&now);
    char *serial="/dev/ttyS0";
    int fd;

    FILE * errorfile=fopen(ERRORPATH,"w");

    if(!filedesp)
        {
        fprintf(errorfile,"filedescriptor is NULL\n",ctime(&now));
        return 1;
        }

    else
        fd=*filedesp;
        fd=open(serial,O_RDWR);
            if(fd<0)
        {
            fprintf(errorfile,"Cannot open the filedescriptor \n",ctime(&now));
            return 1;
        }

    tcgetattr(fd,old_term);
    term=malloc(sizeof(struct termios));
        if(!term)
        {
            fprintf(errorfile,"NO enough memory to allocate term\n",ctime(&now));
            return 1;
        }
    term->c_cflag = CS8 | CLOCAL | CREAD ;
    term->c_iflag =0;
    term->c_oflag= 0;
    cfsetispeed(term,fileConfig.BAUD);
    cfsetospeed(term,fileConfig.BAUD);

    tcflush(fd, TCIFLUSH);
    if(!tcsetattr(fd,TCSANOW,term))
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
    fileConfig=malloc(sizeof(config));

char *buffer;
char *p=NULL;
char pathOfConfig[]="/home/herczig/thesis/config.txt";

char equalsign='=';

struct Repair
    {
        char RT;
        char NOD;
        char ST;
        char BR;
        char Delta;
        char MOMA;
    };



FILE * fconfig,errorfile;

    errorfile=fopen(lf,"w");
    fconfig=fopen(pathOfConfig,"r");

    buffer=malloc(sizeof(MAXLINE));
        if(!buffer)
        {
            fprintf(errorfile,"Can not allocate memory to buffer\n",ctime(&now));
            return 1;
        }
        if(fconfig)
        {
            struct Repair ConfReplacement={0,0,0,0,0,0};;
            while(fgets(buffer,MAXLINE,fconfig))
            {

                if(*buffer!='\n')      //minimum the second function
                {

                   switch(*buffer)
                    {
                        case 'R':       //Request Time
                            p=strchr(buffer,equalsign);
                            p++;
                            fileConfig.time=atoi(p);
                            if(fileConfig->time<1000 || fileConfig->time>1000000)
                                fileConfig->time=1000;
                            ConfRepalcement.RT++;
                        continue;

                        case 'n':       //Number of devices
                            p=strchr(buffer,equalsign);
                            p++;
                            fileConfig->numbOfDev=atoi(p);
                            if(fileConfig->numbOfDev<DevMin || fileConfig->numbOfDev>DevMax)
                                fileConfig->numbOfDev=DevMin;
                                ConfReplacement.NOD++;
                        continue;

                        case 's':       //SamplingTime
                            p=strchr(buffer,equalsign);
                            p++;
                            fileConfig->samplingTime=atoi(p);
                            if(fileConfig->samplingTime<DEFTIME || fileConfig->samplingTime>MAXTIME)
                                fileConfig->samplingTime=DEFTIME;
                                ConfReplacement.ST++;
                        continue;

                       case 'B':       //Baudrate
                            p=strchr(buffer,equalsign);
                            p++;
                            fileConfig->BAUD=malloc(sizeof(p));
                            strcpy(fileConfig->BAUD,p);
                            if(fileConfig->BAUD!="B9600" ||fileConfig->BAUD!="B38400" ||fileConfig->BAUD!="B57600" ||fileConfig->BAUD!="B115200" )
                                fileConfig->BAUD=DefBaud;
                                ConfReplacement.BR++;
                        continue;

                        case 'D':           //Delta for moving histeresys
                            p=strchr(buffer,equalsign);
                            p++;
                            fileConfig->Delta=atoi(p);
                            if(fileConfig->Delta<DELTAMIN || fileConfig->Delta>DELTAMAX)
                                fileConfig->Delta=DELTAMIN;
                                ConfReplacement.Delta++;
                            continue;

                       case 'm':           //members to flowchart
                            p=strchr(buffer,equalsign);
                            p++;
                            fileConfig->members=atoi(p);
                            if(fileConfig->members<MEMBERSMIN || fileConfig->members>MEMBERSMAX)
                                fileConfig->members=MEMBERSMIN;
                                ConfReplacement.MOMA++;
                            continue;

                    }
                }
            }


        }
        else
            fprintf(fconfig,"Cannot open config file\n",ctime(&now));



        free(buffer);
        fclose(fconfig);


}
/**Direction=0 if OUTPUT, Direction =1 if INPUT*/
int queueInit(queueData *data)       //direction INPUT or OUTPUT with queue!?!?!?!?!?!?!
{
    data=malloc(sizeof(queueData));
        if(!data)
            return 1;

        //struct tailhead ;

        TAILQ_HEAD(tailhead, queueData) OutHd;
        TAILQ_INIT(&OutHd);
	    TAILQ_HEAD(tailhead, queueData) InHd;
        TAILQ_INIT(&InHd);

    pthread_mutex_init(data->mutex,NULL);
    return 0;
}



int takeoutFromQueue(config conffile,queueData *Received_data)
{
    int movAverArray[conffile.members]={0};
    float *temp;
    float finalResult;
    int i=0;
    float data=0;
    queueData *tempPacket;
    time_t now;
    time(&now);

    FILE * log_file=fopen(lf,"w");

         while(!TAILQ_EMPTY(&InHd))
         {
            pthread_mutex_lock(Received_data->mutex);
            tempPacket=TAILQ_FIRST(&InHd);
            TAILQ_REMOVE(&InHd,tempPacket,entries);
            pthread_mutex_unlock(Received_data->mutex);

            if(tempPacket->data)
            {
                *temp= mov_average(movAverArray, &data, i, conffile,tempPacket);
                i++;
                if (i>= conffile.members)
                    i= 0;
                finalResult=moving_hysteresis(conffile,temp);
                fprintf(log_file,"Measured temperature from %s address of device with moving average and moving hysteresis :%d\t %s\n",tempPacket->address,finalResult,ctime(&now));

            }
            sleep(conffile.samplingTime);
         }

     free(tempPacket);
}



int controllingProcess()
{

}



