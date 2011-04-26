/*
 * LED.c -- Host-side driver for the 64-LED matrix.
 *
 * Copyright (c) LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 *
 * On the Opus.1 stop action, there are several groups of LEDs.
 * At the top, there is a row of 9 knobs for organ selection;
 * when an organ is selected, one of 9 LEDs is lit.
 * Immediately below the organ selection row, there are 9 rows
 * of speaking stops, 5 stops per row.
 * At the very bottom, there are 4 couplers, and also 4 status LEDs.
 * The "audio mode" is not LED-illuminated; it is an actual mechanical
 * pull knob.
 */

#include "LED.h"
#include "IO.h"
#include "CRASH.h"

/*
 * Construct a wire protocol command to turn a LED on/off given
 * matrix coordinates.  The matrix coordinates are contained in
 * argument m as two nibbles: 0xXY.  For example, matrix coordinates
 * of the first organ selection LED are (4,3), represented as 0x43.
 */
static unsigned matrixCoordinatesToCommand(unsigned m, OnOff status) {
  unsigned x = (m&0xF0) >> 4;
  unsigned y = m&0x0F;
  unsigned s = status? 0x40:0;
  return s | x | (y<<3);
}

void setStopLed(int x, int y, OnOff status) {
  static unsigned M[9][5] = {
    { 0x44, 0x47, 0x42, 0x46, 0x40 },
    { 0x74, 0x77, 0x72, 0x76, 0x70 },
    { 0x04, 0x07, 0x02, 0x06, 0x00 },
    { 0x14, 0x17, 0x12, 0x16, 0x10 },
    { 0x54, 0x57, 0x52, 0x56, 0x50 },
    { 0x34, 0x37, 0x32, 0x36, 0x30 },
    { 0x64, 0x67, 0x62, 0x66, 0x60 },
    { 0x24, 0x27, 0x22, 0x26, 0x20 },
    { 0x41, 0x71, 0x01, 0x11, 0x51 }
  };
  if ((x<1 || x>5) || (y<1 || y>9)) CRASH("Coordinate out of range");
  out(matrixCoordinatesToCommand(M[y-1][x-1], status));
}

void setOrganLed(int organNo, OnOff status) {
  static unsigned M[9] = {
    0x43, 0x73, 0x03, 0x13, 0x53, 0x33, 0x63, 0x23, 0x31
  };
  if (organNo<1 || organNo>9) CRASH("Organ No. out of range");
  out(matrixCoordinatesToCommand(M[organNo-1], status));
}

void setStatusLed(int S, OnOff status) {
  static unsigned M[4] = {
    0x45, 0x75, 0x05, 0x15
  };
  if (S<1 || S>4) CRASH("Status No. out of range");
  out(matrixCoordinatesToCommand(M[S-1], status));
}

void setCouplerLed (int C, OnOff status) {
  static unsigned M[4] = {
    0x55, 0x65, 0x35, 0x25
  };
  if (C<1 || C>4) CRASH("Coupler No. out of range");
  out(matrixCoordinatesToCommand(M[C-1], status));
}


