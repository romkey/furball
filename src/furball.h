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

#define DDC_AIR_SENSOR "org.homebus.experimental.air-sensor"
#define DDC_AIR_QUALITY_SENSOR "org.homebus.experimental.air-quality-sensor"
#define DDC_LIGHT_SENSOR "org.homebus.experimental.light-sensor"
#define DDC_SYSTEM "org.homebus.experimental.system"
#define DDC_DIAGNOSTIC "org.homebus.experimental.diagnostic"
#define DDC_AQI_SENSOR "org.homebus.experimental.aqi-pm25"

void furball_setup();
void furball_loop();
void furball_stream();
