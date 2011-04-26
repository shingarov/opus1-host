/*
 * Hauptwerk MIDI settings.
 */

// MIDI ports
#define MIDI_PORT_DRAWKNOBS_OPUS1_TO_HAUPTWERK "1. Internal MIDI"
#define MIDI_PORT_LED_HAUPTWERK_TO_OPUS1       "2. Internal MIDI"
#define MIDI_PORT_CONTROL_OPUS1_TO_HAUPTWERK   "3. Internal MIDI"

// MIDI channels (0-based)
#define SPEAKING_STOP_CHANNEL 0
#define COUPLER_CHANNEL 1
#define ORGAN_SELECTION_CHANNEL 2
#define SYSTEM_STATUS_CHANNEL 3

// Standard MIDI constants
#define NOTE_ON 9
#define NOTE_OFF 8

//  #define HAUPTWERK_PATH "C:\\Program Files\\Hauptwerk Virtual Pipe Organ\\Hauptwerk.exe"
#define HAUPTWERK_PATH "C:\\windows\\system32\\calc.exe"
#define HAUPTWERK_FOLDER "\"C:\\Program Files\\Hauptwerk Virtual Pipe Organ\""
