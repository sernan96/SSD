#include "ssd.h"

static FILE* g_nand_f = NULL, * g_result_f = NULL;
static uint32_t DRAM[10];
static PageMetaData metaTable[PHYSICAL_PAGES];
static CacheEntry writeBuffer[BUFFER_SIZE];
static SMARTStats stats;

void ssdWrite(int lbaNum, uint32_t data) {
    stats.total_writes++;

    //buffer에 추가하는 경우
    /*
    if (stats.bufferCount < BUFFER_SIZE) {
        for (int i = 0; i < BUFFER_SIZE; i++) {
            if (writeBuffer[i].is_dirty == 1 && )
        }
    }*/

    if (g_nand_f == NULL) {
        if (ssdInit() == NOFILE) return;
    }

    int targetLine = lbaNum;
    long offset = (lbaNum) * (LINE_SIZE);             // PAGE의 주소 * LINE의 길이

    char checkBuf[LINE_SIZE];
    fseek(g_nand_f, offset, SEEK_SET);
    fread(checkBuf, 1, 10, g_nand_f);               // 값 부분 10바이트만 읽기
    fseek(g_nand_f, offset, SEEK_SET);

    uint32_t currentVal = (uint32_t)strtoul(checkBuf, NULL, 16);
    
    // overwrite 방지
    // 추가 개선 사항: STATE table 등 추가
    if (currentVal != 0x00000000) {
        printf("[ERROR] Overwrite: LBA %d is not empty (0x%08X).\n", lbaNum, currentVal);
        return;
    }

    fprintf(g_nand_f, "0x%08X", data);
    fflush(g_nand_f);

    return;
}

uint32_t ssdRead(int lbaNum, int isFull) {
    if (g_nand_f == NULL) {
        if (ssdInit() == NOFILE) return;
    }
    
    switch (isFull) {
    case FULLREAD:
        g_result_f = fopen("result.txt", "w");
        break;
    case SINGLEREAD:
        g_result_f = fopen("result.txt", "rb+");
    }

    char line[LINE_SIZE];
    long offset = lbaNum * (LINE_SIZE);

    fseek(g_nand_f, offset, SEEK_SET);

    if (fread(line, 1, 10, g_nand_f) != 10) return 0;   //읽기 실패

    uint32_t output = (uint32_t)strtoul(line, NULL, 16); // 16진수로 변환

    if (g_result_f != NULL) {
        fprintf(g_result_f, "0x%08X", output);
    }

    return output;
}

void ssdErase(int lbaNum) {
    int start = lbaNum * PAGE_SIZE;
    int end = start + PAGE_SIZE;

    if (start < 0 || start >= LOGICAL_PAGES) {
        fprintf(stderr, "EraseError occured: invalid idx %d\n", lbaNum);
        return EXIT_FAILURE;
    }

    if (g_nand_f == NULL) {
        perror("we don't have file: nand.txt\n");
        return EXIT_FAILURE;
    }

    for (int i = start; i < end; i++)
    {
        if (fseek(g_nand_f, i * LINE_SIZE, SEEK_SET) != 0)
        {
            perror("Erase fseek Failed");
            fclose(g_nand_f);
            return EXIT_FAILURE;
        }
        fprintf(g_nand_f, "0x%08X\n", 0x00000000);
    }
    return EXIT_SUCCESS;
}


int ssdInit() {
    if (g_nand_f != NULL) {
        fclose(g_nand_f);
        g_nand_f = NULL;
    }

    //파일을 읽기&쓰기, binary 모드로 열기: window/linux에서 둘 다 동작하도록
    g_nand_f = fopen("nand.txt", "rb+");

    if (g_nand_f == NULL) {
        printf("[ERROR] NOFILE\n");
        return NOFILE;
    }

    //nand 파일 초기화
    for (int i = 0; i < LOGICAL_PAGES; i++) {
        fprintf(g_nand_f, "0x00000000\n"); 
    }
    return INIT_SUCCESS;
}

int ssdExit() {
    if (g_nand_f == NULL || g_result_f == NULL) {
        return NOFILEPTR;
    }
    fclose(g_nand_f);
    fclose(g_result_f);
    g_nand_f = NULL;
    g_result_f = NULL;
}

void ssdFullWrite(uint32_t data) {
    for (int i = 0;  i < LOGICAL_PAGES; i++) {
        ssdWrite(i, data);
    }
}

void ssdFullRead() {
    for (int i = 0; i < LOGICAL_PAGES; i++) {
        ssdRead(i, FULLREAD);
    }
}