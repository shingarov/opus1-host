/*
 * console.c -- Host-side driver for the arduino controller
 * embedded in the Rodgers console; main program.
 *
 * Copyright (c) LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 *
 */

#include "IO.h"
#include "CRASH.h"
#include "console_piston.h"
#include "hauptwerk.h"

static void process_analog(unsigned which, unsigned value) {
  value = value / 8;
  send_short_message(CONSOLE_CHANNEL,
                     CC,
                     which? MOD_WHEEL : MAIN_VOLUME,
                     value);
}

static void process_digital(unsigned status, unsigned p, unsigned NNNN) {
  if (p) {
    /*
     * Pistons (Rodgers' diode matrix)
     */
    #define UNDEFINED -1
    #define M2 0
    #define BASS 8
    #define MELODY 9
    #define CHIMES 10
    #define RG 11
    #define SET 12
    #define PRESSED 1
    #define NOT_PRESSED 0
    int note_number;
    static int R = NOT_PRESSED;
    static int PISTONS[16] =
      { UNDEFINED, // special -- R is mapped
        1,4,2,5,3,6,7,
        M2,RG,UNDEFINED,SET,BASS,CHIMES,MELODY,UNDEFINED };
    if (NNNN==0) {
      // R is pressed.
      // R is special, sort of "SHIFT" which adds 16 to the MIDI note number.
      R = status? NOT_PRESSED : PRESSED;
      return;
    }
    note_number = PISTONS[NNNN];
    if (note_number == UNDEFINED) {
      // oh...
      return;
    }
    if (R==PRESSED) note_number += 16;
    send_short_message(CONSOLE_CHANNEL,
                       status? NOTE_OFF:NOTE_ON,
                       note_number,
                       127);
  } else {
    /*
     * Non-pistons -- switches not using the Rodgers' diode matrix.
     * These directly pull down pins on the Arduino Mega.
     * One pin per one switch.
     * LOW==ACTIVE.
     */
    send_short_message(CONSOLE_CHANNEL,
                       status? NOTE_OFF:NOTE_ON,
                       NNNN+64,
                       127);
  }
}

void process_console_control(unsigned char high, unsigned char low) {
  if (high & 0x80) {
    unsigned value = ((high&0x03)<<8) | low;
    if (high & 0x3C) CRASH("Invalid pair of bytes received from console (analog?)");
    process_analog((high&0x40)>>6, value);
  } else {
    if (high & 0xBF) CRASH("Invalid pair of bytes received from console (digital)");
    if (low & 0x70) CRASH("Invalid pair of bytes received from console (digital)");
    process_digital(high>>6, (low&0x80)>>7, low&0x0F);
  }
}

