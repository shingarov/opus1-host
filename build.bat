rem Copyright (c) 2009-2011 LADAREVO SOFTWARE INC.
rem LADAREVO ORGELBAU LAB

cl /DREADTIMEOUT=0 /DSERIALBAUD=\"9600\" /DNO_C2H LED.c LED_main.c CRASH.c MIDI.c serial_io.c teVirtualMIDI64.lib
cl /DREADTIMEOUT=0 /DSERIALBAUD=\"9600\" /DNO_H2C drawknob.c CRASH.c MIDI.c serial_io.c teVirtualMIDI64.lib
rem cl /DREADTIMEOUT=0 Debug_LED.c CRASH.c serial_io.c LED.c
rem cl Debug_LED_2.c CRASH.c serial_io.c LED.c
cl /DREADTIMEOUT=50 /DSERIALBAUD=\"28800\" console.c console_led.c console_in.c console_wire.c CRASH.c serial_io.c MIDI.c teVirtualMIDI64.lib
rem cl /DREADTIMEOUT=50 Debug_console_LED.c CRASH.c serial_io.c console_led.c
rem cl Debug_console_input.c CRASH.c serial_io.c
rem cl byte2console.c serial_io.c

