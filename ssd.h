#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PBA_FREE		-1
#define PBA_INVALID		-2
#define NOFILEPTR		0
#define NOFILE			1
#define INIT_SUCCESS	2

#define PAGE_SIZE		10
#define LINE_SIZE		11
#define LOGICAL_PAGES	100
#define PHYSICAL_PAGES	120 //over-provisioning
#define BUFFER_SIZE		10
#define PAGES_PER_BLOCK	4
#define BLOCK_COUNT		(PHYSICAL_PAGES / PAGES_PER_BLOCK)

#define FULLREAD		20
#define SINGLEREAD		21

typedef struct {
	int is_free;			//1: write 가능, 0: write 불가능
	int is_valid;			//1: 유효한 데이터, 0: stale, 과거 데이터인데 erase되지 않은 것
	int erase_count;
} PageMetaData;

typedef struct {
	int lba;          // 어떤 LBA의 데이터인지
	uint32_t data;    // 실제 데이터
	int is_dirty;     // 1: 데이터 있음, 0: 비어있음
} CacheEntry;

typedef struct {
	unsigned long total_writes;
	unsigned long total_reads;
	unsigned long write_amplification_count; // 실제 NAND 쓰기 횟수
	unsigned long erase_counts[PHYSICAL_PAGES];
	unsigned long bufferCount;
} SMARTStats;

typedef struct {
	int erase_count;    // 지운 횟수
	int valid_count;    // 유효한 페이지 수
	int is_free;        // 현재 비어있는 블록인지
} BlockMeta;

void ssdErase(int lbaNum);
void ssdWrite(int lbaNum, uint32_t data);
uint32_t ssdRead(int lbaNum);
int ssdInit();
int ssdExit();
void ssdFullRead();
void ssdFullWrite(uint32_t data);