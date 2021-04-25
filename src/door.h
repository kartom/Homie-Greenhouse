#ifndef Door_h
#define Door_h

#include <Arduino.h>
#include <Homie.h>
#include "settings.h"

#define DOOR_STEPTIME 1000

void door_setup();
void door_loop();

void door_set_man_opening(uint8_t num, float opening);
void door_set_auto_opening(float opening);
void door_set_auto(uint8_t num, bool auto_value);
float door_get_opening_sp();
int door_get_opening(uint8_t num);
String door_get_state(uint8_t num);

#endif