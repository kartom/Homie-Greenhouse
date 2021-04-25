#ifndef Display_h
#define Display_h

#include <Arduino.h>
#include <Homie.h>
#include <U8x8lib.h>
#include <Wire.h>
#include "settings.h"

#define MAX_DISPLAY_HEIGHT 8
#define DISPLAY_ADDRESS 0x3C
#define DISPLAY_WAKE_TIME 60000 //One minute

void display_setup(void);
void display_loop(void);
void display_println(String str);

/**
 * Flash a two row message on the display
 * @param  row1 : First line
 * @param  row2 : Second line
 */
void display_flash(String row1, String row2);

/**
 * Wakes up the display
 * @param  reset : resets the display sequence
 */
void display_wake(bool reset=false);

/**
 * Returns true if display is on
 */
bool display_is_awake(void); 

#endif