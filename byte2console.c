/*
 * byte2console -- Debug utility for the LED circuitry
 * in the Ladarevo OPUS.1R2 console.  Simply send one byte
 * on the wire.
 *
 * Example usage:
 *   byte2console 80
 * This will send the byte 0x80 to the console.
 *
 * Copyright (c) 2009-2014 LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 *
 */

#include <stdio.h>
#include "serial_io.h"


int main(int argc, char **argv) {
  unsigned char x;
  if (argc != 2) {
    printf("Usage: byte2console <byte>\n");
    printf("  (byte is base 16 without the leading '0x')\n");
    return 1;
  }

  x = strtol(argv[1], NULL, 16);
  open_serial_port(RODGERS_COM_PORT);
  out(x);
  return 0;
}

