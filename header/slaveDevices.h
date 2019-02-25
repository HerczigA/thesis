#ifndef SLAVEDEVICES_H_INCLUDED
#define SLAVEDEVICES_H_INCLUDED

#include <iostream>
#include <stdint.h>

#define DATALENGTH 8
#define POLYNOMIAL 0xd8
#define BYTE 8
#define WIDTH (BYTE * sizeof(uint16_t))
#define TOPBIT (1 << (WIDTH-1))



int addressCnt=0;

enum sensorTypes
{
    temp_sensor=0,
    light_sensor,
    PIR_sensor,
    humidity_sensor
    
};

enum actuatorTypes
{
    transciever=0,
    servoMotor,
    motor,
    LCD,
    relay

};

struct packet
{
    int16_t data[BYTE];
    int8_t address;
    int32_t crc;
    int8_t length;
    uint8_t cmd;
};

class devices
{
    private:
        
        int heartBit;
        void frameInit(int16_t *data,int8_t length);
        void frameAssembly();
        
    protected:
        packet Packet;
        int types;
        int address;
        void setType(int type);

    public:
        //devices(int8_t address,int type);
        //devices(int type);
    

};

class sensors : public devices
{
    uint8_t readFromLine();
    
    public:
    sensors(int8_t address,int type);
    sensors(int type);
    void read();
};

class actuators : public devices
{
    int writeLine(int *data, int cmd, int fd);
    public:
    actuators(int8_t address,int type);
    actuators(int type);
    int send(int *data,int cmd);
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


