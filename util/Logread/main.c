#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#define LOGPATH "/var/log/thesis/statistic"
#define MAX 512

void sigcatch(int signal);

volatile static bool loop=true;

int main(int argc,char **argv)
{

char *line=NULL;
FILE *f=NULL;
struct sigaction sig;
sig.sa_handler=sigcatch;
sig.sa_restorer=NULL;
sig.sa_flags=0;
sigaction(SIGINT,&sig,NULL);
if(argc==1)
    {
	f=fopen(LOGPATH,"r");
	    if(!f)
	    {
		perror("no loginfo\n");
		return -1;
	    }
	
    }
else if(argc==2)
    {
	if(strstr(argv[argc-1],"error"))
	{
	    f=fopen("/var/log/thesis/error","r");
	    if(!f)
	    {
		perror("/var/log/thesis/error:");
		return -1;
	    }
	}
	else
	{
		printf("wrong logfile name\n");
		return -1;
	}
    }

    line=(char*)malloc(MAX*sizeof(char));
    if(!line)
    {
	printf("no memory to alloc line array\n");
	return -1;
    }
    while(loop)
    {
	if(!fgets(line,MAX,f))
	    continue;
	else
	    printf("%s\n",line);
    
    }
    free(line);
    fclose(f);
    return 0;
}


void sigcatch(int signal)
{
    printf("signal received I'm out\n");
    loop=false;
}