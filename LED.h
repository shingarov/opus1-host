/*
 * LED.h -- API declarations for the64-LED matrix.
 *
 * Copyright (c) LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 *
 * On the Opus.1 stop action, there are several groups of LEDs.
 * At the top, there is a row of 9 knobs for organ selection;
 * when an organ is selected, one of 9 LEDs is lit.
 * Immediately below the organ selection row, there are 9 rows
 * of speaking stops, 5 stops per row.
 * At the very bottom, there are 4 couplers, and also 4 status LEDs.
 * The "audio mode" is not LED-illuminated; it is an actual mechanical
 * pull knob.
 */

#ifndef LED_h
#define LED_h

typedef int OnOff;
#define ON  1
#define OFF 0

/**
 * Main (speaking stops) LEDs.
 * 1 <= x <= 5,
 * 1 <= y <= 9
 */
void setStopLed(int x, int y, OnOff status);

/*
 * Organ selection LEDs.
 * 1 <= organNo <= 9
 */
void setOrganLed(int organNo, OnOff status);

/*
 * Hauptwerk status LEDs.  S=1,2,3,4
 */
void setStatusLed(int S, OnOff status);

/**
 * Coupler LED.
 * 1 <= C <= 4
 */
void setCouplerLed (int C, OnOff status);

/**
 * OK LED (the green LED under the audio mode knob.
 */
void setOK(OnOff status);

#endif

