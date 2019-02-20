#include <iostream>
#include "../header/slaveDevices.h"

using namespace std;

devices::devices(int8_t address)
{
    devices.Packet.address=address;
}

devices::devices()
{
    
}