#include <iostream>
#include "../header/slaveDevices.h"

using namespace std;

sensors::sensors(int8_t address)
{
    sensors.Packet.address=address;
}

sensors::sensors()
{
    
}