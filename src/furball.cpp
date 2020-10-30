#include "furball.h"

#include <Arduino.h>
#include <time.h>

#include "config.h"
#include "hw.h"

#include <multiball/app.h>
#include <multiball/wifi.h>
#include <multiball/homebus.h>

#ifdef USE_DIAGNOSTICS
#include <diagnostics.h>
#endif

#include "sensors/bme680_sensor.h"
#include "sensors/tsl2561_sensor.h"
#include "sensors/pms_sensor.h"

#include "sensors/pir_sensor.h"
#include "sensors/sound_level_sensor.h"

#include "sensors/uptime.h"

#include "display.h"
#include "aqi.h"

Uptime uptime;

BME680_Sensor bme680(UPDATE_DELAY, 0, 0, false);
TSL2561_Sensor tsl2561(UPDATE_DELAY, 0, 0, false);
PMS_Sensor pms5003(UPDATE_DELAY, 0, 0, false);

static PIR_Sensor pir(1, UPDATE_DELAY, 0, 0, false);
static SoundLevel_Sensor sound_level(SOUND_PIN, UPDATE_DELAY, 0, 0, false);

void furball_setup() {

  bme680.begin();
  if(bme680.is_present())
    Serial.println("[bme680]");
  else
    Serial.println("BME68 not found");

  tsl2561.begin();
  if(tsl2561.is_present())
    Serial.println("[tsl2561]");
  else
    Serial.println("TSL2561 not found");

  // really need to rewrite to detect the presence of the PMS5003
  pms5003.begin(Serial1);
  Serial.println("[pms5003]");

  // we're not using these in the air quality monitor
#if 0
  pir.begin();
  Serial.println("[pir]");

  sound_level.begin();
  Serial.println("[sound intensity]");
#endif

  if(display_begin())
    Serial.println("[display]");
  else
    Serial.println("Dsplay not found");

  //  homebus_reset();

#ifdef MQTT_HOST
  Serial.println("[mqtt credentials]");
  homebus_stuff(MQTT_HOST, MQTT_PORT, MQTT_USER, MQTT_PASS, MQTT_UUID);
#ifdef MQTT_OVERRIDE_TOPIC_PREFIX
  homebus_mqtt_override_prefix(MQTT_OVERRIDE_TOPIC_PREFIX);
#endif
#endif

#ifdef HOMEBUS_NO_ENVELOPE
  homebus_use_envelope(false);
#endif

  homebus_set_provisioner(HOMEBUS_SERVER, HOMEBUS_AUTHENTICATION_TOKEN);

  static const char *ro_ddcs[] = {
		      DDC_AIR_SENSOR,
		      DDC_AIR_QUALITY_SENSOR,
		      DDC_LIGHT_SENSOR,
		      DDC_SYSTEM,
		      DDC_DIAGNOSTIC,
		      DDC_AQI_SENSOR,
		      NULL
  };
  static const char *wo_ddcs[] = { NULL };
  static char mac_address[3*6];

  strncpy(mac_address, App.mac_address().c_str(), 3*6);

  // this is... wrong - needs to be sorted for proper Homebus use
  homebus_configure("Homebus",
		    "Furball Air Quality Monitor",
		    mac_address,
		    "",
		    ro_ddcs,
		    wo_ddcs);

  homebus_setup();
  Serial.println("[homebus]");
}

static boolean furball_air_update(char* buf, size_t buf_len) {
  if(!bme680.is_present())
    return false;

  snprintf(buf,
	   buf_len,
	   "{ \"temperature\": %.1f, \"humidity\": %.1f, \"pressure\": %.1f }",
#ifdef TEMPERATURE_ADJUSTMENT
	   bme680.temperature() + TEMPERATURE_ADJUSTMENT,
#else
	   bme680.temperature(),
#endif
	   bme680.humidity(), bme680.pressure());

#ifdef VERBOSE
  Serial.println(buf);
#endif

  return true;
}

static boolean furball_air_quality_update(char* buf, size_t buf_len) {
  if(!bme680.is_present() && !pms5003.is_present())
    return false;

  uint16_t pm1 = pms5003.density_1_0();
  uint16_t pm25 = pms5003.density_2_5();
  uint16_t pm10 = pms5003.density_10_0();

  if(pm1 > 10000 && uptime.uptime() < 60*1000)
    pm1 = 0;

  if(pm25 > 10000 && uptime.uptime() < 60*1000)
    pm25 = 0;

  if(pm10 > 10000 && uptime.uptime() < 60*1000)
    pm10 = 0;

  snprintf(buf,
	   buf_len,
	   "{ \"tvoc\": %0.2f, \"pm1\": %d, \"pm25\": %d, \"pm10\": %d }",
	   bme680.gas_resistance(), pm1, pm25, pm10);

#ifdef VERBOSE
  Serial.println(buf);
#endif

  return true;
}

static boolean furball_aqi_update(char* buf, size_t buf_len) {
  if(!pms5003.is_present())
    return false;

  uint16_t pm25 = pms5003.density_2_5();
  uint16_t pm10 = pms5003.density_10_0();

  if(pm25 > 10000 && uptime.uptime() < 60*1000)
    pm25 = 0;

  if(pm10 > 10000 && uptime.uptime() < 60*1000)
    pm10 = 0;

  unsigned aqi_value = aqi(pm25, pm10);

  snprintf(buf,
	   buf_len,
	   "{ \"aqi\": %u, \"condition\": \"%s\", \"condition_index\": %d }",
	   aqi_value, aqi_condition_name(aqi_value), aqi_index(aqi_value));

#ifdef VERBOSE
  Serial.println(buf);
#endif

  return true;
}


static boolean furball_light_update(char* buf, size_t buf_len) {
  if(!tsl2561.is_present())
    return false;

  snprintf(buf,
	   buf_len,
	   "{  \"lux\": %d, \"full_light\": %d, \"ir\": %d, \"visible\": %d }",
	   tsl2561.lux(), tsl2561.full(), tsl2561.ir(), tsl2561.visible());

#ifdef VERBOSE
  Serial.println(buf);
#endif

  return true;
}

// the sound and presence sensors are currently unused in the furball air quality monitor
#if 0
static boolean furball_sound_update(char* buf, size_t buf_len) {
  snprintf(buf,
	   buf_len,
	   "{ \"average\": %d, \"min\": %d, \"max\": %d, \"samples\": %d }",
	   sound_level.sound_level(), sound_level.sound_min(), sound_level.sound_max(), sound_level.sample_count()
	   );

#ifdef VERBOSE
  Serial.println(buf);
#endif

  return true;
}

static boolean furball_presence_update(char* buf, size_t buf_len) {
  snprintf(buf,
	   buf_len,
	   "{ \"presence\": %s }",
	   pir.presence() ? "true" : "false"
	   );

#ifdef VERBOSE
  Serial.println(buf);
#endif

  return true;
}
#endif

/*
 * we do this once at startup, and not again unless our IP address changes
 */
static boolean furball_system_update(char* buf, size_t buf_len) {
  static IPAddress oldIP = IPAddress(0, 0, 0, 0);
  static String mac_address = WiFi.macAddress();
  IPAddress local = WiFi.localIP();

  if(oldIP == local)
    return false;

  snprintf(buf,
	   buf_len,
	   "{ \"name\": \"%s\", \"platform\": \"%s\", \"build\": \"%s\", \"ip\": \"%d.%d.%d.%d\", \"mac_addr\": \"%s\" }",
	   App.hostname().c_str(), "furball", App.build_info().c_str(), local[0], local[1], local[2], local[3], mac_address.c_str()
	   );

#ifdef VERBOSE
  Serial.println(buf);
#endif

  return true;
}

static boolean furball_diagnostic_update(char* buf, size_t buf_len) {
  snprintf(buf, buf_len, "{ \"freeheap\": %d, \"uptime\": %lu, \"rssi\": %d, \"reboots\": %d, \"wifi_failures\": %d }",
	   ESP.getFreeHeap(), uptime.uptime()/1000, WiFi.RSSI(), App.boot_count(), App.wifi_failures());

#ifdef VERBOSE
  Serial.println(buf);
#endif

  return true;
}


void furball_loop() {
  static unsigned long next_loop = 0;

  display_loop();

  if(next_loop > millis())
    return;

  next_loop = millis() + UPDATE_DELAY;

  bme680.handle();
  tsl2561.handle();
  pms5003.handle();

#if 0
  sound_level.handle();
  pir.handle();
#endif

  #define BUFFER_LENGTH 700
  char buffer[BUFFER_LENGTH + 1];

  if(furball_air_update(buffer, BUFFER_LENGTH)) {
#ifdef MQTT_OVERRIDE_TOPIC_AIR_SENSOR
    homebus_publish_to("org.homebus.experimental.air-sensor", buffer);
#else
    homebus_publish_to("org.homebus.experimental.air-sensor", buffer);
#endif
  }

  if(furball_air_quality_update(buffer, BUFFER_LENGTH)) {
#ifdef MQTT_OVERRIDE_TOPIC_AIR_SENSOR
    homebus_publish_to("org.homebus.experimental.air-quality-sensor", buffer);
#else
    homebus_publish_to("org.homebus.experimental.air-quality-sensor", buffer);
#endif
  }

  if(furball_aqi_update(buffer, BUFFER_LENGTH)) {
#ifdef MQTT_OVERRIDE_TOPIC_AIR_SENSOR
    homebus_publish_to("org.homebus.experimental.aqi-pm25", buffer);
#else
    homebus_publish_to("org.homebus.experimental.aqi-pm25", buffer);
#endif
  }

  if(furball_light_update(buffer, BUFFER_LENGTH)) {
#ifdef MQTT_OVERRIDE_TOPIC_AIR_SENSOR
    homebus_publish_to("org.homebus.experimental.light-sensor", buffer);
#else
    homebus_publish_to("org.homebus.experimental.light-sensor", buffer);
#endif
  }

#if 0
  if(furball_sound_update(buffer, BUFFER_LENGTH)) {
    homebus_publish_to("org.homebus.experimental.sound-sensor", buffer);
  }

  if(furball_presence_update(buffer, BUFFER_LENGTH)) {
    homebus_publish_to("org.homebus.experimental.presence-sensor", buffer);
  }
#endif

  if(furball_system_update(buffer, BUFFER_LENGTH)) {
#ifdef MQTT_OVERRIDE_TOPIC_AIR_SENSOR
    homebus_publish_to("org.homebus.experimental.system", buffer);
#else
    homebus_publish_to("org.homebus.experimental.system", buffer);
#endif
  }

  if(furball_diagnostic_update(buffer, BUFFER_LENGTH)) {
#ifdef MQTT_OVERRIDE_TOPIC_AIR_SENSOR
    homebus_publish_to("org.homebus.experimental.diagnostic", buffer);
#else
    homebus_publish_to("org.homebus.experimental.diagnostic", buffer);
#endif
  }
}

/* 
 * this callback is used to stream sensor data for diagnostics
 */
#ifdef USE_DIAGNOSTICS
void furball_stream() {
  static uint8_t count = 0;

  if(count == 0) {
    Serial.println("TEMP PRES HUMD TVOC   IR VISB FULL  LUX  1.0  2.5 10.0  SMAX  SMIN  SAVG  SCNT  PIR");
  }

  if(++count == 10) {
    count = 0;
  }

  bme680.handle();
  tsl2561.handle();
  pms5003.handle();
  sound_level.handle();

  Serial.printf( "%03.1f %4.0f %4.0f %4.0f %4d %4d %4d %4d %4d %4d %4d %5d %5d %5d %5d    %c\n",
		 bme680.temperature(),
		 bme680.pressure(),
		 bme680.humidity(),
		 bme680.gas_resistance(),
		 tsl2561.ir(),
		 tsl2561.visible(),
		 tsl2561.full(),
		 tsl2561.lux(),
		 pms5003.density_1_0(),
		 pms5003.density_2_5(),
		 pms5003.density_10_0(),
		 sound_level.sound_max(),
		 sound_level.sound_min(),
		 sound_level.sound_level(),
		 sound_level.sample_count(),
		 pir.presence() ? '1' : '0');

  if(0) {
  Serial.println("[system]");
  Serial.printf("  Uptime %.2f seconds\n", uptime.uptime() / 1000.0);
  Serial.printf("  Free heap %u bytes\n", ESP.getFreeHeap());
  Serial.printf("  Wifi RSSI %d\n", WiFi.RSSI());
  }
}
#endif
