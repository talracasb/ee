#include "alloc.h"

void *p[32];

void test(void) {
  // Phase 1: seed fragmentation
  p[0] = custom_malloc(24);
  p[1] = custom_malloc(16);
  p[2] = custom_malloc(32);
  p[3] = custom_malloc(8);
  p[4] = custom_malloc(40);

  custom_free(p[1]);
  custom_free(p[3]);

  p[5] = custom_malloc(12);
  p[6] = custom_malloc(8);
  p[7] = custom_malloc(20);

  custom_free(p[2]);

  p[8] = custom_malloc(28);
  p[9] = custom_malloc(10);

  // // Phase 2: reuse pressure with gaps
  custom_free(p[0]);
  custom_free(p[4]);

  p[10] = custom_malloc(30);
  p[11] = custom_malloc(14);
  p[12] = custom_malloc(18);

  custom_free(p[6]);
  custom_free(p[5]);

  p[13] = custom_malloc(6);
  p[14] = custom_malloc(22);

  // Phase 3: churn + fragmentation amplification
  custom_free(p[7]);
  custom_free(p[8]);

  p[15] = custom_malloc(35);
  p[16] = custom_malloc(9);

  custom_free(p[9]);
  custom_free(p[10]);

  p[17] = custom_malloc(27);
  p[18] = custom_malloc(13);

  // Phase 4: awkward sizing pressure
  custom_free(p[11]);
  custom_free(p[12]);
  custom_free(p[13]);

  p[19] = custom_malloc(5);
  p[20] = custom_malloc(11);
  p[21] = custom_malloc(19);

  custom_free(p[14]);
  custom_free(p[15]);

  p[22] = custom_malloc(26);
  p[23] = custom_malloc(7);

  // Phase 5: more fragmentation + partial reuse
  custom_free(p[16]);
  custom_free(p[17]);

  p[24] = custom_malloc(17);
  p[25] = custom_malloc(21);

  custom_free(p[18]);
  custom_free(p[19]);

  p[26] = custom_malloc(12);
  p[27] = custom_malloc(8);

  // Phase 6: messy ending state
  custom_free(p[20]);
  custom_free(p[21]);
  custom_free(p[22]);

  p[28] = custom_malloc(14);
  p[29] = custom_malloc(16);

  p[30] = custom_malloc(9);
  p[31] = custom_malloc(6);

  custom_free(p[23]);
  custom_free(p[24]);

  p[0] = custom_malloc(20);
  p[1] = custom_malloc(10);
}