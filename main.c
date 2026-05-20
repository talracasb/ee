#include <stdio.h>
#include <math.h>
#include "alloc.h"

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

struct block_meta *log_memory_map() {
  struct block_meta *current = base;

  // First-fit allocation. It goes through each block meta,
  // and then returns when it finds a block that's big enough.
  while (current) {
    log_block(current, "info");
    current = current->next;
  }

  return current;
}

int main()
{
    char *ptrs[8];

    for (unsigned long i = 0; i < 8; i++)
    {
        ptrs[i] = malloc(16);
    }

    free(ptrs[4]);
    free(ptrs[3]);
    log_memory_map();
}
