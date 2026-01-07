#include <stdint.h>
#include <stdlib.h>
#include <time.h>

uint32_t make_random_address()
{
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }

    uint32_t value = 0;

    value |= ((uint32_t)(rand() & 0xFF)) << 24;
    value |= ((uint32_t)(rand() & 0xFF)) << 16;
    value |= ((uint32_t)(rand() & 0xFF)) << 8;
    value |= ((uint32_t)(rand() & 0xFF));

    return value;
}
