#ifndef INPUTS_H_INCLUDED
#define INPUTS_H_INCLUDED
#include <iostream>


typedef enum inputs
{
    temp_sensor,
    light_sensor,
    PIR_sensor,
    humidity_sensor
}Sensortypes;

class sensors
{
    int address;
    int types;

    
    public:
    sensors();
    

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


