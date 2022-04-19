/*
 * console_in.c -- Host-side driver for the arduino controller
 * embedded in the Rodgers console; take the reports from the
 * console (as parsed by console_wire) and transform them to MIDI.
 *
 * Copyright (c) 2009-2022 LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 *
 */

// #include <stdio.h>
#include "console_wire.h"
#include "MIDI.h"

/*
 * There are three kinds of controls the organist can push on the console.
 * 1) Stop Drawknobs. We transmit NOTE_ON/OFF on CONSOLE_DRAWKNOB_CHANNEL (6)
 *    note number 0xXY, X=1..5, Y=1..6.
 *    However, if the R modifier is pressed, we transmit a NOTE_ON/OFF on the
 *    TEMPERAMENT_CHANNEL (7) instead; the note number is the temperament number
 *    as defined by the extern function temperamentXY2N().
 * 2) Pistons.   We transmit NOTE_ON/OFF on CONSOLE_CHANNEL (5)
 *    note numbers N=1..12, or N+16 if the R modifier is pressed,
 *    or N+32 if the M2 modifier is pressed.
 * 3) Studs.  We transmit NOTE_ON/OFF on CONSOLE_CHANNEL (5)
 *    note numbers 64+S, S=0..3, or 64+4+S if the R modifier is pressed.
 *
 * NB: The reference to channel numbers above is 0-based.
 */


// MIDI notes for pistons
#define UNDEFINED -1
#define BASS 8
#define MELODY 9
#define CHIMES 10
#define RG 11
#define SET 12

#define PRESSED 1
#define NOT_PRESSED 0
static int R  = NOT_PRESSED;
static int M2 = NOT_PRESSED;

void process_piston(int p, int on) {
    int note_number;
    static int PISTONS[16] =
      { UNDEFINED /* R */, // we don't send MIDI when modifier is toggled
        1,4,2,5,3,6,7,
        UNDEFINED /* M2 */,
        RG,
        UNDEFINED, // no piston has scan code 0x*A
        SET,
        BASS,
        CHIMES,
        MELODY,
        UNDEFINED // no piston has scan code 0x*F
      };
    if (p==0) {
      // R changed state
      R = on? PRESSED : NOT_PRESSED;
      return;
    }
    if (p==8) {
      // M2 changed state
      M2 = on? PRESSED : NOT_PRESSED;
      return;
    }
    note_number = PISTONS[p];
    if (note_number == UNDEFINED) {
      // this can never happen.  (should we CRASH() instead?)
      return;
    }
    if (R==PRESSED) note_number += 16;
    if (M2==PRESSED) note_number += 32;
    send_short_message(CONSOLE_CHANNEL,
                       on? NOTE_ON:NOTE_OFF,
                       note_number,
                       127);
}


void process_stud(int s, int on) {
    send_short_message(CONSOLE_CHANNEL,
                       on? NOTE_ON:NOTE_OFF,
                       s+64+(R==PRESSED?4:0),
                       127);
}

extern int temperamentXY2N(int x, int y);

void process_drawknob(int x, int y, int on) {
    unsigned int note;
    note = ((x+1)<<4)|(y+1);
    if (R==NOT_PRESSED) {
      send_short_message(CONSOLE_DRAWKNOB_CHANNEL,
                       on? NOTE_ON:NOTE_OFF,
                       note,
                       127);
    } else {
      // R+Drawknob = load temperament
      if (!on) return;
      note = temperamentXY2N(x+1, y+1);
      if (note==-1) return; // should not happen
      send_short_message(TEMPERAMENT_CHANNEL,
                       NOTE_ON,
                       note,
                       127);
    }
}

