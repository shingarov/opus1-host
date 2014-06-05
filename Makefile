#
#  Makefile -- Drive project compilation.
#  As of OPUS.1R2, we have switched from MS C++ complier (cl) v.15.00 and
#  linker (lib) v.9.00, to mingw32-gcc.
#
#  Copyright (C) 2009-2014 Boris Shingarov and Ladarevo Software Inc.
# 
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along
#  with this program; if not, write to the Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

CC = x86_64-w64-mingw32-gcc
TARGETDIR=/home/boris/lamya/opus.1

all: regisztralopult console

regisztralopult: $(TARGETDIR)/LED.exe $(TARGETDIR)/drawknob.exe

console: $(TARGETDIR)/console.exe





# Debug Utilities

debug: $(TARGETDIR)/debug/byte2console.exe $(TARGETDIR)/debug/Debug_console_LED.exe
# $(TARGETDIR)/debug/Debug_LED.exe $(TARGETDIR)/debug/Debug_LED_2.exe

$(TARGETDIR)/debug/byte2console.exe:
	$(CC) -o $@ -DREADTIMEOUT=50 byte2console.c serial_io.c CRASH.c

$(TARGETDIR)/debug/Debug_console_LED.exe:
	$(CC) -o $@ -DREADTIMEOUT=50 -DNO_C2H Debug_console_LED.c serial_io.c CRASH.c console_wire.c


# End of Debug Utilities

clean:
	rm -f *.obj *.res *.exe

