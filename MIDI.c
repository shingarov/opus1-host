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

#include "teVirtualMIDI/teVirtualMIDI.h"
#include "MIDI.h"

/*
 * Global variables are fine here, as this code is not supposed
 * to be reentrant.
 */
#define MIDI_PORT_IN  LPVM_MIDI_PORT
#define MIDI_PORT_OUT LPVM_MIDI_PORT
static MIDI_PORT_IN  hMidiIn  = (MIDI_PORT_IN)0;
static MIDI_PORT_OUT hMidiOut = (MIDI_PORT_OUT)0;

#define MAX_SYSEX_BUFFER 65535

char *binToStr( const unsigned char *data, DWORD length ) {
        static char dumpBuffer[ MAX_SYSEX_BUFFER * 3 ];
        DWORD index = 0;

        while ( length-- ) {
                sprintf( dumpBuffer+index, "%02x", *data );
                if ( length ) {
                        strcat( dumpBuffer, ":" );
                }
                index+=3;
                data++;
        }
        return dumpBuffer;
}


#ifndef NO_H2C
/*
 * MIDI input callback function.
 * The driver will call this function every time there is
 * a MIDI message on the input port.
 */
static void CALLBACK teVMCallback( LPVM_MIDI_PORT midiPort, LPBYTE midiDataBytes, DWORD length, DWORD_PTR dwCallbackInstance ) {
        if ( ( NULL == midiDataBytes ) || ( 0 == length ) ) {
                printf( "empty command - driver was probably shut down!\n" );
                return;
        }
  if (*midiDataBytes == 0xF0) {
    process_sysex((unsigned char*)midiDataBytes);
  } else {
    unsigned char *x;
    unsigned channel, msgType, value1, value2;
    x = (unsigned char*)midiDataBytes;
    msgType = (0xF0 & x[0]) >> 4;
    channel = 0x0F & x[0];
    value1 = x[1];
    value2 = x[2];
    process_short_message(channel, msgType, value1, value2);
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
  unsigned char x[3];
  x[0] = channel | (msgType << 4);
  x[1] = byte1;
  x[2] = byte2;
  if (!virtualMIDISendData(hMidiOut, x, 3)) {
    CRASH("Error sending short MIDI message");
  }
}

#ifndef NO_H2C
void init_midi_in(LPCWSTR in_device_name) {
        hMidiIn = virtualMIDICreatePortEx2(in_device_name, teVMCallback, 0, MAX_SYSEX_BUFFER, TE_VM_FLAGS_PARSE_RX );
        if (!hMidiIn) {
                CRASH("could not create input port");
                return;
        }
}
#endif  // NO_H2C

void init_midi_out(LPCWSTR out_device_name) {
        hMidiOut = virtualMIDICreatePortEx2(out_device_name, NULL, 0, MAX_SYSEX_BUFFER, TE_VM_FLAGS_PARSE_RX );
        if (!hMidiOut) {
                CRASH("could not create output port");
                return;
        }
}

void reset_midi(void) {
//TODO
}

