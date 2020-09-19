#pragma once

#define WIFI_SSID1     "wifi name 1"
#define WIFI_PASSWORD1 "wifi password 1"

#define WIFI_SSID2     "wifi name 2"
#define WIFI_PASSWORD2 "wifi password 2"

#define WIFI_SSID3     "wifi name 3"
#define WIFI_PASSWORD3 "wifi password 3"

#define GMT_OFFSET_SECS  -8 * 60 * 60
#define DAYLIGHT_SAVINGS_OFFSET_SECS 3600

#define UPDATE_DELAY 60*1000

#define DISPLAY_SCREEN_INTERVAL 1000
#define DISPLAY_SCREENS { DISPLAY_SCREEN_ALL_AIR, DISPLAY_SCREEN_ALL_AIR_F, DISPLAY_SCREEN_ALL_LIGHT, DISPLAY_SCREEN_ALL_PARTICLE, DISPLAY_SCREEN_ALL_OFF }
#define DISPLAY_SCREEN_MESSAGE_TEXT "Hello world"

#define MQTT_HOST "MQTT_BROKER_NAME_OR_IP"
#define MQTT_PORT 1883

#define MQTT_USER "MQTT_BROKER_USERNAME"
#define MQTT_PASS "MQTT_BROKER_PASSWORD"
#define MQTT_UUID "UUID_FOR_THIS_CLIENT"

// by default all MQTT topics are prefixed by homebus/device/UUID_FOR_THIS_CLIENT - a Homebus convention
#define MQTT_OVERRIDE_TOPIC_PREFIX "YOUR-NEW-TOPIC-PREFIX"

// these are the defaults that furball.cpp publishes to - appended to MQTT_OVERRIDE_TOPIC_PREFIX
// for instance
// homebus/device/UUID_FOR_THIS_CLIENT/org.homebus.experimental.air-sensor
#define MQTT_OVERRIDE_TOPIC_AIR_SENSOR         "org.homebus.experimental.air-sensor"
#define MQTT_OVERRIDE_TOPIC_AIR_QUALITY_SENSOR "org.homebus.experimental.air-quality-sensor" 
#define MQTT_OVERRIDE_TOPIC_LIGHT_SENSOR       "org.homebus.experimental.light-sensor"
#define MQTT_OVERRIDE_TOPIC_SYSTEM             "org.homebus.experimental.system"
#define MQTT_OVERRIDE_TOPIC_DIAGNOSTIC	       "org.homebus.experimental.diagnostic"
#define MQTT_OVERRIDE_TOPIC_AQI_SENSOR         "org.homebus.experimental.aqi-pm25"
