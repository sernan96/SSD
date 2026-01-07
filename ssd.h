#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NOFILEPTR	 0
#define NOFILE		 1
#define INIT_SUCCESS 2

#define LINE_SIZE	 10
#define PAGE_SIZE	 11 //0x00000000ÀÇ 10ÀÚ¸® + \n

void ssdWrite(int lbaNum, uint32_t data);
uint32_t ssdRead(int lbaNum);
int ssdInit();
int ssdExit();