#include <stdio.h>
#include "CRASH.h"

void CRASH(const char *msg) {
  printf("%s\n", msg);
  exit(1);
}


