#include "ssd.h"

static FILE* g_nand_f = NULL;
static FILE* g_result_f = NULL;
static BlockMeta blockMetaTable[BLOCK_COUNT];
static int g_current_pba_index = 0;
static int LtoP[LOGICAL_PAGES];
static int PtoL[PHYSICAL_PAGES];

void nand_raw_write(int pba, uint32_t data);
uint32_t nand_raw_read(int pba);
int get_next_free_pba();
int ftl_internal_write(int lba, uint32_t data);
void do_garbage_collection();
void ssdEraseBlock(int blockIdx);

void nand_raw_write(int pba, uint32_t data) {
    long offset = (long)pba * LINE_SIZE;
    fseek(g_nand_f, offset, SEEK_SET);
    fprintf(g_nand_f, "0x%08X", data);
}

uint32_t nand_raw_read(int pba) {
    long offset = (long)pba * LINE_SIZE;
    char buf[LINE_SIZE];
    fseek(g_nand_f, offset, SEEK_SET);
    if (fread(buf, 1, 10, g_nand_f) != 10) return 0;
    return (uint32_t)strtoul(buf, NULL, 16);
}

void ssdEraseBlock(int blockIdx) {
    int startPba = blockIdx * PAGES_PER_BLOCK;
    int endPba = startPba + PAGES_PER_BLOCK;
    for (int pba = startPba; pba < endPba; pba++) {
        nand_raw_write(pba, 0x00000000);
        PtoL[pba] = PBA_FREE;
    }
    blockMetaTable[blockIdx].erase_count++;
    blockMetaTable[blockIdx].valid_count = 0;
}

int get_next_free_pba() {
    int start = g_current_pba_index;
    do {
        int pba = g_current_pba_index;
        g_current_pba_index = (g_current_pba_index + 1) % PHYSICAL_PAGES;
        if (PtoL[pba] == PBA_FREE) return pba;
    } while (g_current_pba_index != start);
    return -1;
}

int ftl_internal_write(int lba, uint32_t data) {
    int pba = get_next_free_pba();
    if (pba == -1) return -1;

    nand_raw_write(pba, data);

    int old_pba = LtoP[lba];
    if (old_pba != -1) {
        PtoL[old_pba] = PBA_INVALID;
        int blk_idx = old_pba / PAGES_PER_BLOCK;
        if (blockMetaTable[blk_idx].valid_count > 0)
            blockMetaTable[blk_idx].valid_count--;
    }

    LtoP[lba] = pba;
    PtoL[pba] = lba;

    int new_blk_idx = pba / PAGES_PER_BLOCK;
    blockMetaTable[new_blk_idx].valid_count++;

    return 0;
}

void do_garbage_collection() {
    int victim_blk = -1;
    int min_valid = PAGES_PER_BLOCK + 1;
    int active_blk = g_current_pba_index / PAGES_PER_BLOCK;

    for (int i = 0; i < BLOCK_COUNT; i++) {
        if (i == active_blk) continue;
        if (blockMetaTable[i].valid_count < min_valid) {
            min_valid = blockMetaTable[i].valid_count;
            victim_blk = i;
        }
    }

    if (victim_blk == -1) return;

    int startPba = victim_blk * PAGES_PER_BLOCK;
    for (int i = 0; i < PAGES_PER_BLOCK; i++) {
        int old_pba = startPba + i;
        int lba = PtoL[old_pba];

        if (lba != -1 && LtoP[lba] == old_pba) {
            uint32_t data = nand_raw_read(old_pba);
            if (ftl_internal_write(lba, data) == -1) {
                printf("[CRITICAL] GC Failed: No Free Space for Migration\n");
                return;
            }
        }
    }
    ssdEraseBlock(victim_blk);
}

void ssdWrite(int lbaNum, uint32_t data) {
    if (g_nand_f == NULL) ssdInit();
    if (ftl_internal_write(lbaNum, data) == -1) {
        do_garbage_collection();

        if (ftl_internal_write(lbaNum, data) == -1) {
            printf("[ERROR] Write Failed: SSD Full\n");
            return;
        }
    }
    fflush(g_nand_f);
}

uint32_t ssdRead(int lbaNum, int isFull) {
    if (g_nand_f == NULL) ssdInit();

    int pba = LtoP[lbaNum];
    uint32_t output = 0;

    if (pba != -1) output = nand_raw_read(pba);

    const char* mode = (isFull == FULLREAD) ? "a" : "w";
    g_result_f = fopen("result.txt", mode);
    if (g_result_f) {
        fprintf(g_result_f, "0x%08X\n", output);
        fclose(g_result_f);
        g_result_f = NULL;
    }
    return output;
}

int ssdInit() {
    if (g_nand_f != NULL) {
        fclose(g_nand_f);
        g_nand_f = NULL;
    }

    g_nand_f = fopen("nand.txt", "wb+");
    for (int i = 0; i < PHYSICAL_PAGES; i++) {
        fprintf(g_nand_f, "0x00000000\n");
    }
    if (g_nand_f == NULL) return NOFILE;

    return INIT_SUCCESS;
}

int ssdExit() {
    if (g_nand_f == NULL) return 0;

    fclose(g_nand_f);
    if (g_result_f) fclose(g_result_f);

    g_nand_f = NULL;
    g_result_f = NULL;
    return 0;
}

void ssdFullWrite(uint32_t data) {
    for (int i = 0; i < LOGICAL_PAGES; i++) {
        ssdWrite(i, data);
    }
}

void ssdFullRead() {
    FILE* fp = fopen("result.txt", "w");
    if (fp) fclose(fp);

    for (int i = 0; i < LOGICAL_PAGES; i++) {
        ssdRead(i, FULLREAD);
    }
}