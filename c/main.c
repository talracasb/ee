#include "alloc.h"
#include "test.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static void log_block(struct block_meta *b, const char *event) {
  // clang-format off
  printf(
    "{\n"
    "\"event\": \"%s\",\n"
    "\"block_addr\": \"%p\",\n"
    "\"meta_size\": %d,\n"
    "\"user_addr\": \"%p\",\n"
    "\"size\": %zu,\n"
    "\"free\": %s,\n"
    "\"next_block\": \"%p\",\n"
    "\"magic\": \"%s\"\n"
    "}%s\n",
    event,
    (void *)b,
    META_SIZE,
    (void *)(b + 1),
    b->size,
    b->free ? "true" : "false",
    (void *)b->next,
    b->magic,
    b->next ? "," : ""
  );
  // clang-format on
}

struct block_meta *log_memory_map() {
  struct block_meta *current = base;

  // First-fit allocation. It goes through each block meta,
  // and then returns when it finds a block that's big
  // enough.
  printf("[\n");
  while (current) {
    log_block(current, "info");
    current = current->next;
  }
  printf("]\n");

  return current;
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    printf("error: select an argument\n");
    return 1;
  }

  if (strcmp(argv[1], "map") == 0) {
    test();
    log_memory_map();
  } else {
    printf("error: please select 'map'\n");
    return 1;
  }
}
