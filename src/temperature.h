#ifndef Temperature_h
#define Temperature_h

#include <Arduino.h>
#include <Homie.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define MAX_TEMPSENSORS 5
#define VALID_MAX 80.0
#define VALID_MIN -80.0

void temperature_setup();
double read_temp();
double get_min_temp();
double get_temp(uint8_t num);

#endif
