#include "alloc.h"

void *p[80];

void test(void)
{
    // Phase 1: full dense packing (establish contiguous baseline)
    p[0]  = custom_malloc(24);
    p[1]  = custom_malloc(24);
    p[2]  = custom_malloc(24);
    p[3]  = custom_malloc(24);
    p[4]  = custom_malloc(24);
    p[5]  = custom_malloc(24);
    p[6]  = custom_malloc(24);
    p[7]  = custom_malloc(24);

    // Phase 2: carve small holes everywhere (critical for first-fit degradation)
    // custom_free(p[1]);
    // custom_free(p[3]);
    // custom_free(p[5]);
    // custom_free(p[7]);

    // p[8]  = custom_malloc(8);
    // p[9]  = custom_malloc(8);
    // p[10] = custom_malloc(8);
    // p[11] = custom_malloc(8);

    // // Phase 3: interleave live blocks so coalescing is blocked
    // custom_free(p[0]);
    // custom_free(p[2]);

    // p[12] = custom_malloc(20);   // forces skip over small holes
    // p[13] = custom_malloc(18);

    // // Phase 4: fragmentation explosion (alternate free/live pattern)
    // custom_free(p[4]);
    // custom_free(p[6]);

    // p[14] = custom_malloc(10);
    // p[15] = custom_malloc(14);
    // p[16] = custom_malloc(6);

    // custom_free(p[8]);
    // custom_free(p[10]);

    // p[17] = custom_malloc(22);
    // p[18] = custom_malloc(12);

    // // Phase 5: create many “almost usable” holes (worst for first-fit)
    // custom_free(p[9]);
    // custom_free(p[11]);
    // custom_free(p[12]);

    // p[19] = custom_malloc(7);
    // p[20] = custom_malloc(9);
    // p[21] = custom_malloc(11);
    // p[22] = custom_malloc(13);

    // // Phase 6: prevent consolidation by keeping separators alive
    // p[23] = custom_malloc(6);
    // p[24] = custom_malloc(6);
    // p[25] = custom_malloc(6);

    // custom_free(p[13]);
    // custom_free(p[14]);

    // p[26] = custom_malloc(16);
    // p[27] = custom_malloc(18);

    // // Phase 7: create long fragmented prefix (classic first-fit failure mode)
    // custom_free(p[15]);
    // custom_free(p[16]);
    // custom_free(p[17]);

    // p[28] = custom_malloc(8);
    // p[29] = custom_malloc(8);
    // p[30] = custom_malloc(8);

    // custom_free(p[18]);
    // custom_free(p[19]);

    // p[31] = custom_malloc(20);
    // p[32] = custom_malloc(10);

    // // Phase 8: repeated “skip-heavy” allocations (forces linear scan waste)
    // custom_free(p[20]);
    // custom_free(p[21]);

    // p[33] = custom_malloc(19);
    // p[34] = custom_malloc(17);
    // p[35] = custom_malloc(15);

    // custom_free(p[22]);
    // custom_free(p[23]);

    // p[36] = custom_malloc(21);
    // p[37] = custom_malloc(9);

    // // Phase 9: final fragmentation lock-in (no global coalescing allowed)
    // custom_free(p[24]);
    // custom_free(p[25]);
    // custom_free(p[26]);

    // p[38] = custom_malloc(7);
    // p[39] = custom_malloc(6);
    // p[40] = custom_malloc(5);

    // custom_free(p[27]);
    // custom_free(p[28]);

    // p[41] = custom_malloc(14);
    // p[42] = custom_malloc(13);

    // custom_free(p[29]);
    // custom_free(p[30]);

    // p[43] = custom_malloc(12);
    // p[44] = custom_malloc(11);

    // // Phase 10: deliberate pathological ending state
    // custom_free(p[31]);
    // custom_free(p[32]);
    // custom_free(p[33]);

    // p[45] = custom_malloc(18);
    // p[46] = custom_malloc(16);

    // custom_free(p[34]);
    // custom_free(p[35]);

    // p[47] = custom_malloc(10);
    // p[48] = custom_malloc(8);

    // custom_free(p[36]);
    // custom_free(p[37]);

    // p[49] = custom_malloc(9);
    // p[50] = custom_malloc(7);

    // // leave scattered small holes + interspersed live blocks
    // // intentionally NOT cleaned up
}