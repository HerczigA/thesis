#include <iostream>


class sensors
{
    private:
    int address;
    
    public:
    sensors();
    

}
/*Template pattern*/

template <class T>
T myMax(T x, T y)
{
    return (x>y) ? x : y;
}