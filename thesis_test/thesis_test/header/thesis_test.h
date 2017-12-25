#ifndef THESIS_TEST_H_INCLUDED
#define THESIS_TEST_H_INCLUDED

#include "../../../header/counting.h"
#include "../../../header/crc.h"
#include "../../../header/Init.h"
#include "../../../header/processing.h"
#include "../../../header/reading.h"
#include "../../../header/writing.h"
#include "../../../header/sending.h"
#include "../../../header/config.h"
#include "../../../header/closing.h"
#include "../../../header/threadHandling.h"

void test_counting();

void test_crc();

void test_config();

void test_Init();

void test_Reading();

//void test_SendPacket(void *arg);

#endif // THESIS_TEST_H_INCLUDED
