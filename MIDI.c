/*
 *  MIDI.c -- Low-level MIDI sending/receiving, message callback.
 *
 *  Copyright (C) 2009 Boris Shingarov
 *  Copyright (C) 2011 Ladarevo Software
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
static HMIDIIN  hMidiIn = 0;
static HMIDIOUT hMidiOut = 0;

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
    default:
      break;
  }
}

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

void init_midi_in(const char *in_device_name) {
	int inputDeviceNo, rc;

	inputDeviceNo = find_input_device(in_device_name);
	if (inputDeviceNo==-1) return;
	rc = midiInOpen(&hMidiIn, inputDeviceNo, (DWORD_PTR)midiInputHandler, 0, CALLBACK_FUNCTION);
	if (rc) return;
	if (!hMidiIn) return;
        midiInStart(hMidiIn);
}

void init_midi_out(const char *out_device_name) {
  int outputDeviceNo, rc;

  outputDeviceNo = find_output_device(out_device_name);
  if (outputDeviceNo==-1) return;
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

