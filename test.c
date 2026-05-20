#include "alloc.h"
#include <stdint.h>
#include <stdlib.h>

void test(void) {
  void *ptr = custom_malloc(1024);
  custom_free(ptr);
  void *new = custom_malloc(64);
}