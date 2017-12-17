#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include "../header/Init.h"
#include "../header/config.h"


int config(Threadcommon *arg)
{
    openlog(NULL,LOG_PID,LOG_LOCAL1);
    if(!arg)
        {
            printf("ReadConfig got NULL\n");
            syslog(LOG_ERR,"threadHandle is NULL");
            return -1;
        }
    int line,allLines;
    char *buffer[MAXLINE];
    arg->BAUD=ZERO;
    arg->Delta=ZERO;
    arg->members=ZERO;
    arg->numbOfDev=ZERO;
    arg->samplingTime=ZERO;
    arg->pollTime=ZERO;

    allLines=Read_config(buffer);
    if(allLines==-1)
        return -1;
    line=Processing_Config(buffer,arg) ;
    if(line==-1)
    {
        free_configBuffer(buffer,allLines);
        return -1;
    }

    if(!deviceparameters(buffer,arg,line,allLines))
    {
        free_configBuffer(buffer,allLines);
        return 0;
    }
    else
    {
        free_configBuffer(buffer,allLines);
        return -1;
    }
}




int Read_config(char **buffer)
{
    char *temp=NULL;
    FILE *fconfig=NULL;
    int i,j;
    i=j=0;
    fconfig=fopen(pathOfConfig,"r");
    if(!fconfig)
        {
            printf("There is no or mistaken path has given\n");
            syslog(LOG_ERR,"Wrong path\n");
            return -1;
        }
    /**Read from file line by line. Only reads which are ended by ';'.*/
    temp=(char*)malloc(MAXCHAR*sizeof(char));
    if(!temp)
        {
            perror("temp:\n");
            fclose(fconfig);
            return -1;
        }
    while(fgets(temp,MAXCHAR,fconfig))
        {
            if(*temp=='\n' || *temp=='#')
                continue;
            if((strchr(temp,'=')))
            {
                buffer[i]=(char*)malloc(MAXLINE*sizeof(char));
                    if(!buffer[i])
                        {
                            if(i>1)
                                j=i-1;
                            while(j!=-1)
                                {
                                    free(buffer[j]);
                                    j--;
                                }
                            printf("no memory to buffer[i]\n");
                            free(temp);
                            fclose(fconfig);
                            return -1;
                        }
                    strcpy(buffer[i],temp);
                    i++;
                }
        }
    i--;
    fclose(fconfig);
    free(temp);
    return i;
}



int Processing_Config(char **configbuffer,Threadcommon *arg)
{
    if(!configbuffer)
        {
            perror("configbuffer:");
            return -1;
        }
    int i;
    char *p=NULL;
    char *s=NULL;
    i=0;
    /**First of all, check the array of pointers by the parameters. Checking by '='.*/
    while(i<5)
        {
            p=strchr(configbuffer[i],'=');
            p++;
            s=strtok(configbuffer[i],"=");
            if(strcmp(s,"Serial.PollingTime")==0) //buffereket nem elfelejteni törölni!!!!
                {
                    arg->pollTime=atoi(p);
                    if(arg->pollTime<POLLTIME || arg->pollTime>MAXTIME )
                        arg->pollTime=POLLTIME;
                    i++;
                    continue;
                }
            else if(strcmp(s,"Serial.NumDevice")==0)
                {
                    arg->numbOfDev=atoi(p);
                    if(arg->numbOfDev<DEVMIN || arg->numbOfDev>DEVMAX)
                        arg->numbOfDev=DEVMIN;
                    i++;
                    continue;
                }
            else if(strcmp(s,"Serial.SamplingTime")==0)
                {
                    arg->samplingTime=atoi(p);
                    if(arg->samplingTime<DEFTIME || arg->samplingTime>MAXTIME)
                        arg->samplingTime=DEFTIME;
                    i++;
                    continue;
                }
            else if(strcmp(s,"Serial.Baud")==0)
                {
                    arg->BAUD=atoi(p);
                    if(!(arg->BAUD==9600 || arg->BAUD==38400 || arg->BAUD==57600 || arg->BAUD==115200 ))
                        arg->BAUD=DEFBAUD;
                    i++;
                    continue;
                }
            else if(strcmp(s,"Delta")==0)
                {
                    arg->Delta=atof(p);
                    if(arg->Delta<DELTAMIN || arg->Delta>DELTAMAX)
                        arg->Delta=DELTAMIN;
                    i++;
                    continue;
                }
        }
    p=NULL;
    if(!arg->numbOfDev)
        {
            printf("There is no device number in configFile\nMaybe you gave it bad\n"
                   "Example in config list:numberOfDevice=5;\n"
                   "Or numberDevices=2;\n"
                   "Important to terminate the line with ';'.\n"
                   "Moreover don't use ':' instead of '='.\nThank You!\n ");
            syslog(LOG_ERR,"no device, I am out");
            return -1;
        }
    if(!arg->BAUD)
        arg->BAUD=DEFBAUD;
    if(!arg->Delta)
        arg->Delta=DELTAMIN;
    if(!arg->members)
        arg->members=MEMBERSMIN;
    if(!arg->samplingTime)
        arg->samplingTime=DEFTIME;
    if(!arg->pollTime)
        arg->pollTime=POLLTIME;

    return i;
}



int deviceparameters(char **configbuffer, Threadcommon *arg,int nextLine,int allLine)
{
    int i,k,len,sensorsNumber;
    char difference=1;
    char *s=NULL;
    char *p=NULL;
    i=nextLine;
    k=allLine-nextLine;
    arg->sensors=malloc(arg->numbOfDev*sizeof(Slaves));
    sensorsNumber=0;
    if(!arg->sensors)
        {
            perror("arg->sensors:");
            syslog(LOG_ERR,"No more memory for sensors");
            return -1;
        }
    while(k)
        {
            p=strchr(configbuffer[i],'=');
            p++;
            s=strtok(configbuffer[i],".");
            s=strtok(NULL,".");
            printf("%c",*s);
            if(*s!=difference)
                {
                    arg->sensors[sensorsNumber].address=atoi(p);
                    if(!arg->sensors[sensorsNumber].address)
                        {
                            perror("Address:");
                            syslog(LOG_ERR,"address is 0");
                            return -1;
                        }
                    arg->sensors[sensorsNumber].state=1;
                    difference=*s;
                    sensorsNumber++;
                    i++;
                    k--;
                    continue;
                }
            else if(isalpha(*p))
                {
                    len=strlen(p);
                    arg->sensors[sensorsNumber].names=malloc(len*sizeof(char));
                    if(!arg->sensors[sensorsNumber].names)
                        {
                            perror("arg->sensors[sensorsNumber].name:\n");
                            syslog(LOG_ERR,"no enough memory to allocate for sensors names");
                            return -1;
                        }
                    strcpy(arg->sensors[sensorsNumber].names,p);
                    i++;
                    k--;
                    continue;
                }
            else if(strstr(p,"measuringTime"))
                {
                    arg->sensors[sensorsNumber].time=atoi(p);
                    if(!arg->sensors[sensorsNumber].time)
                        arg->sensors[sensorsNumber].time=10;
                    i++;
                    k--;
                }
            else if(strstr(p,"movingAverage"))
                {
                    arg->sensors[sensorsNumber].time=atoi(p);
                    if(!arg->sensors[sensorsNumber].time)
                        arg->sensors[sensorsNumber].time=3;
                    i++;
                    k--;
                }
        }
    k=0;
    while(k!=arg->numbOfDev)
        {
            printf("Address=%d\tName:%s\tstate:%d\tTime:%d\t\tMovAve_tag_Number=%d\n",arg->sensors[k].address,arg->sensors[k].names,arg->sensors[k].state,arg->sensors[k].time,arg->sensors[k].movAve_tag_number);
            k++;
        }
    printf("Reading config file succesfully\n");
    syslog(LOG_INFO,"Reading config file succesfully");
    return 0;

}

void free_configBuffer(char **configbuffer,int allLine)
{
    int i=0;
    while(allLine)
        {
            free(configbuffer[i]);
            i++;
            allLine--;
        }
}
