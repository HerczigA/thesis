#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#define pathOfConfig "./util/config.txt"
#define DEFTIME 10
#define DEFMAXTIME 60
#define POLLTIME 60
#define MAXTIME 10800
#define ZERO 0
#define DEVMIN 0
#define DEVMAX 32
#define DEFBAUD 9600
#define DELTAMAX 50.0
#define DELTAMIN 1.0
#define MEMBERSMIN 3.0
#define MEMBERSMAX 4.0
#define TOPMEASURES 15
#define MAXCHAR 128
#define MAXLINE 100
#include "Init.h"

int Read_config(char **buffer);

int config(Threadcommon *arg);

int Processing_Config(char **configbuffer,int lineNumber,Threadcommon *arg);

int deviceparameters(Threadcommon *arg,char **configbuffer);

#endif // CONFIG_H_INCLUDED
