#ifndef SLAVEDEVICES_H_INCLUDED
#define SLAVEDEVICES_H_INCLUDED
#include <iostream>
#define DATALENGTH 8

#include <stdint.h>

enum sensorTypes
{
    temp_sensor,
    light_sensor,
    PIR_sensor,
    humidity_sensor,
    LCD
    
};

struct packet
{
    int16_t *data;
    int8_t address;
    int32_t crc;
    int8_t length;
    uint8_t cmd;
};

class devices
{
    packet Packet;
    int types;
    int heartBit;
    int addressCnt=0;
    void frameInit(int16_t *data,int8_t length);
    void frameAssembly();

    public:
    devices(int8_t address);
    devices();
    

};

class sensors : public devices
{
    uint8_t readFromLine();
    
    public:
    void read();
};

class actuators : public devices
{

    int sendOnLine();
};
/*Template pattern*/

template <class T>
T myMax(T x, T y)
{
    return (x>y) ? x : y;
}

template <class T, class U >
class A 
{
    T x;
    U y;
    
    public:
   A(T x, U y)
    {
        this.x=x;
        this.y=y;
        std::cout<<"constructor is called"<<std::endl;
    }
    A();
} 

/*templates end*/

#endif 


