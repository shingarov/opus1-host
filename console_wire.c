/*
 * console.c -- Host-side driver for the arduino controller
 * embedded in the Rodgers console; main program.
 *
 * Copyright (c) LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 *
 */

#include "CRASH.h"
#include "console_wire.h"

/*
 * console->host, initial entry point:
 * we just got a byte on the wire
 */
void process_console_control(unsigned char x) {
  if ((x&0x7C)==0x60) process_stud(x&0x03, x&0x80!=0);
  else if ((x&0x70)==0x40) process_piston(x&0x0F, x&0x80!=0);
  else if (!(x&0x40)) process_drawknob((x&0x38)>>3, x&0x07, x&0x80!=0);
  else CRASH("UNKNOWN REPORT FROM CONSOLE");
}

