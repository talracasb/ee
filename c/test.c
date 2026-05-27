#include "alloc.h"

#include <stdint.h>
#include <stddef.h>

#define INITIAL_BLOCKS 120
#define SMALL_BLOCKS   80
#define MEDIUM_BLOCKS  40
#define FINAL_BLOCKS   24

static void *p[512];

static uint32_t seed = 0xCAFEBABE;

static uint32_t rng(void)
{
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
}

static size_t rsize(size_t min, size_t max)
{
    return min + (rng() % (max - min + 1));
}

void test(void)
{
    // ============================================================
    // PHASE 1
    //
    // Create a large uniform heap region.
    //
    // This gives both allocators the same clean baseline.
    // ============================================================

    for (int i = 0; i < INITIAL_BLOCKS; i++) {
        p[i] = custom_malloc(rsize(48, 64));
    }

    // ============================================================
    // PHASE 2
    //
    // Free alternating blocks.
    //
    // Produces many medium-sized holes.
    // ============================================================

    for (int i = 0; i < INITIAL_BLOCKS; i += 2) {
        custom_free(p[i]);
        p[i] = NULL;
    }

    // ============================================================
    // PHASE 3
    //
    // Fill medium holes with smaller allocations.
    //
    // FIRST-FIT:
    //   destroys early holes aggressively.
    //
    // BEST-FIT:
    //   packs tighter into better matches.
    //
    // This phase creates split tails.
    // ============================================================

    for (int i = INITIAL_BLOCKS;
         i < INITIAL_BLOCKS + SMALL_BLOCKS;
         i++)
    {
        p[i] = custom_malloc(rsize(12, 20));
    }

    // ============================================================
    // PHASE 4
    //
    // Randomly free some of the small allocations.
    //
    // Creates tiny scattered holes embedded inside larger
    // fragmented regions.
    // ============================================================

    for (int i = INITIAL_BLOCKS;
         i < INITIAL_BLOCKS + SMALL_BLOCKS;
         i++)
    {
        if ((rng() % 100) < 45) {
            custom_free(p[i]);
            p[i] = NULL;
        }
    }

    // ============================================================
    // PHASE 5
    //
    // Allocate medium blocks which do NOT fit into many
    // fragmented leftovers.
    //
    // Critical difference:
    //
    // FIRST-FIT:
    //   burns through larger surviving regions near front.
    //
    // BEST-FIT:
    //   preserves large regions longer.
    // ============================================================

    for (int i = INITIAL_BLOCKS + SMALL_BLOCKS;
         i < INITIAL_BLOCKS + SMALL_BLOCKS + MEDIUM_BLOCKS;
         i++)
    {
        p[i] = custom_malloc(rsize(26, 40));
    }

    // ============================================================
    // PHASE 6
    //
    // Strategic freeing pattern.
    //
    // Leaves isolated "almost useful" holes everywhere.
    //
    // This is where first-fit really collapses.
    // ============================================================

    for (int i = 1;
         i < INITIAL_BLOCKS + SMALL_BLOCKS + MEDIUM_BLOCKS;
         i += 3)
    {
        if (p[i]) {
            custom_free(p[i]);
            p[i] = NULL;
        }
    }

    // ============================================================
    // PHASE 7
    //
    // Allocate many awkward sizes.
    //
    // These produce maximal split leftovers.
    // ============================================================

    for (int i = 0; i < FINAL_BLOCKS; i++) {
        p[300 + i] = custom_malloc(rsize(17, 29));
    }

    // ============================================================
    // PHASE 8
    //
    // FINAL SHAPING PASS
    //
    // This determines the FINAL HEAP STATE ONLY.
    //
    // We intentionally leave:
    //
    // - stranded tiny holes
    // - fragmented front region
    // - isolated unusable gaps
    // - interleaved live/free blocks
    //
    // FIRST-FIT ends with:
    //   massive checkerboarding near heap front.
    //
    // BEST-FIT ends with:
    //   visibly denser packing and larger contiguous holes.
    // ============================================================

    for (int i = 300; i < 300 + FINAL_BLOCKS; i += 4) {
        custom_free(p[i]);
        p[i] = NULL;
    }

    for (int i = 20; i < 80; i += 5) {
        if (p[i]) {
            custom_free(p[i]);
            p[i] = NULL;
        }
    }

    // intentionally leave dirty heap
}