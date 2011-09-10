/*
 * Copyright (c) 2009-2011 LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 */

#include <stdio.h>
#include <string.h>
#include "serial_io.h"


int main() {
  open_serial_port(RODGERS_COM_PORT);
  for (;;) {
    unsigned char c;
    c = in();
    printf("%X  ", c);
    c = in();
    printf("%X\n", c);
  }
}

