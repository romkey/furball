#include <Arduino.h>

#include <multiball/wifi.h>

#include <U8g2lib.h>

#include "furball.h"
#include "display.h"
#include "aqi.h"

#include "config.h"


// U8G2_SSD1309_128X64_NONAME2_F_HW_I2C display(U8G2_R0);
static U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0);

static boolean is_present = false;

static display_screen_t screen_types[] = DISPLAY_SCREENS;

boolean display_begin() {
  is_present = display.begin();
  if(!is_present)
    return false;

  display.clearDisplay();
  display.drawBox(0, 10, 0, 10);
  //  display.setFont(u8g2_font_ncenB14_tr);
  display.setFont(u8g2_font_VCR_OSD_mf);
  display.sendBuffer();

  return true;
}

/*
 * code for different screen types
 * invoked from display_handle() at the end of the file
 * display_handle() needs to be called from the loop
 * in this case, furball_loop() will call it to keep the code in main more generic
 */
static void display_screen_message() {
  display.drawStr(0, 20, DISPLAY_SCREEN_MESSAGE_TEXT);
}

static void display_screen_time() {
  #define TIME_BUFFER_LENGTH 9
  char time_buffer[TIME_BUFFER_LENGTH];
  struct tm timeinfo;

  if(!getLocalTime(&timeinfo)) {
    display.drawStr(0, 20, "time fail");
    return;
  }

  snprintf(time_buffer, TIME_BUFFER_LENGTH, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  display.drawStr(10, 20, time_buffer);
}

static void display_screen_temperature() {
  display.setCursor(10, 20);
  display.print(bme680.temperature());
  display.drawStr(10, 40, "degrees C");
}

static void display_screen_temperature_f() {
  display.setCursor(10, 20);
  display.print((int)(bme680.temperature()*9.0/5 + 32));
  display.drawStr(10, 40, "degrees F");
}

static void display_screen_humidity() {
  display.setCursor(10, 20);
  display.print(bme680.humidity());
  display.drawStr(10, 40, "% humidity");
}

static void display_screen_pressure() {
  display.setCursor(10, 20);
  display.print(bme680.pressure());
  display.drawStr(10, 40, "pressure");
}

static void display_screen_voc() {
  display.setCursor(10, 20);
  display.print(bme680.gas_resistance());
  display.drawStr(10, 40, "voc");
}

static void display_screen_pm1() {
  display.setCursor(10, 20);
  display.print(pms5003.density_1_0());
  display.drawStr(10, 40, "PM1");
}

static void display_screen_pm2_5() {
  display.setCursor(10, 20);
  display.print(pms5003.density_2_5());
  display.drawStr(10, 40, "PM2.5");
}

static void display_screen_pm10() {
  display.setCursor(10, 20);
  display.print(pms5003.density_10_0());
  display.drawStr(10, 40, "PM10");
}

static void display_screen_lux() {
  display.setCursor(10, 20);
  display.print(tsl2561.lux());
  display.drawStr(10, 40, "lux");
}

static void display_screen_infrared() {
  display.setCursor(10, 20);
  display.print(tsl2561.ir());
  display.drawStr(10, 40, "ir");
}

static void display_screen_ip_address() {
  IPAddress local = WiFi.localIP();
#define IP_ADDRESS_STR_LENGTH 3*4+3+1
  char ip_address_str[IP_ADDRESS_STR_LENGTH];

  snprintf(ip_address_str, IP_ADDRESS_STR_LENGTH, "%d.%d.%d.%d", local[0], local[1], local[2], local[3]);
  display.drawStr(10, 20, ip_address_str);
}

static void display_screen_all_air() {
  char buf[12];

  snprintf(buf, 12, "C  %d", (int)bme680.temperature());
  display.drawStr(0, 15, buf);
  snprintf(buf, 12, "mg %d", (int)bme680.pressure());
  display.drawStr(0, 40, buf);
  snprintf(buf, 12, "%%  %d", (int)bme680.humidity());
  display.drawStr(0, 60, buf);
}

static void display_screen_all_air_f() {
  char buf[12];

  snprintf(buf, 12, "F  %d", (int)(bme680.temperature()*9.0/5 + 32));
  display.drawStr(0, 15, buf);
  snprintf(buf, 12, "mg %d", (int)bme680.pressure());
  display.drawStr(0, 40, buf);
  snprintf(buf, 12, "%%  %d", (int)bme680.humidity());
  display.drawStr(0, 60, buf);
}

static void display_screen_all_light() {
  char buf[12];

  snprintf(buf, 12, "lx %d", tsl2561.lux());
  display.drawStr(0, 15, buf);
  snprintf(buf, 12, "ir %d", tsl2561.ir());
  display.drawStr(0, 40, buf);
}

static void display_screen_all_particle() {
  char buf[12];

  snprintf(buf, 12, "1   %d", pms5003.density_1_0());
  display.drawStr(0, 15, buf);
  snprintf(buf, 12, "2.5 %d", pms5003.density_2_5());
  display.drawStr(0, 40, buf);
  snprintf(buf, 12, "10  %d", pms5003.density_10_0());
  display.drawStr(0, 60, buf);
}

static void display_screen_all_off() {
  display.setDrawColor(0);
  display.drawBox(0, 0, 127, 63);
}

static void display_screen_all_on() {
  display.setDrawColor(1);
  display.drawBox(0, 0, 127, 63);
}

static void display_screen_aqi() {
  unsigned aqi_value = aqi(pms5003.density_2_5(), pms5003.density_10_0());

  display.setCursor(0, 20);
  display.print(aqi_value);
  display.drawStr(0, 40, "AQI");
  display.drawStr(0, 60, aqi_condition_name(aqi_value));
}

static void display_screen_poop() {
  extern const uint8_t poop_bits[];

  display.drawXBM(0, 0, 128, 64, poop_bits);
}

static void display_screen_user_bitmap1() {
  extern const uint8_t user_bitmap1_bits[];

  display.drawXBM(0, 0, 128, 64, user_bitmap1_bits);
}

static void display_screen_user_bitmap2() {
  extern const uint8_t user_bitmap2_bits[];

  display.drawXBM(0, 0, 128, 64, user_bitmap2_bits);
}

static void display_screen_user_bitmap3() {
  extern const uint8_t user_bitmap3_bits[];

  display.drawXBM(0, 0, 128, 64, user_bitmap3_bits);

}

static void display_screen_rbg() {
  extern const uint8_t rbg_bits[];

  display.drawXBM(0, 0, 128, 64, rbg_bits);
}


// these MUST be in the same sequence as the enum screen_type_t
// you must not skip any
static void (*screen_handlers[])() = {
  display_screen_message,
  display_screen_time,
  display_screen_temperature,
  display_screen_temperature_f,
  display_screen_humidity,
  display_screen_pressure,
  display_screen_voc,
  display_screen_pm1,
  display_screen_pm2_5,
  display_screen_pm10,
  display_screen_lux,
  display_screen_infrared,
  display_screen_ip_address,
  display_screen_all_air,
  display_screen_all_air_f,
  display_screen_all_light,
  display_screen_all_particle,
  display_screen_all_off,
  display_screen_all_on,
  display_screen_aqi,
  display_screen_poop,
  display_screen_user_bitmap1,
  display_screen_user_bitmap2,
  display_screen_user_bitmap3,
  display_screen_rbg,
};

void display_loop() {
  static int current_screen_index = -1;
  static unsigned long next_screen_change = 0;

  if(millis() > next_screen_change) {
    next_screen_change = millis() + DISPLAY_SCREEN_INTERVAL;
    if(++current_screen_index == sizeof(screen_types)/sizeof(display_screen_t))
      current_screen_index = 0;
  }

  display.clearBuffer();
  //  display.setFont(u8g2_font_ncenB14_tr);
  display.setFont(u8g2_font_VCR_OSD_mf);
  display.setFontMode(0); // not transparent
  display.setDrawColor(1);

  (*screen_handlers[screen_types[current_screen_index]])();
  display.sendBuffer();
}
