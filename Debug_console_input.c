#include <stdio.h>
#include <string.h>
#include "LED.h"
#include "serial_io.h"


int main() {
  open_serial_port(RODGERS_COM_PORT);
  for (;;) {
    unsigned char c;
    c = in();
    printf("%X  ", c);
    c = in();
    printf("%X\n", c);
  }
}

