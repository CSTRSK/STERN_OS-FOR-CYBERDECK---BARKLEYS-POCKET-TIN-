#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Adafruit_SSD1306.h>
#include <Arduino.h>

extern Adafruit_SSD1306 display;
extern bool oledOK;
extern String lastKeyMessage;

bool initOLED();
void drawStandardOled();

#endif