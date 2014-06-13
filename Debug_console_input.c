/*
 * Copyright (c) 2009-2014 LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 */

#include <stdio.h>
#include "serial_io.h"

/*
 * Dump the raw byte stream coming from the console, as hex, one byte per line.
 * Don't try to interpret the wire protocol.
 */
int main() {
  open_serial_port(RODGERS_COM_PORT);
  for (;;) {
    unsigned char c;
    c = in();
    printf("%X\n", c);
  }
}

