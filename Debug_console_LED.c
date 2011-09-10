/*
 * Copyright (c) 2009-2011 LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 */

#include <stdio.h>
#include <string.h>
#include "LED.h"
#include "serial_io.h"

static void usage() {
  printf("Usage: L cmd,\n");
  printf("  L = 0(M2), 1-7, 8(BASS), 9(MELODY), A(CHIMES)\n");
}

static void act_on(char *command) {
  int on_off;
  char X;
  int x;
  if (strlen(command)!=4) {
    usage();
    return;
  }
  if (!strcmp(command+1, " on")) on_off=ON;
  else if (!strcmp(command+1, " off")) on_off=OFF;
  else {
    printf("cmd should be \"on\" or \"off\"\n");
    return;
  }
  X = *command;
  x = X-'0';
  if (X=='A') x=10;
  setConsoleLed(x, on_off);
}

int main() {
  open_serial_port(RODGERS_COM_PORT);
  for (;;) {
    char buf[128];
    gets(buf);
    act_on(buf);
  }
}

