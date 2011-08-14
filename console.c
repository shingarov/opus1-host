/*
 * LED.c -- Host-side driver for the console.
 *
 * Copyright (c) LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 *
 */

#include "IO.h"
#include "CRASH.h"
#include "console.h"

/*
 * Construct a wire protocol command to turn a LED on/off.
 */
static unsigned char ledCommand(unsigned led, int status) {
  if ((status!=0) && (status!=1)) CRASH("Status mus be 0 or 1");
  if (led > 10) CRASH("Bad console LED number");
  return (status << 4) | led;
}

void setConsoleLed(unsigned led, int status) {
  out(ledCommand(led, status));
}

