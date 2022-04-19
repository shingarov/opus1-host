/*
 * console.c -- Host-side driver for the arduino controller
 * embedded in the Rodgers console; main program.
 *
 * Copyright (c) 2009-2022 LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 *
 */

#include "CRASH.h"
#include "console_wire.h"

#ifndef NO_H2C
/*
 * host->console
 */

/*
 * Construct a wire protocol command to turn a LED on/off.
 * This is only called from setConsoleStopLED() and from set7SegmentLED().
 */
static unsigned char ledCommand(unsigned displayNo, unsigned x, unsigned y, unsigned status) {
  return (status<<7) | (displayNo<<6) | (x<<3) | y;
}

void setConsoleStopLED(unsigned x, unsigned y, unsigned status) {
  out(ledCommand(0, x, y, status));
}

void set7SegmentLED(unsigned x, unsigned y, unsigned status) {
  out(ledCommand(1, x, y, status));
}
#endif // NO_H2C

#ifndef NO_C2H
/*
 * console->host, initial entry point:
 * we just got a byte on the wire
 */
void process_console_control(unsigned char x) {
  // decode the ON/OFF bit
  int on = (x&0x80)!=0;

  // look at the top bits to dispatch to the correct scancode space
  if ((x&0x7C)==0x60) process_stud(x&0x03, on);
  else if ((x&0x70)==0x40) process_piston(x&0x0F, on);
  else if (!(x&0x40)) process_drawknob((x&0x38)>>3, x&0x07, on);
  else CRASH("UNKNOWN REPORT FROM CONSOLE");
}
#endif // NO_C2H


