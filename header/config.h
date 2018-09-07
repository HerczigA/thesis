#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <ctype.h>
#include "Init.h"

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
//#define MAKE 1

#ifdef MAKE


#define pathOfConfig "../util/config.txt"     //FOR MAKE

#else

#define pathOfConfig "./util/config.txt"            //FOR CODEBLOCKS

#endif // MAKE

int config(Threadcommon *arg);

void signalcatch(int sig);

#endif // CONFIG_H_INCLUDED
