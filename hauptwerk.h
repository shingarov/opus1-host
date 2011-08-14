/*
 * Hauptwerk MIDI settings.
 */

// MIDI ports:
// The Drawknobs port is uni-directional from the control surface
// to Hauptwerk; it is related to the  physical Duemilanove which
// only transmits data to the host.
#define MIDI_PORT_DRAWKNOBS_OPUS1_TO_HAUPTWERK "1. Internal MIDI"

// The LED port unidirectional: the Uno to which the LEDs are
// connected, only receives data from the host.
#define MIDI_PORT_LED_HAUPTWERK_TO_OPUS1       "2. Internal MIDI"

// MIDI channels (0-based).
// Note that even though these are per-MIDI-port, we share the numbers.
#define SPEAKING_STOP_CHANNEL 0
#define COUPLER_CHANNEL 1
#define ORGAN_SELECTION_CHANNEL 2
#define SYSTEM_STATUS_CHANNEL 3
#define OK_CHANNEL 4

// Standard MIDI constants
#define NOTE_ON  9
#define NOTE_OFF 8

