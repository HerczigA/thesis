#include <stdio.h>
#include <stdlib.h>
#include "header/thesis_test.h"
int main()
{
    printf("This program is made for test my thesis!\n");
    test_config();
   // test_Init();
    test_counting();
    test_crc();

    //test_SendPacket(&test);
    //test_Reading(&test);
    printf("Test was succesfully done\n");
    return 0;
}
