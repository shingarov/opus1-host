#include <windows.h>
#include "CRASH.h"
#include "MIDI.h"
#include "hauptwerk.h"
#include "serial_io.h"

static void organ(unsigned int organNo) {
  send_short_message(ORGAN_SELECTION_CHANNEL,
                     NOTE_ON,
                     organNo,
                     127);
}

static void coupler(unsigned int couplerNo) {
  send_short_message(COUPLER_CHANNEL,
                     NOTE_ON,
                     couplerNo,
                     127);
}

static void speaking_stop(unsigned int x, unsigned int y) {
  send_short_message(SPEAKING_STOP_CHANNEL,
                     NOTE_ON,
                     (x<<4) | y,
                     127);
}

static void start_hauptwerk() {
  BOOL success;
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );

  success = CreateProcess(
    "c:\\windows\\system32\\cmd.exe",
    "/c c:\\starthw.bat",
    NULL, // process attributes
    NULL, //thread attributes
    FALSE, // inherit handles
    CREATE_NEW_CONSOLE, // flags
    NULL, //environment
    "c:\\",  // current dir
    &si,
    &pi);
  if (!success) {
    CRASH("Could not spawn hauptwerk.exe");
  }
}

/**
 * Take the action based on the encoded drawknob information.
 * The code is 0xXY,
 * X - column (1-9, 1-5, 1-4),
 * Y - row:
 *   0: Organ selection
 *   1-9: Speaking stops
 *   A: Couplers
 *
 * NB: Because we have two separate microcontrollers, the Duemilanove
 * is now exclusively concerned with "drawknob pressed" scancodes: no
 * other commands are transmitted on that channel.
 */
void process_raw_drawknob_input(unsigned char c) {
  unsigned int x,y;
  x = (c&0xF0) >> 4;
  y = c&0x0F;

  if (y==0) organ(x);
  else if (y==0x0A) coupler(x);
  else if ((y>=1) && (y<=9)) speaking_stop(x, y);
  else CRASH("Bad drawknob input");
}

int main() {
  open_serial_port(KNOB_COM_PORT);
  init_midi_out(MIDI_PORT_DRAWKNOBS_OPUS1_TO_HAUPTWERK);
  start_hauptwerk();
  for (;;) {
    unsigned char c;
    c = in();
    process_raw_drawknob_input(c);
  }
}

void process_short_message(unsigned channel, unsigned msgType, unsigned value1, unsigned value2) {
  CRASH("Drawknob controller does not accept MIDI messages");
}

