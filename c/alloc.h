#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdalign.h>

// Basic alignment information.

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

// Internal information about blocks & the allocator.

struct block_meta {
  char magic[16];
  bool free;
  size_t size;
  size_t used;
  struct block_meta *next;
};

#define META_SIZE ALIGN(sizeof(struct block_meta))
extern struct block_meta *base;

// The basic allocator API. There's more, but it's just
// convenience.

void *custom_malloc(size_t size);
void custom_free(void *ptr);
