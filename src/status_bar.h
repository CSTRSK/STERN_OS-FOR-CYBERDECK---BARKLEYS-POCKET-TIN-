#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <Arduino.h>
#include "gui_settings.h"

extern unsigned long lastSecond;
extern int currentHour;
extern int currentMinute;
extern int currentSecond;
extern char oldTimeStr[9];
extern bool wifiConnected;
extern int timeStartX;
extern int timeWidth;
extern int wifiIconX;

void drawStatusBar();
void updateTimeOnStatusBar();
void updateWifiIcon();
void updateTimeDisplay();

#endif