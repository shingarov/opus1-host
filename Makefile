#
#  MIDITransform -- transform stop change messages from SysEx bitmap
#  format to individual engage/disengage messages.
#
#  Makefile -- Drive project compilation.
#  I have successfully used this with MS C++ complier (cl) v.15.00,
#  linker (lib) v.9.00, and resource compiler (rc) v.6.1.
#
#  Copyright (C) 2009 Boris Shingarov
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


all: MIDITransform.exe

OBJECTS = Main.obj MIDI.obj Windowing.obj TransformationLogic.obj

MIDITransform.exe: $(OBJECTS) MIDITransform.res
	lib /OUT:"$@" $(OBJECTS) MIDITransform.res

MIDITransform.res: MIDITransform.rc
	rc /fo"$@" MIDITransform.rc

clean:
	del *.obj *.res *.exe

