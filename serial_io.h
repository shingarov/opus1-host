#ifndef serial_io_h
#define serial_io_h

#define RODGERS_COM_PORT  "\\\\.\\COM3"
#define KNOB_COM_PORT "\\\\.\\COM4"
#define LED_COM_PORT "\\\\.\\COM6"

void open_serial_port(const char *portName);
void out(unsigned char x);
unsigned char in(void);

#endif

