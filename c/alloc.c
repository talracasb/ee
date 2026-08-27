#include "alloc.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct block_meta *base = NULL;
size_t heap_usage = 0;
size_t used_memory = 0;
enum Strategy strategy = FIRST;

// Function which actually finds a free block.
static struct block_meta *first_find(size_t size) {
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

static struct block_meta *best_find(size_t size) {
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

static struct block_meta *find(size_t size) {
  struct block_meta *block;
  if (strategy == FIRST) {
    block = first_find(size);
  } else {
    block = best_find(size);
  }

  return block;
}

// Log current allocator state as CSV.
static void log_memory(void) {
  printf("%zu,%zu\n", used_memory, heap_usage);
  fflush(stdout);
}

// Request a new block by extending the heap.
static struct block_meta *request(struct block_meta *previous, size_t size) {
  struct block_meta *block = sbrk(0);
  size_t total = size + META_SIZE;

  // Requests the block.
  void *request = sbrk(total);

  // Makes sure that the block allocated correctly.
  // This is done by ensuring that the address returned by
  // the second `sbrk` call is the same as the previous
  // program break.
  assert((void *)block == request); // Not thread safe.
  if (request == (void *)-1) {
    return NULL; // sbrk failed.
  }

  heap_usage += total;
  // If there was a previous block, then ensure it's
  // correctly added to the linked list.
  if (previous) previous->next = block;

  // Populate the block metadata.
  block->size = size;
  block->next = NULL;
  block->free = false;
  strcpy(block->magic, "requested");

  return block;
}

// Gets the pointer to the start of the block_meta with the
// pointer to its data.
static struct block_meta *get_block_ptr(void *ptr) {
  // Pointer arithmetic in C is based on the type, so this
  // goes back one unit of block_meta.
  return (struct block_meta *)ptr - 1;
}

// Little helper to find the previous block.
// In a real allocator, there would be a pointer attached to
// each block for the sake of performance.
static struct block_meta *find_previous(struct block_meta *block) {
  struct block_meta *current = base;
  if (!current || current == block) return NULL;

  while (current && current->next != block) {
    current = current->next;
  }

  return current;
}

// Split the block by cutting it down to a certain size,
// and then creating a new block right afterwards.
static void split(struct block_meta *block, size_t size) {
  size = ALIGN(size);

  // If it can't fit both a new block meta, the data it
  // already has, and some alignment then don't split it.
  if (block->size < size + META_SIZE + ALIGNMENT) return;

  // Basically, set the new block to be after both the
  // current block's metadata and size.
  struct block_meta *new = (struct block_meta *)((char *)(block + 1) + size);

  new->size = block->size - size - META_SIZE;
  new->free = true;
  new->next = block->next;
  strcpy(new->magic, "split");

  block->size = size;
  block->next = new;
}

void *custom_malloc(size_t size) {
  if (size == 0) return NULL;
  size = ALIGN(size);

  // First call, we need to allocate a block at the base.
  if (!base) {
    base = request(NULL, size);
    if (!base) return NULL;

    base->used = size;
    used_memory += size;
    log_memory();

    return base + 1;
  }

  struct block_meta *block = find(size);
  struct block_meta *prev = find_previous(block);
  if (!prev) prev = base;

  // Find a new block.
  if (!block) {
    block = request(prev, size);
    if (!block) return NULL;
  } else {
    // Try splitting off the found block to save it's excess space.
    split(block, size);
    block->free = false;
    strcpy(block->magic, "found");
  }

  block->used = size;
  used_memory += size;
  log_memory();

  return block + 1;
}

// Coalesces surrounding blocks if possible and returns the
// new, hopefully bigger, block.
static void coalesce(struct block_meta *block) {
  if (!block) return;

  bool merged = true;
  while (merged) {
    merged = 0;

    // Merge with next block if free
    if (block->next && block->next->free) {
      struct block_meta *next = block->next;

      block->size += META_SIZE + next->size;
      block->next = next->next;
      strcpy(block->magic, "merged_next");
      merged = true;
    }

    // Merge with previous block if free
    struct block_meta *prev = find_previous(block);
    if (prev && prev->free) {
      prev->size += META_SIZE + block->size;
      prev->next = block->next;
      strcpy(prev->magic, "merged_prev");
      block = prev;
      merged = true;
    }
  }
}

void custom_free(void *ptr) {
  // Don't do anything on freeing NULL.
  if (!ptr) return;

  // Get the block pointer.
  struct block_meta *block_ptr = get_block_ptr(ptr);
  if (block_ptr->free) return;
  size_t used = block_ptr->used;
  used_memory -= used;

  block_ptr->free = true;
  strcpy(block_ptr->magic, "freed");

  coalesce(block_ptr);
  log_memory();
}