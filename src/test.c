#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../header/counting.h"
#include "../header/crc.h"
#include "../header/Init.h"
#include "../header/processing.h"
#include "../header/reading.h"

/** Must not test mov_average with 3 and 4 members in one time! */

void test_counting()
{
    float delta=5.00;
    movAverage teszt;

/*    teszt.k=3.0;
    teszt.k_next=12.0;
    teszt.k_prev=1.2;
    teszt.k_fourth=13.3;
    teszt.summary=0.0;
*/
  /*  assert(moving_hysteresis(delta,-4.00)==1.00);
    assert(moving_hysteresis(delta,1.0)==1.0);
    assert(moving_hysteresis(delta,1.25)==(float)1.25);
    assert(moving_hysteresis(delta,6.69)==(float)1.69);
    assert(moving_hysteresis(delta,-2.69)==(float)2.31);
    assert(moving_hysteresis(delta,-5.00)==(float)0.00);
    assert(moving_hysteresis(delta,-6.01)==(float)-1.01000023);
    assert(mov_average(&teszt,3.0)==(float)5.4);
    assert(mov_average(&teszt,3.0)==(float)9.0);
    assert(mov_average(&teszt,3.0)==(float)12.0);
    /*assert(mov_average(&teszt,4.0)==(float)3.6875);
    assert(mov_average(&teszt,4.0)==(float)8.725);
    */
}

void test_crc()
{
    //int calcrc=20;
    assert(addCRC(16,2)==4528);


    assert(compareCRC(2,2)==1);
    assert(compareCRC(2,1)==0);
}



void test_Init()
{
    char *testbuffer[MAXLINE];
    Threadcommon test,*testp=NULL;
    struct termios old, *testterm=NULL;
    assert(ReadConfig(testp)==-1);
    assert(configlist(NULL,testp)==-1);
    assert(configlist(NULL,&test)==-1);
    assert(configlist(testbuffer,&test)==0);
    assert(queueInit(testp)==-1);
    assert(queueInit(&test)==0);
    assert(InitSerialPort(NULL,NULL,NULL)==-1);
    assert(InitSerialPort(NULL,NULL,testp)==-1);
    assert(InitSerialPort(&old,testterm,&test)==0);

}

void test_Reading(void *arg)
{
    char test_data=0x01;
    Threadcommon *testp=arg;
    readingFromSerial(testp);
    readingFromSerial(NULL);
    assert(reserve(test_data)!=NULL);


}
/*void test_SendPacket(void *arg)
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

