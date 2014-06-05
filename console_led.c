/*
 * console.c -- Host-side driver for the arduino controller
 * embedded in the Rodgers console.
 *
 * Copyright (c) LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 *
 */

#include "IO.h"
#include "CRASH.h"
#include "hauptwerk.h"
#include "console_wire.h"





void process_short_message(unsigned channel,
                           unsigned msgType,
                           unsigned value1,
                           unsigned value2) {
/*  if (channel != CONSOLE_CHANNEL) {
    printf("MIDI message on wrong channel %d: type=%d, value1=%d, value2=%d\n",
         channel, msgType, value1, value2);
    return;
  }
  if ((msgType!=NOTE_ON) && (msgType!=NOTE_OFF)) {
    printf("Unknown message type %d (value1=%d, value2=%d)\n",
         msgType, value1, value2);
    return;
  }
  if (value1>10) {
    printf("Unknown LED number %d\n", value1);
    return;
  }
  setConsoleLed(value1, msgType==NOTE_ON? 1:0);
*/
}

char *hw_sysex_names[] = {
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

void process_7bit_sysex(unsigned char *data) {
  printf("7bit: %s->%d\n", hw_sysex_names[data[3]], data[4]);
}

void process_28bit_sysex(unsigned char *data) {
  unsigned int a,b,c,d, value;
  a = data[4]; b = data[5]; c = data[6]; d = data[7];
  value = (a<<21) | (b<<14) | (c<<7) | d;
  printf("28bit: %s->%d\n", hw_sysex_names[data[3]], value);
}

void process_16char_sysex(unsigned char *data) {
  data[20]=0;
  printf("char: %s->%s\n", hw_sysex_names[data[3]], data+4);
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

