#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../header/counting.h"
void test_counting()
{
    movAverage teszt;
    teszt.k=3.0;
    teszt.k_next=12.0;
    teszt.k_prev=1.2;

assert(moving_hysteresis(5.00,-4.00)==1.00);
assert(moving_hysteresis(5.0,1.0)==1.0);
assert(moving_hysteresis(5.00,1.25)==(float)1.25);
assert(moving_hysteresis(5.00,6.69)==(float)1.69);
assert(moving_hysteresis(5.00,-2.69)==(float)2.31);
assert(moving_hysteresis(5.00,-5.00)==(float)0.00);
assert(moving_hysteresis(5.00,-5.01)==(float)-0.01);
assert(mov_average(&teszt,3.0)==(float)5.4);

}
