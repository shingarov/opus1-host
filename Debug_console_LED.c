/*
 * Copyright (c) 2009-2014 LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 */

#include <stdio.h>
#include <string.h>
#include "LED.h"
#include "serial_io.h"

static void usage() {
  printf("Usage: DXY on|off\n");
}

static void act_on(char *command) {
  int on_off;
  int d;
  int x, y;
  if (strlen(command)==6) {
    if (strcmp(command+3, " on")) {
      usage();
      return;
    }
    on_off=ON;
  } else {
    if (strcmp(command+3, " off")) {
      usage();
      return;
    }
    on_off=OFF;
  }

  x = *(command+1) - '0';
  y = *(command+2) - '0';

  d = *command - '0';
  switch(d) {
    case 0:
      setConsoleStopLED(x,y,on_off); return;
    case 1:
      set7SegmentLED(x,y,on_off); return;
    default:
      usage();
      return;
  }
}

unsigned char sevenSegmentFont[256] = {
0,0,0,0, 0,0,0,0,  // row 1
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,22, 0,0,0,128, // row 5
0,0,0,0, 1,4,1,0,
250,192,182,214, 204,94,126,194, // row 7
254,222,0,0, 0,20,0,0,
0,234,0,58, 0,62,46,42, // row 9
236,0,208,0, 56,0,0,0,
174,0,0,0, 0,248,0,0, // row 11
0,220,0,58, 0,210,0,16, // row 12
8,246,124,52, 244,190,46,36, // row 13
108,64,80,0, 0,0,100,116, // row 14
0,0,36,0, 60,112,0,0, // row 15
0,0,0,0, 0,0,2,0, // row 16

// 8-bit (upper) half from here
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0
};

/*
 * Set digitNo (0..7, left to right) to show the character value (0..255)
 * according to the font.
 * If dp!=0, light the decimal point; if the font already contains the dp,
 * so be it.
 */
void set7segmentDigit(unsigned digitNo, unsigned char value, int dp) {
  unsigned char mask = (value==-1)? 0:sevenSegmentFont[value];
  int i;
  for (i=0; i<=7; i++) {
    set7SegmentLED(digitNo, i, (mask>>i)&1);
  }
  set7SegmentLED(digitNo, 0, dp);
}

/*
 * display an ASCII string s on the 7segment display,
 * starting at position p (p=0..7).
 */
void print7segment(char *s, unsigned p) {
  char c;
  while (c=*(s++)) {
    set7segmentDigit(p++, c, 0);
    if (p>7) return;
  }
}

void test7segment() {
/*
  set7segmentDigit(0,'1',1);
  set7segmentDigit(1,'2',0);
  set7segmentDigit(2,'3',0);
  set7segmentDigit(3,'4',0);
  set7segmentDigit(4,'5',0);
  set7segmentDigit(5,'6',0);
  set7segmentDigit(6,'7',0);
  set7segmentDigit(7,'8',0);
*/
print7segment("Eggog 95", 0);
}

int main() {
  open_serial_port(RODGERS_COM_PORT);
  for (;;) {
    char buf[128];
    test7segment();
    gets(buf);
    act_on(buf);
  }
}

