#include "gen_test.h"
#include "ssd.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 

#define LBA_MAX 100

static uint32_t rand_u32(void) {
    return ((uint32_t)rand() << 16) ^ (uint32_t)rand();
}

static int rand_lba(void) {
    return rand() % LBA_MAX;
}

int gen_test_scenario(const char* path, int line, unsigned seed) {
    FILE* f = fopen(path, "w");
    if (!f) return 0;

    srand(seed);

    for (int i = 0; i < line; i++) {
        int cmd = rand() % 5;

        switch (cmd) {
        case 0: {
            int addr = rand_lba();
            uint32_t data = rand_u32();
            ssdWrite(addr, data);
            fprintf(f, "write %d 0x%08X\n", addr, data);
            break;
        }
        case 1: {
            int addr = rand_lba();
            ssdRead(addr);
            fprintf(f, "read %d\n", addr);
            break;
        }
        case 2: {
            uint32_t data = rand_u32();
            ssdFullWrite(data);
            fprintf(f, "fullwrite 0x%08X\n", data);
            break;
        }
        case 3:
            ssdFullRead();
            fprintf(f, "fullread\n");
            break;
        case 4:
            fprintf(f, "help\n");
            break;
        }
    }

    fprintf(f, "exit\n");
    fclose(f);
    return 1;
}
