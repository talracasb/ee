#include "alloc.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct block_meta *base = NULL;

static struct block_meta *find(struct block_meta **out_prev, size_t size) {
  struct block_meta *prev = NULL;
  struct block_meta *cur = base;

  while (cur) {
    if (cur->free && cur->size >= size) {
      *out_prev = prev;
      return cur;
    }
    prev = cur;
    cur = cur->next;
  }

  *out_prev = prev;
  return NULL;
}

// Request a new block by extending the heap (sbrk just extends the data segment.)
static struct block_meta *request(struct block_meta *previous, size_t size) {
  // Create a new block pointer at the current program break.
  struct block_meta *block = sbrk(0);

  // Requests the block.
  void *request = sbrk((size + META_SIZE));

  // Makes sure that the block allocated correctly.
  // This is done by ensuring that the address returned by the second `sbrk`
  // call is the same as the previous program break.
  assert((void *)block == request); // Not thread safe.
  if (request == (void *)-1) {
    return NULL; // sbrk failed.
  }

  // If there was a previous block, then ensure it's correctly added to the linked list.
  if (previous)
    previous->next = block;

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
  return (struct block_meta *)ptr - 1;
}

// Little helper to find the previous block.
// In a real allocator, there would be a pointer attached to each block for the sake of performance.
static struct block_meta *find_previous(struct block_meta *block) {
  struct block_meta *current = base;
  if (!current || current == block)
    return NULL;

  while (current && current->next != block) {
    current = current->next;
  }

  return current;
}

static void split(struct block_meta *block, size_t size) {
  if (block->size < size + META_SIZE + 8)
    return;

  struct block_meta *new =
      (struct block_meta *)((char *)(block + 1) + size);

  new->size = block->size - size - META_SIZE;
  new->free = true;
  new->next = block->next;
  strcpy(new->magic, "split");

  block->size = size;
  block->next = new;
}

void *custom_malloc(size_t size) {
  if (size == 0) // First call, we need to allocate a block at the base.
    return NULL;

  struct block_meta *prev = NULL;

  if (!base) {
    base = request(NULL, size);
    if (!base) return NULL;
    return base + 1;
  }

  struct block_meta *block = find(&prev, size);

  if (!block) {
    block = request(prev, size);
    if (!block) return NULL;
  } else {
    split(block, size);
    block->free = false;
    strcpy(block->magic, "found");
  }

  return block + 1;
}

// Coalesces surrounding blocks if possible and returns the new, hopefully bigger, block.
static void coalesce(struct block_meta *block) {
  if (!block) return;

  bool merged = true;
  while (merged) {
    merged = 0;

    // Merge with next block if free
    if (block->next && block->next->free) {
      struct block_meta *next = block->next;

      // Adjacency check
      if ((char *)(block + 1) + block->size == (char *)next) {
        block->size += META_SIZE + next->size;
        block->next = next->next;
        strcpy(block->magic, "merged_next");
        merged = 1;
      }
    }

    // Merge with previous block if free
    struct block_meta *prev = find_previous(block);
    if (prev && prev->free) {
      if ((char *)(prev + 1) + prev->size == (char *)block) {
        prev->size += META_SIZE + block->size;
        prev->next = block->next;
        strcpy(prev->magic, "merged_prev");
        block = prev;
        merged = 1;
      }
    }
  }
}

void custom_free(void *ptr) {
  // Don't do anything on freeing NULL.
  if (!ptr)
    return;

  // Get the block pointer.
  struct block_meta *block_ptr = get_block_ptr(ptr);

  // Just mark it as free.
  block_ptr->free = true;
  strcpy(block_ptr->magic, "freed");

  coalesce(block_ptr);
}