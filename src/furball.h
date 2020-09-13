#pragma once

/* expose some sensors that were previously private to furball.cpp
 * we need these for display.cpp and this is the most expedient way to do it
 * this needs some redesign at a later time
 */
#include "sensors/bme680_sensor.h"
#include "sensors/tsl2561_sensor.h"
#include "sensors/pms_sensor.h"

extern BME680_Sensor bme680;
extern TSL2561_Sensor tsl2561;
extern PMS_Sensor pms5003;

void furball_setup();
void furball_loop();
void furball_stream();
