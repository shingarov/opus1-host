#include <windows.h>
#include "serial_io.h"
#include "LED.h"
#include "MIDI.h"
#include "hauptwerk.h"
#include "CRASH.h"

// Possible organ loading states
#define ORGAN_NOT_LOADED 0
#define ORGAN_LOADING 1
#define ORGAN_LOADED 2

static void start_drawknobs() {
  BOOL success;
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );

  success = CreateProcess(
    NULL,
    "c:\\OPUS.1\\drawknob.exe",
    NULL, // process attributes
    NULL, //thread attributes
    FALSE, // inherit handles
    CREATE_NEW_CONSOLE, // flags
    NULL, //environment
    "C:\\OPUS.1",  // current dir
    &si,
    &pi);
  if (!success) {
    setStatusLed(4, ON);
    CRASH("Could not spawn drawknobs.exe");
  }

  success = CreateProcess(
    NULL,
    "c:\\OPUS.1\\console.exe",
    NULL, // process attributes
    NULL, //thread attributes
    FALSE, // inherit handles
    CREATE_NEW_CONSOLE, // flags
    NULL, //environment
    "C:\\OPUS.1",  // current dir
    &si,
    &pi);
  if (!success) {
    setStatusLed(4, ON);
    CRASH("Could not spawn console.exe");
  }
}

static void allOrganLoadingLedsOff(void) {
  int x;
  for (x=1; x<=9; x++)
    setOrganLed(x, OFF);
}

static void allLedsOff(void) {
  int x,y;

  for (x=1; x<=5; x++)
  for (y=1; y<=9; y++)
    setStopLed(x,y, OFF);

  for (x=1; x<=4; x++)
    setCouplerLed(x, OFF);

  for (x=1; x<=4; x++)
    setStatusLed(x, OFF);

  allOrganLoadingLedsOff();
}

static int organ_status = ORGAN_NOT_LOADED;
static int favorite_organ = 0; // 1-9, or 0 if none selected

static void organ_loading_tick() {
  static int current = OFF;
  if (organ_status == ORGAN_LOADING) {
    current = (current==ON)? OFF:ON;
    setOrganLed(favorite_organ, current);
  }
}

int main() {
  open_serial_port(LED_COM_PORT);
  allLedsOff();
  init_midi_in(MIDI_PORT_LED_HAUPTWERK_TO_OPUS1);
  atexit(reset_midi);
  start_drawknobs();

  for (;;) {
    Sleep(200); //TODO: enable asynchronous reading from the Mega; real timer for organ loading led
    organ_loading_tick();
  }
}

static void process_speaking_stop(unsigned msgType, unsigned stop) {
  int x,y;
  x = (stop&0xF0) >> 4;
  y = (stop&0x0F);
  if (msgType == NOTE_ON) setStopLed(x,y, ON);
  else if (msgType == NOTE_OFF) setStopLed(x,y, OFF);
  // do not react on stray messages
}

static void process_organ_selection(unsigned msgType, unsigned value1 ) {
      if (value1==0) { // ORGAN LOADING ON/OFF
        if (msgType==NOTE_ON) {
          allOrganLoadingLedsOff();
          organ_status = ORGAN_LOADING;
        } // NOTE_OFF ignored
      } else if ((value1>=1) && (value1<=9)) { // Favorite Organ
        if (msgType==NOTE_ON) {
          favorite_organ = value1;
        } // NOTE_OFF ignored
      } else if (value1==127) { // AUDIO ACTIVE
        if (msgType==NOTE_ON) {
          organ_status = ORGAN_LOADED;
          allOrganLoadingLedsOff();
          setOrganLed(favorite_organ, ON);
        } else {
          organ_status = ORGAN_NOT_LOADED;
          allOrganLoadingLedsOff();
        }
     }
}

static void process_system_status(unsigned msgType, unsigned note ) {
  setStatusLed(note, msgType==NOTE_ON? ON:OFF);
}

static void process_coupler(unsigned msgType, unsigned note ) {
  setCouplerLed(note, msgType==NOTE_ON? ON:OFF);
}

static void process_ok(unsigned msgType, unsigned note ) {
  // Any NOTE ON on the OK channel will turn the OK LED on,
  // Any NOTE OFF on the OK channel will turn the OK LED off.
  // NB: we do not care which note it is.
  setOK(msgType==NOTE_ON? ON:OFF);
}

void process_short_message(unsigned channel,
                           unsigned msgType,
                           unsigned value1,
                           unsigned value2) {
  switch (channel) {
    case SPEAKING_STOP_CHANNEL:
      process_speaking_stop(msgType, value1);
      break;
    case ORGAN_SELECTION_CHANNEL:
      process_organ_selection(msgType, value1);
      break;
    case SYSTEM_STATUS_CHANNEL:
      process_system_status(msgType, value1);
      break;
    case COUPLER_CHANNEL:
      process_coupler(msgType, value1);
      break;
    case OK_CHANNEL:
      process_ok(msgType, value1);
      break;
    default:
      break;
  }
}

