#pragma once

/*
 * optional display module
 * currently written to support an SSD1306 128x64 OLED module
 * we should generalize
 *
 * uses u8g2 graphics library, which supports a wide range of
 * hardware including some e-ink/e-paper devices
 *
 * https://github.com/olikraus/u8g2
 *
 * keep all display-related code isolated to this file
 * we don't want to have to hunt it down in order to update it
 */

boolean display_begin();
void display_loop();

typedef enum {
  DISPLAY_SCREEN_MESSAGE,
  DISPLAY_SCREEN_TIME,
  DISPLAY_SCREEN_TEMPERATURE,
  DISPLAY_SCREEN_HUMIDITY,
  DISPLAY_SCREEN_PRESSURE,
  DISPLAY_SCREEN_VOC,
  DISPLAY_SCREEN_PM1,
  DISPLAY_SCREEN_PM2_5,
  DISPLAY_SCREEN_PM10,
  DISPLAY_SCREEN_LUX,
  DISPLAY_SCREEN_INFRARED,
  DISPLAY_SCREEN_IP
} display_screen_t;
