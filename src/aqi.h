#pragma once

#include <Arduino.h>

// Thanks to nyx for the AQI code

unsigned aqi25(float pm25_conc);
unsigned aqi10(float pm10_conc);
unsigned aqi(float pm25_conc, float pm10_conc);
uint8_t aqi_index(unsigned aqi); // returns 1-based index
const char *aqi_condition_name(unsigned aqi);
