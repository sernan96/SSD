#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NOFILEPTR	 0
#define NOFILE		 1
#define INIT_SUCCESS 2

#define PAGE_SIZE	    10
#define LINE_SIZE	    11
#define PAGES_PER_BLOCK 10
#define MAX_ADDRESS     100

void ssdErase(int lbaNum);
void ssdWrite(int lbaNum, uint32_t data);
uint32_t ssdRead(int lbaNum);
int ssdInit();
int ssdExit();