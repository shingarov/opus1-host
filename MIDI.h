/*
 * MIDI.h -- Ladarevo Opus 1's interface to OS's MIDI subsystem.
 *
 * Copyright (c) Ladarevo Software, 2009, 2011.
 *
 */

/**********************************
 *       Exported functions       *
 **********************************/

void init_midi_in(const char *in_device_name);
void init_midi_out(const char *out_device_name);

void reset_midi(void);

void send_short_message(unsigned channel,
                        unsigned msgType,
                        unsigned value1,
                        unsigned value2);


/**********************************
 *       Imported functions       *
 **********************************/

void process_short_message(unsigned channel,
                           unsigned msgType,
                           unsigned value1,
                           unsigned value2);

