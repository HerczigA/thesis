#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED

#include "counting.h"
#include "crc.h"
#include "header.h"
#include "measuring.h"
#include "reading.h"

void test_counting();
void test_crc();
void test_queueInit();
void test_Init();
void test_Reading();
void test_SendPacket(void *arg);
#endif // TEST_H_INCLUDED
