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
#define MAXLINE 512
#define ERRORPATH "/home/herczig/Dokumentumok/errorlog.txt"
#define lf "/herczig/Dokumentumok/log.txt"
#define pathOfConfig "/home/herczig/thesis/config.txt"

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

void ReadConfig(config *fileConfig,int *fd)
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
                        case 'R':       //Request Time
                            p=strchr(buffer,equalsign);
                            p++;
                            fileConfig.time=atoi(p);
                            if(fileConfig->time<REQUESTTIME|| fileConfig->time>MAXTIME)
                                fileConfig->time=REQUESTTIME;

                        continue;

                        case 'n':       //Number of devices
                            p=strchr(buffer,equalsign);
                            p++;
                            fileConfig->numbOfDev=atoi(p);
                            if(fileConfig->numbOfDev<DevMin || fileConfig->numbOfDev>DevMax)
                                fileConfig->numbOfDev=DevMin;

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
                            fileConfig->BAUD=(char*)malloc(sizeof(len));
                            strcpy(fileConfig->BAUD,p);
                            if(fileConfig->BAUD!="B9600" ||fileConfig->BAUD!="B38400" ||fileConfig->BAUD!="B57600" ||fileConfig->BAUD!="B115200" )
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
            }
            if(!fileConfig->BAUD)
                {
                    fileConfig->BAUD=(char*)malloc(DefBaud);
                    fileConfig->BAUD=DefBaud;
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
int queueInit(queueData *inData,queueData *outData)
{
    if(!(inData&&outData))
        return 1;

        TAILQ_HEAD(tailhead, queueData) OutHd;
        TAILQ_INIT(&OutHd);
	    TAILQ_HEAD(tailhead, queueData) InHd;
        TAILQ_INIT(&InHd);

    pthread_mutex_init(inData->mutex,NULL);
    pthread_mutex_init(outData->mutex,NULL);
    return 0;
}



int takeoutFromQueue(threadArg *arg)
{
    int movAverArray[arg->conf.members]={0};
    float *temp;
    float finalResult;
    int i=0;
    float data=0;
    queueData *tempPacket;
    time_t now;
    time(&now);
    char *time;
    time=(char*)malloc(TIMELINE*sizeof(char));
    time=timeToString(time);
    FILE * log_file=fopen(time,"w");

         while(!TAILQ_EMPTY(&InHd))
         {
            pthread_mutex_lock(arg->Packet->mutex);     //second ->?
            tempPacket=TAILQ_FIRST(&InHd);
            TAILQ_REMOVE(&InHd,tempPacket,entries);
            pthread_mutex_unlock(arg->Packet->mutex);

            if(tempPacket->data)
            {
                *temp=mov_average(movAverArray, &data, i, *arg,tempPacket);
                i++;
                if (i>= arg->conf.members)
                    i= 0;
                finalResult=moving_hysteresis(conffile,temp);
                fprintf(log_file,"Measured temperature from %s address of device with moving average and moving hysteresis :%d\t %s\n",
                        tempPacket->address,finalResult,ctime(&now));

            }
            sleep(arg->conf.samplingTime);
         }

     fclose(log_file);
     free(tempPacket);
     free(time);
}






