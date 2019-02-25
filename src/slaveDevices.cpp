#include <iostream>
#include "../header/slaveDevices.h"

using namespace std;

/*devices::devices(int8_t address,int type)
{
    devices.Packet.address=address;
    setType(type);
    catch(char message);
}

devices::devices(int type)
{
    devices.Packet.address=addressCnt++;
}
*/


void devices::setType(int type)
{
        switch(type)
        {
            case temp_sensor:
                sensors.types=type;
                break;

            case light_sensor:
                sensors.types=type;
                break;

            case PIR_sensor:
                sensors.types=type;
                break;

            case humidity_sensor:    
                sensors.types=type;
                break;
            
            default:
                throw "Wrong typecode";
                //break;

        }
    
}

sensors::sensors(int8_t address,int type)
{
    sensors.Packet.address=address;
    try
    {
        setType(type);
    }
    catch(char message)
    {
        //syslog
        cout<<"bad type"<<endl;
    }
}

sensors::sensors(int type)
{
    sensors.Packet.address=addressCnt++;
    try
    {
        setType(type);
    }
    catch(char message)
    {
        //syslog
        cout<<"bad type"<<endl;
    }
}

actuators::actuators(int8_t address,int type)
{
    actuators.Packet.address=address;
    try
    {
        setType(type);
    }
    catch(char message)
    {
        //syslog
        cout<<"bad type"<<endl;
    }
}

actuators::actuators(int type)
{
    actuators.Packet.address=addressCnt++;
    try
    {
        setType(type);
    }
    catch(char message)
    {
        //syslog
        cout<<"bad type"<<endl;
    }
}

int actuators::writeLine(int *data, int cmd,int fd)
{
    uint8_t result;

    return result
}

int actuators::send(int *data,int cmd,int fd)
{
    if(!data || sizeof(data)>BYTE || fd<0)
        return 1;
    int result=writeLine(data,cmd);
    return result;

}



