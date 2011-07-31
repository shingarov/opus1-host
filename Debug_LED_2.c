/*
 * Lights all LEDs on the Opus.1 one after the other.
 * First, the organ selection row, then speaking stops,
 * then couplers, then status.
 * After all LEDs are lit, turn them off in the same order.
 *
 * Copyright (c) 2011, Ladarevo Software Inc.
 */

#include <stdio.h>
#include <string.h>
#include <windows.h> // for Sleep()
#include "LED.h"
#include "serial_io.h"

void wait_a_little(void) {
  Sleep(100);
}

static void illuminate(OnOff on_off) {
  int x, y;

  /* Organ selection */
  for (x=1; x<=9; x++) {
    setOrganLed(x, on_off);
    wait_a_little();
  }

  /* Speaking stops */
  for (y=1; y<=9; y++)
  for (x=1; x<=5; x++) {
    setStopLed(x, y, on_off);
    wait_a_little();
  }

  /* Couplers */
  for (x=1; x<=4; x++) {
    setCouplerLed(x, on_off);
    wait_a_little();
  }

  /* Status */
  for (x=1; x<=4; x++) {
    setStatusLed(x, on_off);
    wait_a_little();
  }
}

int main() {
  open_serial_port(LED_COM_PORT);
  illuminate(ON);
  illuminate(OFF);
}

