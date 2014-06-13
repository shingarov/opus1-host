/*
 * Copyright (c) 2009-2014 LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 *
 * console.c -- Host-side driver for the arduino controller
 * embedded in the Rodgers console.
 *
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "CRASH.h"
#include "MIDI.h"
#include "LED.h"
#include "serial_io.h"
#include "console_wire.h"



unsigned char sevenSegmentFont[256] = {
0,0,0,0, 0,0,0,0,  // row 1
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,22, 0,0,0,128, // row 5
0,0,0,0, 1,4,1,0,
250,192,182,214, 204,94,126,194, // row 7
254,222,0,0, 0,20,0,0,
0,234,0,58, 0,62,46,42, // row 9
236,0,208,0, 56,0,0,0,
174,0,0,0, 0,248,0,0, // row 11
0,220,0,58, 0,210,0,16, // row 12
8,246,124,52, 244,190,46,36, // row 13
108,64,80,0, 0,0,100,116, // row 14
0,0,36,0, 60,112,0,0, // row 15
0,0,0,0, 0,0,2,0, // row 16

// 8-bit (upper) half from here
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0
};

/*
 * Set digitNo (0..7, left to right) to show the character value (0..255)
 * according to the font.
 * If dp!=0, light the decimal point; if the font already contains the dp,
 * so be it.
 */
void set7segmentDigit(unsigned digitNo, unsigned char value, int dp) {
  unsigned char mask = (value==-1)? 0:sevenSegmentFont[value];
  int i;
  for (i=0; i<=7; i++) {
    set7SegmentLED(digitNo, i, (mask>>i)&1);
  }
  set7SegmentLED(digitNo, 0, dp);
}

/*
 * display an ASCII string s on the 7segment display,
 * starting at position p (p=0..7).
 */
void print7segment(char *s, unsigned p) {
  char c;
  while (c=*(s++)) {
    set7segmentDigit(p++, c, 0);
    if (p>7) return;
  }
}


#define BUF_SIZE 8
char sharedMemName[]="FavOrganNo";
static LPCTSTR favOrganBuf;

void getSharedMemory() {
   HANDLE hMapFile;
   hMapFile = OpenFileMapping(
                   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   sharedMemName);               // name of mapping object
   if (hMapFile == NULL)
   {
      printf("Could not open file mapping object (%d).\n",
             GetLastError());
      return;
   }
   favOrganBuf = (LPTSTR) MapViewOfFile(hMapFile, // handle to map object
               FILE_MAP_ALL_ACCESS,  // read/write permission
               0,
               0,
               BUF_SIZE);
   if (favOrganBuf == NULL)
   {
      printf("Could not map view of file (%d).\n",
             GetLastError());
      CloseHandle(hMapFile);
      return;
   }
}

int getFavOrgan() {
  return *((int*)favOrganBuf);
}

void process_short_message(unsigned channel,
                           unsigned msgType,
                           unsigned value1,
                           unsigned value2) {
  unsigned x, y;
  switch (channel) {
    case CONSOLE_DRAWKNOB_CHANNEL:
      if (msgType != NOTE_OFF) {
        printf("Unknown message type %d (value1=%d, value2=%d)\n", msgType, value1, value2);
        return;
      }
      x = (value1&0xF0)>>4;
      if (x>5) {
        printf("x=%d > 5\n", x);
        return;
      }
      y = value1&0x0F;
      if (y>6) {
        printf("y=%d > 6\n", x);
        return;
      }
      setConsoleStopLED(x-1, y-1, msgType==NOTE_ON? 1:0);
      return;
    default:
      printf("Unknown channel %d (value1=%d, value2=%d)\n", channel, value1, value2);
      return;
  }
}

#define SYSX_CurrOrganShortName 0
#define SYSX_CuedFavOrganNum 1
#define SYSX_CuedFavOrganShortName 2
#define SYSX_LastOrganShortName 3
#define SYSX_CurrCombSetShortName 4
#define SYSX_CuedFavCombSetNum 5
#define SYSX_CuedFavCombSetShortName 6
#define SYSX_CurrTemperShortName 7
#define SYSX_CuedFavTemperNum 8
#define SYSX_CuedFavTemperShortName 9
#define SYSX_FloatingDiv1RouteNum 10
#define SYSX_FloatingDiv1KbdShortNameList 11
#define SYSX_FloatingDiv2RouteNum 12
#define SYSX_FloatingDiv2KbdShortNameList 13
#define SYSX_FloatingDiv3RouteNum 14
#define SYSX_FloatingDiv3KbdShortNameList 15
#define SYSX_FloatingDiv4RouteNum 16
#define SYSX_FloatingDiv4KbdShortNameList 17
#define SYSX_FloatingDiv5RouteNum 18
#define SYSX_FloatingDiv5KbdShortNameList 19
#define SYSX_CurrCombStepperFrameNum 20
#define SYSX_CuedCombStepperFrameBankNum 21
#define SYSX_CurrMasterGeneralNum 22
#define SYSX_LastTriggeredMasterRegObjType 23
#define SYSX_CurrMasterCrescNum 24
#define SYSX_CuedMasterCrescStage 25
#define SYSX_TransposerIncSemitones 26
#define SYSX_OrganBasePitchHz 27
#define SYSX_MasterTuningIncCents 28
#define SYSX_OrganAudioLevelTrimDecibels 29
#define SYSX_PolyphonyLimitSetting 30
#define SYSX_SampleRateHz 31
#define SYSX_LatencyMs 32
#define SYSX_IsSetterModeOn 33
#define SYSX_IsScopeModeOn 34
#define SYSX_IsRecordingAudio 35
#define SYSX_IsRecordingMIDI 36
#define SYSX_IsPlayingMIDI 37
#define SYSX_IsOrganReady 38
#define SYSX_IsInErrorState 39
#define SYSX_ProgressIndicatorPct 40
#define SYSX_MasterOutputLevelCCPct 41
#define SYSX_FloatingDiv1ExprPedCCPct 42
#define SYSX_FloatingDiv2ExprPedCCPct 43
#define SYSX_FloatingDiv3ExprPedCCPct 44
#define SYSX_FloatingDiv4ExprPedCCPct 45
#define SYSX_FloatingDiv5ExprPedCCPct 46
#define SYSX_MasterCrescPedCC 47
#define SYSX_AudioLevelClipping 48
#define SYSX_PolyphonyClipping 49
#define SYSX_CPUClipping 50
#define SYSX_RAMClipping 51
#define SYSX_CurrObjShortNameAllTypes 52
#define SYSX_CuedFavNumAllTypes 53
#define SYSX_CuedFavShortNameAllTypes 54
#define SYSX_AllTypesObjTyp 55


static char *hw_sysex_names[] = {
  "CurrOrganShortName",
  "CuedFavOrganNum",
  "CuedFavOrganShortName",
  "LastOrganShortName",
  "CurrCombSetShortName",
  "CuedFavCombSetNum",
  "CuedFavCombSetShortName",
  "CurrTemperShortName",
  "CuedFavTemperNum",
  "CuedFavTemperShortName",
  "FloatingDiv1RouteNum",
  "FloatingDiv1KbdShortNameList",
  "FloatingDiv2RouteNum",
  "FloatingDiv2KbdShortNameList",
  "FloatingDiv3RouteNum",
  "FloatingDiv3KbdShortNameList",
  "FloatingDiv4RouteNum",
  "FloatingDiv4KbdShortNameList",
  "FloatingDiv5RouteNum",
  "FloatingDiv5KbdShortNameList",
  "CurrCombStepperFrameNum",
  "CuedCombStepperFrameBankNum",
  "CurrMasterGeneralNum",
  "LastTriggeredMasterRegObjType",
  "CurrMasterCrescNum",
  "CuedMasterCrescStage",
  "TransposerIncSemitones",
  "OrganBasePitchHz",
  "MasterTuningIncCents",
  "OrganAudioLevelTrimDecibels",
  "PolyphonyLimitSetting",
  "SampleRateHz",
  "LatencyMs",
  "IsSetterModeOn",
  "IsScopeModeOn",
  "IsRecordingAudio",
  "IsRecordingMIDI",
  "IsPlayingMIDI",
  "IsOrganReady",
  "IsInErrorState",
  "ProgressIndicatorPct",
  "MasterOutputLevelCCPct",
  "FloatingDiv1ExprPedCCPct",
  "FloatingDiv2ExprPedCCPct",
  "FloatingDiv3ExprPedCCPct",
  "FloatingDiv4ExprPedCCPct",
  "FloatingDiv5ExprPedCCPct",
  "MasterCrescPedCC",
  "AudioLevelClipping",
  "PolyphonyClipping",
  "CPUClipping",
  "RAMClipping",
  "CurrObjShortNameAllTypes",
  "CuedFavNumAllTypes",
  "CuedFavShortNameAllTypes",
  "AllTypesObjType"
};

static int isOrganReady = 0;
static void event_organIsNotReady() {
  isOrganReady = 0;
  print7segment("        ", 0);
}

#define TEMPERAMENT_ZwolleMeantone 1
#define TEMPERAMENT_DomBedos415Hz 2
#define TEMPERAMENT_Well_KirnbergerIII 3
#define TEMPERAMENT_Well_WerckmeisterIII 4
#define TEMPERAMENT_Well_Kellner 5
#define TEMPERAMENT_Well_VallottiYoung 6
#define TEMPERAMENT_Well_FifthComma 7
#define TEMPERAMENT_Well_SeventhComma 8
#define TEMPERAMENT_Well_Young 9
#define TEMPERAMENT_Mean_QCPA 10
#define TEMPERAMENT_Mean_5Comma 11
#define TEMPERAMENT_Mean_6Comma 12
#define TEMPERAMENT_Mean_Mod6Comma 13
#define TEMPERAMENT_Just_Natural 14
#define TEMPERAMENT_Just_Pythagor 15
#define TEMPERAMENT_Ordinaire 16
#define TEMPERAMENT_8Comma 17
#define TEMPERAMENT_10Comma 18
#define TEMPERAMENT_OrganGabler 19
#define TEMPERAMENT_OrganSilbermann 20
// additional start from 21
#define TEMPERAMENT_FreibergDomAD1982 21
#define TEMPERAMENT_FreibergDomAD1985 22
#define TEMPERAMENT_FreibergDom458Hz  23
#define TEMPERAMENT_FreibergPetrikirche458Hz 24
#define TEMPERAMENT_FreibergPetrikircheNeihardt 25
#define TEMPERAMENT_Bach1 26
#define TEMPERAMENT_Bach2 27
#define TEMPERAMENT_Bach3 28
#define TEMPERAMENT_Bach4 29
#define TEMPERAMENT_KrzeszowMeantone 30

static int temperament_positions[] = {
  12,
  22,
  14, // kirn-3
  24, // werck
  34, // kellner
  44, // VY
  26, // 5c
  36, // 7c
  54, // Y
  42, // QCPA
  52, // mean 5c
  13, // mean 6c
  23, // mod mean 6c
nat
pyth
ord
8c
10c



};

int temperamentXY2N(int x, int y) {

}

static char *temperament_shortnames[] = {
  // Equal and Original are shifted to the end
  "Zw-mt           ",
  "Bd415           ",
  // standard
  "Well-KirnIII    ",
  "Well-WerkIII    ",
  "Well-Kellner    ",
  "Well-VallotY    ",
  "Well-1/5c       ",
  "Well-1/7c       ",
  "Well-Young      ",
  "Mean-1/4c       ",
  "Mean-1/5c       ",
  "Mean-1/6c       ",
  "Mean-Mod1/6c    ",
  "Just-Natural    ",
  "Just-Pythag     ",
  "Ordinaire       ",
  "1/8c            ",
  "1/10c           ",
  "Org-Gabler      ",
  "Org-Silbermn    ",
  // additional start from 21
  "FrbDS           ",
  "Frb85           ",
  "FrbSS           ",
  "FrbgM           ",
  "FrbgN           ",
  "Bach1           ",
  "Bach2           ",
  "Bach3           ",
  "Bach4           ",
  "krzes           "
};

#define TEMPERAMENT_ORIGINAL -1
#define TEMPERAMENT_EQUAL -2
#define TEMPERAMENT_UNKNOWN -3
static int temp_no = 0;
static int currentTemperamentNo() {
  return temp_no;
}
void printCurrentTemperament() {
  char buf[32];
  int temp = currentTemperamentNo();
  if (temp==TEMPERAMENT_EQUAL) { print7segment("E ", 3); return; }
  if (temp==TEMPERAMENT_ORIGINAL) { print7segment("0g", 3); return; }
  if (temp==TEMPERAMENT_UNKNOWN) { print7segment("--", 3); return; }
  sprintf(buf, "%d ", temp+1); // TODO print positions
  print7segment(buf, 3);
}

static int current_combination = -1;
void printCurrentCombination() {
  char buf[32];
  sprintf(buf, "%d ", current_combination);
  print7segment(buf, 6);
}

static void temperamentIs(unsigned char *name) {
  if      (!strcmp(name, "Equal           ")) temp_no = TEMPERAMENT_EQUAL;
  else if (!strcmp(name, "OrigOrgTemp     ")) temp_no = TEMPERAMENT_ORIGINAL;
  else {
    int i;
    for (i=0; i<30; i++) {
      if (!strcmp(name, temperament_shortnames[i])) {
        temp_no=i;
        return;
      }
    }
    temp_no=TEMPERAMENT_UNKNOWN;
  }
}

static void event_organIsReady() {
  char buf[32];
  isOrganReady = 1;

  // organ No.
  sprintf(buf, "%d  ", getFavOrgan());
  print7segment(buf, 0);

  printCurrentTemperament();
  printCurrentCombination();
}

static void event_progressIndicatorPct(unsigned char percent) {
  char buf[32];
  sprintf(buf, "%d", percent);
  print7segment(buf, 0);
}

void process_7bit_sysex(unsigned char *data) {
  unsigned char value = data[4];
  switch (data[3]) {
    case SYSX_IsOrganReady:
      if (value) event_organIsReady(); else event_organIsNotReady();
      return;
    case SYSX_ProgressIndicatorPct:
      event_progressIndicatorPct(value);
      return;
    case SYSX_IsInErrorState:
      if (value) print7segment("Eggog   ", 0);
      return;
    default:
      return;
  }
}

void process_28bit_sysex(unsigned char *data) {
  unsigned int a,b,c,d, value;
  a = data[4]; b = data[5]; c = data[6]; d = data[7];
  value = (a<<21) | (b<<14) | (c<<7) | d;
  switch (data[3]) {
    case SYSX_CurrCombStepperFrameNum:
      current_combination = value;
      if (isOrganReady) printCurrentCombination();
      return;
    default:
      printf("28bit: %s->%d\n", hw_sysex_names[data[3]], value);
  }
}

void process_16char_sysex(unsigned char *data) {
  data[20]=0;
  switch (data[3]) {
    case SYSX_CurrTemperShortName:
      temperamentIs(data+4);
      printCurrentTemperament();
      return;
    default:
      return;
  }
}

void process_sysex(unsigned char *data) {
  unsigned char kind = data[2];
  if (data[0] != 0xF0) return;
  if (data[1] != 0x7D) return;
  switch (kind) {
    case 0x1B: process_7bit_sysex(data); return;
    case 0x1A: process_28bit_sysex(data); return;
    case 0x19: process_16char_sysex(data); return;
    default: return;
  }
}

