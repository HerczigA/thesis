#include <stdio.h>
#include <stdlib.h>
#include "header/thesis_test.h"
int main()
{
    Threadcommon test;
    printf("This program is made for test my thesis!\n");
    test_counting();
    test_crc();
    test_Init();
    //test_SendPacket(&test);
    test_Reading(&test);
    printf("Test was succesfully done\n");
    return 0;
}
