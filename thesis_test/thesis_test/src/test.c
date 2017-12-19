#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
/*#include "/home/herczig/Dokumentumok/thesis/thesis/header/config.h"
#include "/home/herczig/Dokumentumok/thesis/thesis/header/counting.h"
#include "/home/herczig/Dokumentumok/thesis/thesis/header/crc.h"
#include "/home/herczig/Dokumentumok/thesis/thesis/header/Init.h"
#include "/home/herczig/Dokumentumok/thesis/thesis/header/processing.h"
#include "/home/herczig/Dokumentumok/thesis/thesis/header/reading.h"*/
#include "../header/thesis_test.h"
#define MAXLINE 100

/** Must not test mov_average with 3 and 4 members in one time! */

void test_counting()
{
    //float delta=5.00;
    movAverage tesztmov[2];
    Threadcommon teszt[2];
    //teszt.sensors[5];
    teszt->sensors[0].address=1;
    teszt->sensors[0].movAve_tag_number=3;



    /*teszt.sensors[0].address=1;
    teszt.sensors[0].movAve_tag_number=3;
    teszt.sensors[1].address=2;
    teszt.sensors[1].movAve_tag_number=3;
    teszt.sensors[2].address=3;
    teszt.sensors[2].movAve_tag_number=3;
    teszt.sensors[3].address=4;
    teszt.sensors[3].movAve_tag_number=3;
    teszt.sensors[4].address=5;
    teszt.sensors[4].movAve_tag_number=3;*/
    //tesztmov[0].k_element[teszt->sensors[0].movAve_tag_number];

    tesztmov[0].k_element[0]=10;
    tesztmov[0].k_element[1]=8;
    tesztmov[0].k_element[2]=24;

    assert(mov_average(&tesztmov[0],teszt,0)!=(float)0.0);
  /*  assert(mov_average(&tesztmov,&teszt,3.0)==(float)9.0);
    assert(mov_average(&tesztmov,&teszt,3.0)==(float)12.0);
    assert(mov_average(&tesztmov,&teszt,4.0)==(float)3.6875);
    assert(mov_average(&tesztmov,&teszt,4.0)==(float)8.725);







    assert(moving_hysteresis(delta,&tesztmov)==1.00);
    assert(moving_hysteresis(delta,&tesztmov)==1.0);
    assert(moving_hysteresis(delta,&tesztmov)==(float)1.25);
    assert(moving_hysteresis(delta,&tesztmov)==(float)1.69);
    assert(moving_hysteresis(delta,&tesztmov)==(float)2.31);
    assert(moving_hysteresis(delta,&tesztmov)==(float)0.00);
    assert(moving_hysteresis(delta,&tesztmov)==(float)-1.01000023);*/
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
    assert(Processing_Config(buffer,NULL)==-1);
    assert(Processing_Config(NULL,&test)==-1);
    assert(Processing_Config(buffer,&test)==5);   //First 5 line
    assert(deviceparameters(buffer,&test,5,24)==0);
    //assert(deviceparameters(buffer,&test,5,24)==-1);    //if there are same addresses in config.txt

}
/*void test_Reading(void *arg)
{
    char test_data=0x01;
    Threadcommon *testp=arg;
    readingFromSerial(testp);
    readingFromSerial(NULL);
    assert(reserve(test_data)!=NULL);
}

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

