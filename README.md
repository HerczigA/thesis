It is just an ordinary thesis topic. The topic is called about RS-485 communication via Motorola protocoll.
Hardware what I used is a RPI 2B.
Hope, it will be a good project, and everybody could use it for them responsibility

CHECK the util dir!!!

Features:
-rsyslog.conf:
    You can set where to syslog the errors and datas under working
-config.txt:
    This file include the important datas for the init and for the working.It can work without this of course.
-logrotate.conf:
    You can set how freqvently rotate the syslogs and how many days of logs compress

content of rsyslog.conf and logrotate.conf have to copy to the proper place BEFORE you use the app! 

Important to check the config.txt path. You have to give the correct path in header.h
#define pathOfConfig