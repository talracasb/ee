#pragma once
#include <stddef.h>
#include <stdbool.h>

// Internal information about blocks & the allocator.

struct block_meta {
  char magic[16];
  bool free;
  size_t size;
  struct block_meta *next;
};

#define META_SIZE sizeof(struct block_meta)
extern struct block_meta *base;

// The basic allocator API. There's more, but it's just convenience.

void *malloc(size_t size);
void free(void *ptr);
