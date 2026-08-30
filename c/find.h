#pragma once
#include <sys/types.h>

enum Strategy {
  FIRST,
  BEST,
  WORST,
  NEXT
};

extern enum Strategy strategy;
extern struct block_meta *next_cursor;

struct block_meta *find(size_t size);