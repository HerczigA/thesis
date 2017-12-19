#ifndef THESIS_TEST_H_INCLUDED
#define THESIS_TEST_H_INCLUDED

#include "/home/herczig/Dokumentumok/thesis/thesis/header/counting.h"
#include "/home/herczig/Dokumentumok/thesis/thesis/header/crc.h"
#include "/home/herczig/Dokumentumok/thesis/thesis/header/Init.h"
#include "/home/herczig/Dokumentumok/thesis/thesis/header/processing.h"
#include "/home/herczig/Dokumentumok/thesis/thesis/header/reading.h"
#include "/home/herczig/Dokumentumok/thesis/thesis/header/writing.h"
#include "/home/herczig/Dokumentumok/thesis/thesis/header/sending.h"
#include "/home/herczig/Dokumentumok/thesis/thesis/header/config.h"
#include "/home/herczig/Dokumentumok/thesis/thesis/header/closing.h"
#include "/home/herczig/Dokumentumok/thesis/thesis/header/threadHandling.h"

void test_counting();

void test_crc();

void test_config();

void test_Init();

//void test_Reading(void *arg);

//void test_SendPacket(void *arg);

#endif // THESIS_TEST_H_INCLUDED
