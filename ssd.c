#include "ssd.h"

static FILE* g_nand_f = NULL;

void ssdWrite(int lbaNum, uint32_t data) {
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

uint32_t ssdRead(int lbaNum) {
    if (g_nand_f == NULL) {
        if (ssdInit() == NOFILE) return;
    }

    char line[LINE_SIZE];
    long offset = lbaNum * (LINE_SIZE);

    fseek(g_nand_f, offset, SEEK_SET);

    if (fread(line, 1, 10, g_nand_f) != 10) return 0;   //읽기 실패

    uint32_t output = (uint32_t)strtoul(line, NULL, 16); // 16진수로 변환

    // result.txt에 결과 기록
    FILE* res_fp = fopen("result.txt", "w");
    if (res_fp == NULL) { printf("[ERROR] NOFILE: result.txt\n"); }

    if (res_fp != NULL) {
        fprintf(res_fp, "0x%08X", output);
        fclose(res_fp);
    }

    return output;
}


int ssdInit() {
    if (g_nand_f != NULL) {
        fclose(g_nand_f);
        g_nand_f = NULL;
    }

    //파일을 읽기&쓰기, binary 모드로 열기: window/linux에서 둘 다 동작하도록
    g_nand_f = fopen("nand.txt", "w+b"); 

    if (g_nand_f == NULL) {
        printf("[ERROR] NOFILE: nand.txt\n");
        return NOFILE;
    }

    //nand 파일 초기화
    for (int i = 0; i < 100; i++) {
        fprintf(g_nand_f, "0x00000000\n"); 
    }
    return INIT_SUCCESS;
}

int ssdExit() {
    if (g_nand_f == NULL) {
        return NOFILEPTR;
    }
    fclose(g_nand_f);
}