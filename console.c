/*
 * console.c -- Host-side driver for the arduino controller
 * embedded in the Rodgers console; main program.
 *
 * Copyright (c) 2009-2014 LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 *
 */

#include <windows.h>
#include "IO.h"
#include "CRASH.h"
#include "serial_io.h"
#include "MIDI.h"
#include "hauptwerk.h"
#include "console_led.h"
#include "console_wire.h"


int main() {
  open_serial_port(RODGERS_COM_PORT);
  init_midi_out(MIDI_PORT_CONSOLE_PISTONS);
  init_midi_in (MIDI_PORT_CONSOLE_LED);
  atexit(reset_midi); 

  for (;;) {
    process_console_control(in());
  }
}

