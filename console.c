/*
 * console.c -- Host-side driver for the arduino controller
 * embedded in the Rodgers console; main program.
 *
 * Copyright (c) 2009-2014 LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 *
 */

#include "serial_io.h"
#include "MIDI.h"
#include "console_wire.h"

extern void print7segment(char*, unsigned);
extern void getSharedMemory();

int main() {
  open_serial_port(RODGERS_COM_PORT);
  init_midi_out(MIDI_PORT_CONSOLE_PISTONS);
  init_midi_in (MIDI_PORT_CONSOLE_LED);
  atexit(reset_midi); 

  getSharedMemory();
  print7segment("LE05____", 0);

  for (;;) {
    process_console_control(in());
  }
}

