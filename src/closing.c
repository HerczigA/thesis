#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include <termios.h>
#include "../header/reading.h"
#include <pthread.h>

void setBackTermios(Threadcommon *fileconf,struct termios *old)
{
    if(!(old&&fileconf))
    {
        syslog(LOG_ERR,"there is a NULL in argumentum list.");
        return;
    }
    int i=0;
    tcflush(fileconf->fd,TCIOFLUSH);
    tcsetattr(fileconf->fd,TCSANOW,old);
    while(i<fileconf->numbOfDev)
    {
        free(fileconf->sensors[i].names);
        i++;
    }
    pthread_mutex_destroy(&fileconf->mutex);
    free(fileconf->sensors);
    printf("All allocated memory is free and mutex destroyed succesfully."
           "Setting back is succesfully done.\n "
           "Thank you for use this device, hope you enjoy it!\n");
    syslog(LOG_NOTICE,"Setting back is succesfully done");
    close(fileconf->fd);
    closelog();
}

void closeOnFAIL(void *arg)
{
    Threadcommon *temp=arg;
    int i=0;
    while(i<temp->numbOfDev)
    {
        free(temp->sensors[i].names);
        i++;
    }
    free(temp->sensors);
    printf("Program finished, because filedescriptor is wrong"
           " or there are some problems\n"
           "when setting serial port\n"
           "Used memory is free\n");
    syslog(LOG_ERR,"Close on Fail successfully.");
}
