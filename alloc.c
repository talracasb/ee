#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>
#include "alloc.h"
#include <unistd.h>

struct block_meta *base = NULL;

// Function which actually finds a free block.
static struct block_meta *find(struct block_meta **previous, size_t size) {
  struct block_meta *current = base;

  // First-fit allocation. It goes through each block meta,
  // and then returns when it finds a block that's big enough.
  while (current && !(current->free && current->size >= size)) {
    // log_block(current, "loop iteration to find");
    *previous = current;
    current = current->next;
  }

  return current;
}

// Request a new block by extending the heap (sbrk just extends the data segment.)
static struct block_meta *request(struct block_meta* previous, size_t size) {
  // Create a new block pointer at the current program break.
  struct block_meta *block = sbrk(0);

  // Requests the block.
  void *request = sbrk((size + META_SIZE));

  // Makes sure that the block allocated correctly.
  // This is done by ensuring that the address returned by the second `sbrk`
  // call is the same as the previous program break.
  assert((void*)block == request); // Not thread safe.
  if (request == (void*)-1) {
    return NULL; // sbrk failed.
  }

  // If there was a previous block, then ensure it's correctly added to the linked list.
  if (previous) previous->next = block;

  // Populate the block metadata.
  block->size = size;
  block->next = NULL;
  block->free = false;
  strcpy(block->magic, "requested");

  return block;
}

// Gets the pointer to the start of the block_meta with the pointer to it's data.
static struct block_meta *get_block_ptr(void *ptr) {
  // Pointer arithmetic in C is based on the type, so this goes back one unit of block_meta.
  return (struct block_meta*)ptr - 1;
}

// Little helper to find the previous block.
// In a real allocator, there would be a pointer attached to each block for the sake of performance.
static struct block_meta *find_previous(struct block_meta *block) {
  struct block_meta *current = base;
  if (!current || current == block) return NULL;

  while (current && current->next != block) {
    current = current->next;
  }

  return current;
}

void *malloc(size_t size) {
  if (size <= 0) return NULL;

  if (!base) { // First call, we need to allocate a block at the base.
    base = request(NULL, size);
    if (!base) return NULL;
    return base + 1;
  }

  struct block_meta *previous = base;

  // Find a new block.
  struct block_meta *block = find(&previous, size);
  if (!block) { // Failed to find free block.
    block = request(previous, size);
    if (!block) return NULL;
  } else {
    // TODO: consider splitting block here.
    block->free = false;
    strcpy(block->magic, "found");
  }

  return (block + 1);
}

// Coalesces surrounding blocks if possible and returns the new, hopefully bigger, block.
static struct block_meta* coalesce(struct block_meta *block) {
  if (!block) return NULL;

  // Merge with next block if free
  if (block->next && block->next->free) {
    struct block_meta *next = block->next;

    block->size += META_SIZE + next->size;
    block->next = next->next;

    strcpy(block->magic, "merged_next");
  }

  // Merge with previous block if free
  struct block_meta *prev = find_previous(block);
  if (prev && prev->free) {
    prev->size += META_SIZE + block->size;
    prev->next = block->next;

    strcpy(prev->magic, "merged_prev");
    block = prev;
  }

  return block;
}

void free(void *ptr) {
  // Don't do anything on freeing NULL.
  if (!ptr) return;

  // Get the block pointer.
  struct block_meta* block_ptr = get_block_ptr(ptr);

  // Just mark it as free.
  block_ptr->free = 1;
  strcpy(block_ptr->magic, "freed");

  coalesce(block_ptr);
}