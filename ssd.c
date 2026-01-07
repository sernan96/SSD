#include "ssd.h"

static FILE* g_nand_f = NULL, * g_result_f = NULL;

void ssdWrite(int lbaNum, uint32_t data) {
    if (g_nand_f == NULL) {
        if (ssdInit() == NOFILE) return;
    }

    int targetLine = lbaNum;
    long offset = (lbaNum) * (LINE_SIZE);             // PAGE�� �ּ� * LINE�� ����

    char checkBuf[LINE_SIZE];
    fseek(g_nand_f, offset, SEEK_SET);
    fread(checkBuf, 1, 10, g_nand_f);               // �� �κ� 10����Ʈ�� �б�
    fseek(g_nand_f, offset, SEEK_SET);

    uint32_t currentVal = (uint32_t)strtoul(checkBuf, NULL, 16);
    
    // overwrite ����
    // �߰� ���� ����: STATE table �� �߰�
    if (currentVal != 0x00000000) {
        printf("[ERROR] Overwrite: LBA %d is not empty (0x%08X).\n", lbaNum, currentVal);
        return;
    }

    fprintf(g_nand_f, "0x%08X", data);
    fflush(g_nand_f);

    return;
}

uint32_t ssdRead(int lbaNum) {
    if (g_nand_f == NULL || g_result_f == NULL) {
        if (ssdInit() == NOFILE) return;
    }

    char line[LINE_SIZE];
    long offset = lbaNum * (LINE_SIZE);

    fseek(g_nand_f, offset, SEEK_SET);

    if (fread(line, 1, 10, g_nand_f) != 10) return 0;   //�б� ����

    uint32_t output = (uint32_t)strtoul(line, NULL, 16); // 16������ ��ȯ

    if (g_result_f != NULL) {
        fprintf(g_result_f, "0x%08X", output);
        fclose(g_result_f);
    }

    return output;
}

void ssdErase(int lbaNum) {
    int start = lbaNum * PAGES_PER_BLOCK;
    int end = start + PAGES_PER_BLOCK;

    if (start < 0 || start >= MAX_ADDRESS) {
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
        fprintf(g_nand_f, "0x%08X\n", 0xFFFFFFFFu);
    }
    return EXIT_SUCCESS;
}

int ssdInit() {
    if (g_nand_f != NULL) {
        fclose(g_nand_f);
        g_nand_f = NULL;
    }
    if (g_result_f != NULL) {
        fclose(g_result_f);
        g_result_f = NULL;
    }

    //������ �б�&����, binary ���� ����: window/linux���� �� �� �����ϵ���
    g_nand_f = fopen("nand.txt", "w+b");
    g_result_f = fopen("result.txt", "w+b");

    if (g_nand_f == NULL || g_result_f == NULL) {
        printf("[ERROR] NOFILE\n");
        return NOFILE;
    }

    //nand ���� �ʱ�ȭ
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
    g_nand_f = NULL;
    g_result_f = NULL;
}