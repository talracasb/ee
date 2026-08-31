#include "alloc.h"

#include <stddef.h>
#include <stdint.h>

#define SLOTS 256
#define OPERATIONS 20000

static void *blocks[SLOTS];

static uint32_t seed = 0xABCDEF;

static uint32_t rng(void) {
  seed ^= seed << 13;
  seed ^= seed >> 17;
  seed ^= seed << 5;
  return seed;
}

static size_t random_size(void) { return 8 + (rng() % 4088); }

// Number of live blocks.
static size_t live_count(void) {
  size_t count = 0;

  for (size_t i = 0; i < SLOTS; i++)
    if (blocks[i]) count++;

  return count;
}

// Selects a random live slot.
static size_t random_live_slot(void) {
  size_t count = live_count();

  if (count == 0) return SLOTS;

  size_t target = rng() % count;

  for (size_t i = 0; i < SLOTS; i++) {
    if (blocks[i]) {
      if (target == 0) return i;
      target--;
    }
  }

  return SLOTS;
}

// Selects a random free slot.
static size_t random_free_slot(void) {
  size_t count = SLOTS - live_count();

  if (count == 0) return SLOTS;

  size_t target = rng() % count;

  for (size_t i = 0; i < SLOTS; i++) {
    if (!blocks[i]) {
      if (target == 0) return i;
      target--;
    }
  }

  return SLOTS;
}

void test(void) {
  for (size_t i = 0; i < SLOTS; i++)
    blocks[i] = NULL;

  for (size_t i = 0; i < OPERATIONS; i++) {
    size_t live = live_count();

    /*
     * Roughly 60% allocation / 40% freeing.
     */
    int allocate;

    if (live == 0)
      allocate = 1;
    else if (live == SLOTS)
      allocate = 0;
    else
      allocate = (rng() % 100) < 60;

    if (allocate) {
      size_t slot = random_free_slot();
      size_t size = random_size();

      blocks[slot] = custom_malloc(size);

      if (!blocks[slot]) return;
    } else {
      size_t slot = random_live_slot();

      if (slot != SLOTS) {
        custom_free(blocks[slot]);
        blocks[slot] = NULL;
      }
    }
  }
}