/*
 *  MIDI.c -- Low-level MIDI sending/receiving, message callback.
 *
 *  Copyright (C) 2009 Boris Shingarov
 *  Copyright (C) 2014 Ladarevo Software
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include <windows.h>
#include <mmsystem.h>
#include "MIDI.h"

/*
 * Global variables are fine here, as this code is not supposed
 * to be reentrant.
 */
#define MIDI_PORT_IN  HMIDIIN
#define MIDI_PORT_OUT HMIDIOUT
static MIDI_PORT_IN  hMidiIn  = (MIDI_PORT_IN)0;
static MIDI_PORT_OUT hMidiOut = (MIDI_PORT_OUT)0;

static unsigned char sysex_buffer[1024];
MIDIHDR sysex_header;

#ifndef NO_H2C
/*
 * MIDI input callback function.
 * The driver will call this function every time there is
 * a MIDI message on the input port.
 */
static void FAR PASCAL midiInputHandler(HMIDIIN hMidiIn,
                                        WORD wMsg,
                                        DWORD dwInstance,
                                        DWORD dwParam1,
                                        DWORD dwParam2) {
  switch(wMsg) {
    case MIM_DATA:
      {
      unsigned channel, msgType, note, velocity;
      channel  = (dwParam1 & 0x0000000F);
      msgType  = (dwParam1 & 0x000000F0) >> 4;
      note     = (dwParam1 & 0x0000FF00) >> 8;
      velocity = (dwParam1 & 0x00FF0000) >> 16;

      // Call back into our user
      process_short_message(channel, msgType, note, velocity);
      }
      break;
    case MIM_LONGDATA:
      process_sysex(sysex_buffer);
      midiInAddBuffer(hMidiIn, &sysex_header, sizeof(sysex_header));
      break;
    default:
      break;
  }
}
#endif // NO_H2C


/**
 * Send a short message on the out interface,
 * if one was configured at the time of init_midi().
 */
void send_short_message(unsigned channel,
                        unsigned msgType,
                        unsigned byte1,
                        unsigned byte2) {
  unsigned param1;
  if (!hMidiOut) return;
  param1 =
    channel |
    (msgType << 4) |
    (byte1 << 8) |
    (byte2 << 16);
  midiOutShortMsg(hMidiOut, param1);
}

/*
 * Return the number (index) of the physical input device
 * named @name, or -1 if not found.
 */
static int find_input_device(const char *name) {
  int numDevices;
  int i;

  numDevices = midiInGetNumDevs();
  for (i=0; i<numDevices; i++) {
    MIDIINCAPS midiInCaps;
    UINT rc = midiInGetDevCaps(i, (LPMIDIINCAPS)&midiInCaps, sizeof(MIDIINCAPS));
    if (rc) return -1;
    if (!lstrcmp(midiInCaps.szPname, name)) return i;
  }
  return -1;
}

/*
 * Return the number (index) of the physical output device named @name, or -1 if not found.
 */
static int find_output_device(const char *name) {
  int numDevices;
  int i;

  numDevices = midiOutGetNumDevs();
  for (i=0; i<numDevices; i++) {
    MIDIOUTCAPS midiOutCaps;
    UINT rc = midiOutGetDevCaps(i, (LPMIDIOUTCAPS)&midiOutCaps, sizeof(MIDIOUTCAPS));
    if (rc) return -1;
    if (!lstrcmp(midiOutCaps.szPname, name)) return i;
  }
  return -1;
}

#ifndef NO_H2C
void init_midi_in(const char *in_device_name) {
	int inputDeviceNo, rc;

	inputDeviceNo = -1;
        while (inputDeviceNo == -1) {
          inputDeviceNo = find_input_device(in_device_name);
          sleep(1);
        }
	rc = midiInOpen(&hMidiIn,
                        inputDeviceNo,
                        (DWORD_PTR)midiInputHandler,
                        0,
                        CALLBACK_FUNCTION);
	if (rc) CRASH("midiInOpen()");
	if (!hMidiIn) CRASH("hMidiIn==NULL");
        sysex_header.lpData = sysex_buffer;
        sysex_header.dwBufferLength = 1024;
        sysex_header.dwFlags = 0;
        midiInPrepareHeader(hMidiIn, &sysex_header, sizeof(sysex_header));
        midiInAddBuffer(hMidiIn, &sysex_header, sizeof(sysex_header));
        midiInStart(hMidiIn);
}
#endif  // NO_H2C

void init_midi_out(const char *out_device_name) {
  int outputDeviceNo, rc;

  outputDeviceNo = -1;
  while (outputDeviceNo==-1) {
    outputDeviceNo = find_output_device(out_device_name);
    sleep(1);
  }
  rc = midiOutOpen(&hMidiOut, outputDeviceNo, (DWORD_PTR)NULL, (DWORD_PTR)NULL, 0);
  if (rc) return; 
  if (!hMidiOut) return;
}

void reset_midi() {
  if (hMidiIn) {
    midiInStop (hMidiIn);
    midiInReset(hMidiIn);
    midiInClose(hMidiIn);
  }
  if (hMidiOut) {
    midiOutReset(hMidiOut);
    midiOutClose(hMidiOut);
  }
}

