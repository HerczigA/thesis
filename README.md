It is just an ordinary thesis topic. The topic is called about RS-485 communication via Motorola protocoll on an IoT.
Hardware what I used is a RPI 2B and sowtware is a Minibian.
Hope, it will be a good project, and everybody could use it for them responsibility.

CHECK the util dir!!!

Features:
-rsyslog.conf:
    You can set where to syslog the errors and datas under working
-config.txt:
    This file includes the important datas for the init and for the working.It can not work without this of course. 
-logrotate.conf:
    You can set how freqvently rotate the syslogs and how many days of logs compress

************************************************************************************************************

For developing I used Codeblocks IDE. In Codeblocks I used "./util/config.txt" for test the program. However for the real test
I used my RPi where I had to modify the relative path to "../util/config.txt" to can use my device. Now both of them is used
in config.c. Config.c wait for pathOfConfig from config.h.

Important to check the config.txt path in header/config.h. You have to give the correct path in config.h if you remove config.txt 
from util dir. Of course important the privilage too.

Without number of device, the system shall not check the names, addresses, states etc. just exit with -1. 

There is an ethanol config.txt in util, feel free to use it.

The proper usage of the device parameters e.g:

device.1.address=1
device.1.name=kitchen
device.1.measuringTime=20
device.1.movingAverage=3

where measuringTime value is given in sec.After the first point to the value symbolises the address of the given device.

There is no limit to names. If you mistake or use float number to times or mov_average just for fun,
it will be convert to int.

*************************************************************************************************************


Content of rsyslog.conf and logrotate.conf have to copy to the proper place BEFORE you use the app! 
logrotate.conf -> /etc/logrotate.d/
rsyslog.conf -> /etc/rsyslog.d/

For makefile you just have to use make in the project root dir. It will compile automatically. 
Test it RPI 2B, Linux Debian Stretch and Jessie.

If you have any question don't hesitate to ask me.
email: herczig.adam@gmail.com


