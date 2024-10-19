#include "rand.h"

uint32_t g_rand_seed = 0;

void srand(uint32_t seed) {
    g_rand_seed = seed;
}

int rand() {
  int result;

  g_rand_seed *= 1103515245;
  g_rand_seed += 12345;
  result = (uint32_t) (g_rand_seed / 65536) % 2048;

  g_rand_seed *= 1103515245;
  g_rand_seed += 12345;
  result <<= 10;
  result ^= (uint32_t) (g_rand_seed / 65536) % 1024;

  g_rand_seed *= 1103515245;
  g_rand_seed += 12345;
  result <<= 10;
  result ^= (uint32_t) (g_rand_seed / 65536) % 1024;

  return result;
}

