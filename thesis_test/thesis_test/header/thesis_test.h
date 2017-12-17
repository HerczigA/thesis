#ifndef THESIS_TEST_H_INCLUDED
#define THESIS_TEST_H_INCLUDED

#include "/home/herczig/thesis/header/counting.h"
#include "/home/herczig/thesis/header/crc.h"
#include "/home/herczig/thesis/header/Init.h"
#include "/home/herczig/thesis/header/processing.h"
#include "/home/herczig/thesis/header/reading.h"
#include "/home/herczig/thesis/header/config.h"
void test_counting();

void test_crc();

void test_queueInit();

void test_Init();

void test_Reading(void *arg);

void test_SendPacket(void *arg);

#endif // THESIS_TEST_H_INCLUDED
