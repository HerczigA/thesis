It is just an ordinary thesis topic. The topic is called about RS-485 communication via Motorola protocoll on an IoT.
Hardware what I used is a RPI 2B and sowtware is a minibian.
Hope, it will be a good project, and everybody could use it for them responsibility.

CHECK the util dir!!!

Features:
-rsyslog.conf:
    You can set where to syslog the errors and datas under working
-config.txt:
    This file includes the important datas for the init and for the working.It can not work without this of course. 
Important to check the config.txt path in header/header.h. You have to give the correct path in header.h
Without number of device, the system shall not check the names, addresses, states etc.
The proper usage e.g:

device.1.address=1
device.1.name=kitchen
device.1.measuringTime=20
device.1.movingAverage=3

where measuringTime value is given in sec.After the first point the value symbolises the given address at the parameter part.

There is no limit to names. If you mistake or use float number to times or mov_average just for fun,
it will be convert to int.

-logrotate.conf:
    You can set how freqvently rotate the syslogs and how many days of logs compress

content of rsyslog.conf and logrotate.conf have to copy to the proper place BEFORE you use the app! 
Import to set the proper path in makefile for the target app!

For makefile you just have to use make in root dir. It will compile automatically. 
Test it RPI 2B, Linux Debian Stretch and Jessie.

If you have any question don't hesitate to ask me.
email: herczig.adam@gmail.com


