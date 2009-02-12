#include <stdio.h>

#include "util.h"

int main()
  {

  float test = 10.42;
  unsigned int* test_addr = ((unsigned int*) &test);
  printf("float bytes: 0x%x\n", *test_addr);

  unsigned int packed_bytes = pack_float(test);
  printf("packed bytes: 0x%x\n", packed_bytes);

  float unpacked = unpack_float(&packed_bytes);
  unsigned int* unpacked_addr = ((unsigned int*) &unpacked);
  printf("unpacked bytes: 0x%x\n", *unpacked_addr);

  printf("orig: %f\n", test);
  printf("unpacked: %f\n", unpacked);

  }
