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

unsigned char sevenSegmentFont[] = {250, 192, 182,214, 204,94,126,194,254,222};

void set7segmentDigit(unsigned digitNo, int value, int dp) {
  unsigned char mask = (value==-1)? 0:sevenSegmentFont[value];
  int i;
  for (i=0; i<=7; i++) {
    set7SegmentLED(digitNo, i, (mask>>i)&1);
  }
  set7SegmentLED(digitNo, 0, dp);
}

void test7segment() {
  set7segmentDigit(0,1,1);
  set7segmentDigit(1,2,0);
  set7segmentDigit(2,3,0);
  set7segmentDigit(3,4,0);
  set7segmentDigit(4,5,0);
  set7segmentDigit(5,6,0);
  set7segmentDigit(6,7,0);
  set7segmentDigit(7,8,0);

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

