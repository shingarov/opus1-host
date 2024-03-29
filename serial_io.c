/*
 * Copyright (c) 2009-2014 LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 */

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "serial_io.h"
#include "CRASH.h"

#ifndef SERIALBAUD
#error SERIALBAUD must be defined
#endif

int DEBUG=1;

// Global handle is ok, we are not reentrant/concurrent.
HANDLE h;

static void set_comm_parm(void) {
  DCB dcb;
  COMMTIMEOUTS ctm;
  char *portParm;
  portParm = "baud=" SERIALBAUD " parity=N data=8 stop=1";

  FillMemory(&dcb, sizeof(dcb), 0);
  dcb.DCBlength = sizeof(dcb);
  if (!BuildCommDCB(portParm, &dcb)) CRASH("BuildCommDCB() failed");
  if (!SetCommState(h, &dcb)) CRASH("SetCommState() failed");
  if (!GetCommTimeouts(h, &ctm)) CRASH("GetCommTimeouts() failed");
  ctm.ReadIntervalTimeout = 0;
  ctm.ReadTotalTimeoutMultiplier = 0;
  // very ugly; see below.  set this to num of miliseconds or to 0
  ctm.ReadTotalTimeoutConstant = READTIMEOUT;
  if (!SetCommTimeouts(h, &ctm)) CRASH("SetCommTimeouts() failed");
}

void open_serial_port(const char *portName) {
  if (DEBUG)
    printf("Opening serial port (%s)...\n", portName);
  h = CreateFile(portName,
                  GENERIC_READ | GENERIC_WRITE,
                  0, 0, OPEN_EXISTING, 0, 0);
  if (h == INVALID_HANDLE_VALUE) CRASH("Unable to open LED COM port");
  if (DEBUG)
    printf("...port open OK, now setting comm parms...\n");
  set_comm_parm();
  if (DEBUG)
    printf("Port setup OK.\n");
}

void out(unsigned char x) {
  DWORD written;
  BOOL success;
  success = WriteFile(h, &x, 1, &written, NULL);
  if ((!success) || (written!=1)) CRASH("WriteFile failed");
}

unsigned char in(void) {
  unsigned char b;
  BOOL success;
  DWORD actuallyRead;
again:
  success = ReadFile(h, &b, 1, &actuallyRead, NULL);
  if (!success) CRASH("ReadFile failed");
  // Very ugly.  I'd just wait here forever, but unfortunately,
  // MIDI input messages (if any) will not have a chance to come
  // in if we sit blocked on comm i/o; even though they are supposedly
  // interrupt based.
  if (actuallyRead != 1) goto again;
  return b;
}

