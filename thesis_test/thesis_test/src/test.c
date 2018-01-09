#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../header/thesis_test.h"
#include "../header/counting.h"
#include "../header/crc.h"
#include "../header/Init.h"
#include "../header/processing.h"
#include "../header/reading.h"
#include "../header/writing.h"
#include "../header/sending.h"
#include "../header/config.h"
#include "../header/closing.h"
#include "../header/threadHandling.h"
#define MAXLINE 100

/** Must not test mov_average with 3 and 4 members in one time! */

void test_counting()
{
   Threadcommon arg;
   float delta=5.0;
    movAverage *tesztmov;
    arg.numbOfDev=2;
    arg.sensors=malloc(2*sizeof(Slaves));
    tesztmov=malloc(2*sizeof(movAverage));
    tesztmov[0].k_element=malloc(3*sizeof(float));
    tesztmov[1].k_element=malloc(3*sizeof(float));

    arg.sensors[0].address=1;
    arg.sensors[0].movAve_tag_number=3;
    arg.sensors[0].names="pince";
    arg.sensors[0].time=20;
    arg.sensors[0].state=1;
    arg.sensors[0].watchdog=3;
    arg.sensors[1].address=2;
    arg.sensors[1].movAve_tag_number=3;
    arg.sensors[1].names="szoba";
    arg.sensors[1].time=30;
    arg.sensors[1].state=1;
    arg.sensors[1].watchdog=3;
    tesztmov[0].k_element[0]=10.0;
    tesztmov[0].k_element[1]=8.0;
    tesztmov[0].k_element[2]=24.0;
    tesztmov[0].act_min_val=1.0;
    tesztmov[0].act_max_value=tesztmov[0].act_min_val+delta;

    tesztmov[1].k_element[0]=5;
    tesztmov[1].k_element[1]=8;
    tesztmov[1].k_element[2]=11;
    tesztmov[1].act_min_val=1.0;
    tesztmov[1].act_max_value=tesztmov[1].act_min_val+delta;
    /**With 3 moving_tag*/
    assert(mov_average(&tesztmov[0],&arg,0)==(float)14.0);
    assert(mov_average(&tesztmov[1],&arg,1)==(float)8.0);
    tesztmov[0].k_element[0]=5.0;
    assert(mov_average(&tesztmov[0],&arg,0)<=(float)7.67);
    tesztmov[0].k_element[0]=2.0;
    assert(mov_average(&tesztmov[0],&arg,0)<=(float)5.67);
    tesztmov[1].k_element[0]=12.0;
    assert(mov_average(&tesztmov[1],&arg,1)<=(float)8.34);
    tesztmov[1].k_element[0]=32.0;
    assert(mov_average(&tesztmov[1],&arg,1)<=(float)16.34);
    /**End with 3 moving_tag*/

    /** Start with 4 tag*/
    arg.sensors[0].movAve_tag_number=4;
    arg.sensors[1].movAve_tag_number=4;
    tesztmov[0].k_element=realloc(tesztmov[0].k_element,4*sizeof(float));
    tesztmov[1].k_element=realloc(tesztmov[1].k_element,4*sizeof(float));
    tesztmov[0].k_element[0]=10.0;
    tesztmov[0].k_element[1]=8.0;
    tesztmov[0].k_element[2]=24.0;
    tesztmov[0].k_element[3]=16.0;
    tesztmov[0].summary=0;
    tesztmov[1].k_element[0]=5;
    tesztmov[1].k_element[1]=8;
    tesztmov[1].k_element[2]=11;
    tesztmov[1].k_element[3]=14;
    tesztmov[1].summary=0;
    assert(mov_average(&tesztmov[0],&arg,0)<=(float)7.76);  //result=7.75
    assert(mov_average(&tesztmov[1],&arg,0)<=(float)4.75);  //result=4.75
    tesztmov[0].k_element[0]=13.0;                             //summary=14.5
    tesztmov[1].k_element[0]=7.64;                             //summary=9.5
    assert(mov_average(&tesztmov[0],&arg,0)==(float)14.125);  //result=14.125
    tesztmov[0].measuredValue=14.125;
    assert(mov_average(&tesztmov[1],&arg,0)==(float)8.705);  //result=8.705
    tesztmov[0].measuredValue=8.705;
    /**EOF mov_average test*/

    /**Start of mov_hysteresis*/
    assert(moving_hysteresis(delta,&tesztmov[0])<=(float)9.125);
    assert(moving_hysteresis(delta,&tesztmov[1])<=(float)3.705);
    assert(moving_hysteresis(delta,&tesztmov[1])>=(float)-2.505);

    free(tesztmov[0].k_element);
    free(tesztmov[1].k_element);
    free(tesztmov);
    free(arg.sensors);
}

void test_crc()
{
    uint16_t crc1,crc2;
    crc1=32541;
    crc2=32543;
    assert(compareCRC(crc1,crc1)==1);
    assert(compareCRC(crc2,crc1)==0);
}



void test_Init()
{
    Threadcommon test,*testp=NULL;
    struct termios old,new_term;

    assert(queueInit(testp)==-1);
    assert(queueInit(&test)==0);
    assert(InitSerialPort(NULL,NULL,NULL)==-1);
    assert(InitSerialPort(NULL,NULL,&test)==-1);
    assert(InitSerialPort(NULL,&new_term,NULL)==-1);
    assert(InitSerialPort(&old,NULL,NULL)==-1);
    assert(InitSerialPort(NULL,&new_term,&test)==-1);
    assert(InitSerialPort(&old,NULL,&test)==0);
    assert(InitSerialPort(&old,&new_term,&test)==0);
    assert(InitSerialPort(&old,NULL,&test)==0);
}

void test_config()
{
    Threadcommon test;
    test.sensors=NULL;
    char *buffer[100];
    assert(config(NULL)==-1);
    assert(config(&test)==0);
    assert(Read_config(NULL)==-1);
    assert(Read_config(buffer)==24);            //configfile.txt line number
    assert(Processing_Config(NULL,NULL)==-1);
    printf("fasza\n");
    assert(Processing_Config(buffer,NULL)==-1);
    printf("fasza\n");
    assert(Processing_Config(NULL,&test)==-1);
    assert(Processing_Config(buffer,&test)==5);   //First 5 line

    assert(deviceparameters(buffer,&test,5,24)==0);
    //assert(deviceparameters(buffer,&test,5,24)==-1);    //if there are same addresses in config.txt

}

/*
void test_SendPacket(void *arg)
{
    Threadcommon *testp;
    testp=arg;
    unsigned char taddress,tcmd,tdata,tdLen;
    Crc crc1,crc2;
     crc1=0;
     crc2=0;
    taddress=1;
    tcmd=0;
    tdata=1;
    tdLen=1;

    assert(sendPacket(testp->fd,taddress,tcmd,&tdata,tdLen)==1);
    assert(sendPacket(0,0,0,NULL,0)==-1);
    assert(sendPacket(testp->fd,0,0,NULL,0)==-1);
    assert(sendPacket(0,0,0,&tdata,0)==1);
    tdata=0;
    assert(sendPacket(7,1,1,&tdata,0)==1);
    assert(sendPacket(&test.fd,address,tcmd,&tdata,tDlen)==1);
    test.fd=-1;
    assert(sendPacket(&test.fd,0,0,NULL,0)==-1);


}*/

