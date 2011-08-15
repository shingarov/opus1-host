/*
 * console.c -- Host-side driver for the arduino controller
 * embedded in the Rodgers console.
 *
 * Copyright (c) LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 *
 */

#include "IO.h"
#include "CRASH.h"
#include "hauptwerk.h"
#include "console_led.h"
#include "console_piston.h"

/*
 * Construct a wire protocol command to turn a LED on/off.
 * This is only called from setConsoleLed().
 */
static unsigned char ledCommand(unsigned led, int status) {
  if ((status!=0) && (status!=1)) CRASH("Status mus be 0 or 1");
  if (led > 10) CRASH("Bad console LED number");
  return (status << 4) | led;
}

/*
 * Turn the specified LED ON (status=1) or OFF (status=0).
 * LED numbers:
 *  0    -  M2
 *  1-7  -  1-7
 *  8    -  BASS
 *  9    -  MELODY
 *  10   -  CHIMES
 */
void setConsoleLed(unsigned led, int status) {
  out(ledCommand(led, status));
}

void process_short_message(unsigned channel,
                           unsigned msgType,
                           unsigned value1,
                           unsigned value2) {
  if (channel != CONSOLE_CHANNEL) {
    printf("MIDI message on wrong channel %d: type=%d, value1=%d, value2=%d\n",
         channel, msgType, value1, value2);
    return;
  }
  if ((msgType!=NOTE_ON) && (msgType!=NOTE_OFF)) {
    printf("Unknown message type %d (value1=%d, value2=%d)\n",
         msgType, value1, value2);
    return;
  }
  if (value1>10) {
    printf("Unknown LED number %d\n", value1);
    return;
  }
  setConsoleLed(value1, msgType==NOTE_ON? 1:0);
}

