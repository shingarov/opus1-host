/*
 * Copyright (c) 2009-2011 LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 */

#include <stdio.h>
#include "CRASH.h"

void CRASH(const char *msg) {
  printf("%s\n", msg);
  exit(1);
}


