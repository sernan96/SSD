#include "ssd.h"
static FILE* g_nand_f = NULL, * g_result_f = NULL;

//overwrite 불가능하게 추가 구현 필요
void ssdWrite(int lbaNum, uint32_t data) {
    if (g_nand_f == NULL) {
        if (init() == NOFILE) return;
    }

    int targetLine = lbaNum;
    long offset = (lbaNum) * PAGE_SIZE;             // PAGE의 주소 * PAGE의 길이
    fseek(g_nand_f, offset, SEEK_SET);              // 이동

    fprintf(g_nand_f, "0x%08X\n", data);

    return;
}

uint32_t ssdRead(int lbaNum) {
    if (g_nand_f == NULL || g_result_f == NULL) {
        if (init() == NOFILE) return;
    }

    char line[LINE_SIZE]; //읽어온 값
    int targetLine = lbaNum;
    long offset = (lbaNum) * PAGE_SIZE;             // PAGE의 주소 * PAGE의 길이
    fseek(g_nand_f, offset, SEEK_SET);              // 이동

    uint32_t output;

    fread(line, sizeof(char), LINE_SIZE, g_nand_f);

    //spare txt -> 유효성 검사?

    output = strtol(line, NULL, LINE_SIZE);         //진법 변환
    fprintf(g_result_f, "0x%08X", output);

    return output;
}


int ssdInit() {
    if (g_nand_f != NULL) {
        fclose(g_nand_f);
    }
    if (g_result_f != NULL) {
        fclose(g_result_f);
    }

    //파일을 읽기&쓰기, binary 모드로 열기: window/linux에서 둘 다 동작하도록
    g_nand_f = fopen("nand.txt", "r+b"); 
    g_result_f = fopen("result.txt", "r+b");

    if (g_nand_f == NULL || g_result_f == NULL) {
        perror("NO SUCH FILE: nand.txt\n");
        return NOFILE;
    }

    //nand 파일 초기화
    for (int i = 0; i < 100; i++) {
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
}