/*!

Source from Zidarics Zoltan a.k.a Zamek
\version $Id: crc.c v 1.4 2009-05-08 06:16:09 zamek Exp $

*/
#include <stdio.h>
#include <stdlib.h>
#include "../header/reading.h"
#include "../header/crc.h"

uint16_t addCRC(uint16_t packetCrc, unsigned char countedCrc)
{
    char bit;
    packetCrc^= ( countedCrc<< (WIDTH-BYTE));
    for (bit=BYTE; bit >0; --bit)
        if (packetCrc& TOPBIT)
            packetCrc= (packetCrc<< 1) ^ POLYNOMIAL;
        else
            packetCrc<<= 1;
    return packetCrc;
}

int compareCRC(uint16_t crc1, uint16_t crc2)
{
    return crc1==crc2 ? 1:0;
}
/*
#include <msp430g2553.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define TX BIT2
#define RX BIT1
#define TXLED BIT0
#define RXLED BIT6
#define POLYNOMIAL 0xd8
#define BYTE 8
#define WIDTH (BYTE * sizeof(int))
#define TOPBIT (1 << (WIDTH-1))
#define TX BIT2
#define RX BIT1
#define TXLED BIT0
#define RXLED BIT6
#define PING 0x69
#define TERM 0x01
#define LIMIT 2
#define TRUE 1
#define FALSE 0
typedef enum PacketState
{
    //! Default condition
    EmptyState,

    // min. one 0x55 received
    moto55,

    //! after 0xFF, 0x01 received
    moto1,

    // Address
    address,

    //! Command
    command,

    //! Low byte of datalength
    DLenLow,

    //! High byte of datalength
    DLenHigh,

    //! Databyte
    Data,

    //! low byte of crc  Packet
    CrcLow,

    //! high byte of crc  Packet
    CrcHigh

} packetState;

typedef struct FIFO
{
  char data;
  struct FIFO *next;
} List;

List *first,*act,*temp;
char myAddress=1;
char i=0;
char heartbit=0x69;
char data[2]={0x01,0x00};
int dLen=2;

int reading(List *act,unsigned char myAddress);
int addCRC(int packetCrc, unsigned char countedCrc);
int compareCRC(int crc1, int crc2);
void sending(unsigned char address, unsigned char cmd,char *data, int dLen);
void setup() {
  // put your setup code here, to run once:
    DCOCTL=0;
    BCSCTL1=CALBC1_1MHZ;
    DCOCTL=CALDCO_1MHZ;

    /****************************

    P1DIR |= RXLED+TXLED;
    P1OUT &= 0x00;
    P2DIR |= 0xFF;
    P2OUT &= 0x00;

    P1DIR |= BIT0;
    P1SEL |= RX+TX ;   // P1.1 UCA0RXD input
    P1SEL2 |=  RX +TX;  // P1.2 UCA0TXD output
    P1DIR |= RXLED + TXLED;
    UCA0CTL0 &= 0x00;
    UCA0CTL1 |= UCSSEL_2;
    UCA0BR0 = 0x6D;   //9600-> 109|d=0x6D BAUD
    UCA0BR1 = 0x00;
    UCA0MCTL=UCBRS0+UCA0BR1 ;
    UCA0CTL1 &= ~UCSWRST;
    IE2 |= UCA0RXIE;    //UC0IE
    first=act=temp=NULL;
}

void loop() {
  // put your main code here, to run repeatedly:
   WDTCTL=WDTPW + WDTHOLD;   //Watchdog STOP
    /*Setting inner clock to 1MHz*

    if(first)
    {
      if(reading(first,myAddress)==2)
      {

      }
     while(first)
     {
      i++;
      first=first->next;
     }

    }


}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
 // __bic_SR_register_on_exit (LPM3_bits);
  //_BIC_SR(LPM3_EXIT);
  //P1OUT |=BIT0;
  List *newlist=(List*)malloc(sizeof(List));
  newlist->data=UCA0RXBUF;
  newlist->next=NULL;
  if(first)
     act->next=newlist;
  else
    first=newlist;
  act=newlist;
  //P1OUT &= ~BIT0;
}




int reading(List *act,unsigned char myAddress){
    if(!act)
      return -1;

    List *temp;
    int i=0;
    char cmd=0;
    char *reqData=NULL;
    unsigned int dataIndex;
    unsigned int len=0,crc,calculateCrc;
    calculateCrc=crc=0;
    int LOOP=TRUE;
    packetState State=EmptyState;
  //  char P1OUT;
    while(LOOP)
        {

            switch (State)
                {
                case EmptyState:

                    if (act->data== 0x55)
                        {

                            State= moto55;
                            i++;
                        }

                    temp=act;
                    act=temp->next;
                    free(temp);
                    continue;

                case moto55:
                    if (act->data== 0x55)
                        {

                            if(i==5)
                            {

                                temp=act;
                                act=temp->next;
                                free(temp);
                                break;
                            }
                            else
                            {

                              i++;
                              temp=act;
                              act=temp->next;
                              free(temp);
                              continue;
                            }

                        }
                    if (act->data== 0xFF)
                        {
                            P1OUT |=BIT0+BIT6;
                            State=moto1;
                            temp=act;
                            act=temp->next;
                            free(temp);
                            continue;
                        }
                    else
                        {

                            temp=act;
                            act=temp->next;
                            free(temp);
                            break;
                        }

                case moto1:
                    if(act->data==0x01)
                        {

                            calculateCrc=0;
                            State= address;
                            temp=act;
                            act=temp->next;
                            free(temp);
                            continue;
                        }
                    else
                    {
                        temp=act;
                        act=temp->next;
                        free(temp);
                        break;
                    }
                case address:
                    if(act->data==myAddress)
                    {
                      calculateCrc = addCRC(calculateCrc, act->data);
                      State = command;
                      temp=act;
                      act=temp->next;
                      free(temp);
                      continue;
                    }
                    else
                    {
                      State=EmptyState;
                      LOOP=FALSE;
                     // while(act)
                     // {
                      temp=act;
                      act=temp->next;
                      free(temp);
                     // }
                      break;
                    }

                case command :
                    calculateCrc = addCRC(calculateCrc,act->data);
                    cmd=act->data;
                    State = DLenLow;
                    temp=act;
                    act=temp->next;
                    free(temp);
                    continue;

                case DLenLow :
                    calculateCrc = addCRC(calculateCrc, act->data);
                    len=act->data & 0xFF;
                    State = DLenHigh;
                    temp=act;
                    act=temp->next;
                    free(temp);
                    continue;

                case DLenHigh :
                    calculateCrc = addCRC(calculateCrc, act->data);
                    len |= (act->data& 0xff) << BYTE ;
                    /*temp=act;
                    act=temp->next;
                    free(temp);*
                    dataIndex=0;
                    if (len> 0)
                        {
                            if (len <= LIMIT)
                                {
                                    reqData =(char*)malloc((len)*sizeof(char));
                                    if(!reqData)
                                      {
                                        while(act)
                                       {
                                          temp=act;
                                          act=temp->next;
                                          free(temp);
                                          P1OUT = BIT0+BIT6;
                                       }
                                            break;
                                      }
                                    State = Data;
                                    temp=act;
                                    act=temp->next;
                                    free(temp);
                                    continue;
                                }
                            else
                            {
                              LOOP=FALSE;
                              while(act)
                                {
                                  temp=act;
                                  act=temp->next;
                                  free(temp);
                                }
                                break;
                            }

                        }
                    else
                        {
                            State =  CrcLow;
                            temp=act;
                            act=temp->next;
                            free(temp);
                            continue;
                        }
                case Data :
                    calculateCrc = addCRC(calculateCrc, act->data);
                    *((reqData)+dataIndex) = act->data;
                    if(++dataIndex>=len)
                        State = CrcLow;
                     temp=act;
                     act=temp->next;
                     free(temp);
                     continue;

                case CrcLow :
                    crc = (act->data & 0xff);
                    State = CrcHigh;
                    temp=act;
                    act=temp->next;
                    free(temp);
                    continue;

                case CrcHigh:
                    crc |= ( act->data & 0xff)<< BYTE;
                    if (compareCRC(crc, calculateCrc))
                        {
                            if(cmd==TERM && *reqData)           //cmdTerm =1, not polling
                                {
                                  while(act->next)
                                    {
                                      temp=act;
                                      act=temp->next;
                                      free(temp);
                                    }
                                   free(reqData);
                                    return 1;
                                }
                            else if (cmd==PING)
                               {
                                while(act->next)
                                {
                                  temp=act;
                                  act=temp->next;
                                  free(temp);
                                }
                                return 2;
                               }

                        }
                    if(reqData)
                      {
                        free(reqData);
                        LOOP=FALSE;
                        break;
                      }

                }

        }
        if(reqData)
          free(reqData);

        return 0;
}

int addCRC(int packetCrc, unsigned char countedCrc)
{
    char bits;
    packetCrc^= ( countedCrc<< (WIDTH-BYTE));
    for (bits=BYTE; bits >0; --bits)
        if (packetCrc& TOPBIT)
            packetCrc= (packetCrc<< 1) ^ POLYNOMIAL;
        else
            packetCrc<<= 1;
    return packetCrc;
}

int compareCRC(int crc1, int crc2)
{
    return crc1==crc2 ? 1:0;
}

void sending(unsigned char address, unsigned char cmd,char *data, int  dLen)
{
    if ( !data || dLen <=0 )
            {
              P1OUT |= BIT0 + BIT6;
              return;
              }

    char *buff=(char*)malloc((dLen+13)*sizeof(char));
    if(!buff)
    {
      P1OUT |= BIT0 + BIT6;
      delay(10);
      P1OUT &=0x00;
      delay(10);
      P1OUT |= BIT0 + BIT6;
      delay(10);
      return;
    }

    int i=0;
    int dataElement=11;
    unsigned int crc=0;
    unsigned char len1,len2,crc1,crc2;
    crc = addCRC(crc, address);
    crc = addCRC(crc, cmd);
    len1= dLen & 0xff;
    crc = addCRC(crc,len1);
    len2 = (dLen >> BYTE) & 0xff;
    crc = addCRC(crc, len2);
    if(dLen>0)
        {
            int j;
            for (j=0; j<dLen; j++,data++)
                {
                    buff[dataElement]=*data;
                    crc = addCRC(crc, *data);
                    dataElement++;
                }
        }
    crc1=crc & 0xff;
    crc2=(crc>>BYTE) & 0xff;
    while(i!=5)
        {
            buff[i]=0x55;
            i++;
        }
    buff[5]=0xFF;
    buff[6]=0x01;
    buff[7]=address;
    buff[8]=cmd;
    buff[9]=len1;
    buff[10]=len2;
    buff[dataElement]=crc1;
    dataElement++;
    buff[dataElement]=crc2;
    //dataElement++;??
    i=0;
    while(i<dataElement)
    {
      P1OUT |=BIT0;
      UCA0TXBUF=buff[i];
      i++;
      P1OUT &= ~BIT0;
    }
    free(buff);
}


*/
