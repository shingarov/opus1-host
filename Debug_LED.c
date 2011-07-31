#include <stdio.h>
#include <string.h>
#include "LED.h"
#include "serial_io.h"

static void usage() {
  printf("Usage: XY cmd\n");
  printf("  where Y is either 0-9, K or S,\n");
  printf("  X is accordingly 1-9, 1-5, or 1-4,\n");
  printf("  and cmd is on or off\n");
}

static void act_on(char *command) {
  OnOff on_off;
  char X, Y;
  int x;
  if ((strlen(command)!=5) && (strlen(command)!=6)) {
    usage();
    return;
  }
  if (!strcmp(command+2, " on")) on_off=ON;
  else if (!strcmp(command+2, " off")) on_off=OFF;
  else {
    printf("cmd should be \"on\" or \"off\"\n");
    return;
  }
  X = *command;
  x = X - '0';
  Y = *(command+1);
  if (Y=='K') setCouplerLed(x, on_off);
  else if (Y=='S') setStatusLed(x, on_off);
  else if (Y=='0') setOrganLed(x, on_off);
  else setStopLed(x, Y-'0', on_off);
}

int main() {
  open_serial_port(LED_COM_PORT);
  for (;;) {
    char buf[128];
    gets(buf);
    act_on(buf);
  }
}

