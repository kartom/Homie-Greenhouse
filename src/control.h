#ifndef Control_h
#define Control_h

#include <Arduino.h>
#include <Homie.h>
#include "temperature.h"
#include "settings.h"

void control_setup();
float control_loop(double sample_interval, double temperature, float opening, float gain);

#endif