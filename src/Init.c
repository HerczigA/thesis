#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "../header/Init.h"
#include "../header/counting.h"
#include "../header/reading.h"
#include "../header/closing.h"
#include "../header/config.h"

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
    term->c_iflag = IGNPAR;
    term->c_lflag &= ~( ICANON | ECHO | ISIG);
    term->c_oflag =0;
    term->c_cc[VTIME]=0;
    term->c_cc[VMIN]=0;
    cfsetispeed(term,(speed_t)init->BAUD);
    cfsetospeed(term,(speed_t)init->BAUD);

    tcflush(init->fd, TCIOFLUSH);
    if(!tcsetattr(init->fd,TCSANOW,term))
        {
            free(term);
            printf("Serial port has succesfully initialized\n");
            syslog(LOG_INFO,"Serial port OK");
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

/**Initialize in-way and out-way queue and mutexes*/
int queueInit(Threadcommon *arg)
{
    if(!(arg && &arg->head))
        return -1;
    arg->loop=true;
    TAILQ_INIT(&arg->head);
    if(!(pthread_mutex_init(&arg->temperature_mutex,NULL) || pthread_mutex_init(&arg->watchdog_mutex,NULL)))
        {
            printf("TAILQ_INIT and pthread_mutex_init is successfully\n");
            syslog(LOG_INFO,"TAILQ_INIT and pthread_mutex_init is successfully");
            return 0;
        }
    else
        {
            printf("Cannot initialize temperature_mutex and-or watchdog_mutex\n");
            syslog(LOG_ERR,"Cannot initialize mutexes");
            return -1;
        }
}

