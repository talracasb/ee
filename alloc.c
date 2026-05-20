#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "alloc.h"

struct block_meta {
  char magic[16];
  bool free;
  size_t size;
  struct block_meta *next;
};

#define META_SIZE sizeof(struct block_meta)

static void log_block(struct block_meta *b, const char *event) {
  fprintf(stderr,
    "\n[allocator] %s\n"
    "  block addr : %p (size: %d)\n"
    "  user addr  : %p\n"
    "  size       : %zu bytes\n"
    "  free       : %s\n"
    "  next block : %p\n"
    "  magic      : %s\n",
    event,
    (void*)b,
    META_SIZE,
    (void*)(b + 1),
    b->size,
    b->free ? "yes" : "no",
    (void*)b->next,
    b->magic
  );
}

struct block_meta *base = NULL;

// Function which actually finds a free block.
struct block_meta *find(struct block_meta **previous, size_t size) {
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
struct block_meta *request(struct block_meta* previous, size_t size) {
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
  log_block(block, "new block requested");

  return block;
}

struct block_meta *get_block_ptr(void *ptr) {
  return (struct block_meta*)ptr - 1;
}

void *malloc(size_t size) {
  if (size <= 0) {
    return NULL;
  }

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
    log_block(block, "reusing block");
  }

  return (block + 1);
}

void free(void *ptr) {
  if (!ptr) {
    return;
  }

  struct block_meta* block_ptr = get_block_ptr(ptr);
  block_ptr->free = 1;
  strcpy(block_ptr->magic, "freed");
}