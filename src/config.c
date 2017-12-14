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
    int i;
    char *buffer[MAXLINE];
    arg->BAUD=ZERO;
    arg->Delta=ZERO;
    arg->members=ZERO;
    arg->numbOfDev=ZERO;
    arg->samplingTime=ZERO;
    arg->pollTime=ZERO;

    i=Read_config(buffer);
    if(i==-1)
        return i;

    return Processing_Config(buffer,i,arg) ? 1:0;

}
int Read_config(char **buffer)
{
    char *temp=NULL;
    FILE *fconfig=NULL;
    int i=0,j;
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
            if(!temp)
                {
                    printf("no memory for temp\n");
                    fclose(fconfig);
                    return -1;
                }
            while(fgets(temp,MAXCHAR,fconfig))
                {
                    if(strchr(temp,';'))
                        {
                            buffer[i]=(char*)malloc((strlen(temp)+1)*sizeof(char));     //????
                            if(!buffer[i])
                                {
                                    if(i)
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
                            else
                                {
                                    strcpy(buffer[i],temp);
                                    i++;
                                }
                        }
                }
            i--;
            fclose(fconfig);
            free(temp);
            return i;
        }
}


int Processing_Config(char **configbuffer,int lineNumber,Threadcommon *arg)
{

    if(!configbuffer)
        return -1;
    int i,len;

    const char equalsign='=';
    char *p=NULL;
    i=0;
    /**First of all, check the array of pointers by the parameters. Checking by '='.*/
    while((p=strchr(configbuffer[i],equalsign)))
        {
            len=strlen(p)-1;
            p++;
            if(strstr(configbuffer[i],"Polling")) //buffereket nem elfelejteni törölni!!!!
                {
                    if((p[len]='\n'))
                        p[len-2]='\0';
                    else
                        p[len-1]='\0';
                    arg->pollTime=atoi(p);
                    if(arg->pollTime<POLLTIME || arg->pollTime>MAXTIME || !arg->pollTime)
                        arg->pollTime=POLLTIME;
                    i++;
                    continue;
                }
            if(strstr(configbuffer[i],"Device"))
                {
                    if((p[len]='\n'))
                        p[len-2]='\0';
                    else
                        p[len-1]='\0';
                    arg->numbOfDev=atoi(p);
                    if(arg->numbOfDev<DEVMIN|| arg->numbOfDev>DEVMAX|| !arg->numbOfDev)
                        arg->numbOfDev=DEVMIN;
                    i++;
                    continue;
                }
            if(strstr(configbuffer[i],"Sampling"))
                {
                    if((p[len]='\n'))
                        p[len-2]='\0';
                    else
                        p[len-1]='\0';
                    arg->samplingTime=atoi(p);
                    if(arg->samplingTime<DEFTIME|| arg->samplingTime>MAXTIME|| !arg->samplingTime)
                        arg->samplingTime=DEFTIME;
                    i++;
                    continue;
                }
            if(strstr(configbuffer[i],"Baud"))
                {
                    if((p[len]='\n'))
                        p[len-2]='\0';
                    else
                        p[len-1]='\0';
                    arg->BAUD=atoi(p);
                    if(!(arg->BAUD==9600 || arg->BAUD==38400 || arg->BAUD==57600 || arg->BAUD==115200 ))
                        arg->BAUD=DEFBAUD;
                    i++;
                    continue;
                }
            if(strstr(configbuffer[i],"Delta"))
                {
                    if((p[len]='\n'))
                        p[len-2]='\0';
                    else
                        p[len-1]='\0';
                    arg->Delta=atof(p);
                    if(arg->Delta<DELTAMIN || arg->Delta>DELTAMAX|| !arg->Delta)
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
                   "Important to end the line with ';'.\n"
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

    if(deviceparameters(arg,configbuffer))
        return -1;

    i=0;
    while(i<=lineNumber)
        {
            free(configbuffer[i]);
            i++;
        }

    return 0;
}



int deviceparameters(Threadcommon *arg,char **configbuffer)
{
    int i,k,len,address,sensorsNumber;
    int j=arg->numbOfDev;
    char *seged=NULL;
    char *p=NULL;
    const char tab='\t';

    arg->sensors=malloc(arg->numbOfDev*sizeof(Slaves));
    sensorsNumber=0;
    if(!arg->sensors)
        {
            syslog(LOG_ERR,"No more memory for sensors");
            return -1;
        }
    while(j)
        {
            if((p=strrchr(configbuffer[i],tab)))
                {
                    //First the tagnumbers of devices
                    p++;
                    seged=configbuffer[i];
                    len=strlen(p)-1;
                    if(p[len]=='\n')
                        {
                            p[len-1]='\0';
                            arg->sensors[sensorsNumber].movAve_tag_number=atoi(p);
                            if(!arg->sensors[sensorsNumber].movAve_tag_number || arg->sensors[sensorsNumber].movAve_tag_number==1 || arg->sensors[sensorsNumber].movAve_tag_number==2)
                                arg->sensors[sensorsNumber].movAve_tag_number=3;
                        }
                    else if(p[len]==';')
                        {
                            p[len]='\0';
                            arg->sensors[sensorsNumber].movAve_tag_number=atoi(p);
                            if(!arg->sensors[sensorsNumber].movAve_tag_number || arg->sensors[sensorsNumber].movAve_tag_number==1 || arg->sensors[sensorsNumber].movAve_tag_number==2)
                                arg->sensors[sensorsNumber].movAve_tag_number=3;
                        }

                    //second the names of devices
                    while(!(isalpha(*seged)))
                        seged++;
                    p=seged;
                    while(isalpha(*p))
                        p++;
                    *p='\0';
                    arg->sensors[sensorsNumber].names=malloc((strlen(seged)+1)*sizeof(char));
                    if(!arg->sensors[sensorsNumber].names)
                        {
                            printf("No more memory for allocating names of device.\n");
                            syslog(LOG_ERR,"Cannot allocate memory");
                            return -2;
                        }
                    strcpy(arg->sensors[sensorsNumber].names,seged);
                    while(!(isdigit(*p)))
                        p++;

                    //third the states
                    arg->sensors[sensorsNumber].state=atoi(p);
                    p++;
                    while(!(isdigit(*p)))
                        p++;

                    //fourth for time
                    arg->sensors[sensorsNumber].time=atoi(p);
                    p=configbuffer[i];
                    while(isdigit(*p))
                        p++;
                    *p='\0';
                    address=atoi(configbuffer[i]);
                    //last the addresses
                    arg->sensors[sensorsNumber].address=(char)address;
                    j--;
                    k=0;
                    //compare the addresses
                    while(k<sensorsNumber)
                        {
                            if(arg->sensors[k].address==arg->sensors[sensorsNumber].address)
                                {
                                    arg->sensors[k].state=0;
                                    arg->sensors[sensorsNumber].state=0;
                                    printf("There is an address conflict occured.\n"
                                           "The problems are in the following devices:\n%s\t\t%s\n\n",
                                           arg->sensors[k].names,arg->sensors[sensorsNumber].names);
                                }
                            k++;
                        }
                    sensorsNumber++;
                }
            i++;
        }
    j=0;
    while(j!=arg->numbOfDev)
        {
            printf("Address=%d\tName:%s\tstate:%d\tTime:%d\t\tMovAve_tag_Number=%d\n",arg->sensors[j].address,arg->sensors[j].names,arg->sensors[j].state,arg->sensors[j].time,arg->sensors[j].movAve_tag_number);
            j++;
        }
    printf("Reading config file succesfully\n");
    syslog(LOG_INFO,"Reading config file succesfully");
    return 0;

}
