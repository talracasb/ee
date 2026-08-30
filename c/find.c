#include <sys/types.h>
#include "alloc.h"
#include <stdio.h>
#include "find.h"

enum Strategy strategy = FIRST;

// Function which actually finds a free block.
static struct block_meta *first(size_t size) {
  struct block_meta *current = base;

  while (current) {
    // First-fit allocation. It goes through each block
    // meta, and then returns when it finds a block that's
    // big enough.
    if (current->free && current->size >= size) return current;
    current = current->next;
  }

  return NULL;
}

static struct block_meta *best(size_t size) {
  struct block_meta *current = base;
  struct block_meta *best = NULL;

  while (current) {
    if (current->free && current->size >= size) {
      if (best == NULL || current->size < best->size) best = current;
    }

    current = current->next;
  }

  return best;
}

static struct block_meta *worst(size_t size) {
  struct block_meta *current = base;
  struct block_meta *worst = NULL;

  while (current) {
    if (current->free && current->size >= size) {
      if (worst == NULL || current->size > worst->size) worst = current;
    }

    current = current->next;
  }

  return worst;
}

struct block_meta *next_cursor = NULL;
static struct block_meta *next(size_t size) {
  if (base == NULL) return NULL;
  if (next_cursor == NULL) next_cursor = base;

  struct block_meta *start = next_cursor;
  struct block_meta *current = next_cursor;

  do {
    if (current->free && current->size >= size) {
      next_cursor = current->next ? current->next : base;
      return current;
    }

    current = current->next ? current->next : base;
  } while (current != start);

  return NULL;
}

struct block_meta *find(size_t size) {
  struct block_meta *block;
  switch (strategy) {
  case FIRST:
    block = first(size);
    break;
  case BEST:
    block = best(size);
    break;
  case WORST:
    block = worst(size);
    break;
  case NEXT:
    block = next(size);
    break;
  }

  return block;
}