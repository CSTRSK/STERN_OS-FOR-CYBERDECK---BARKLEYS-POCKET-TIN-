#ifndef GUI_SETTINGS_H
#define GUI_SETTINGS_H

#include <Arduino.h>
#include <stdint.h>

struct GuiSettings {
  uint8_t globalTextSize;
  uint16_t globalTextColor;
  uint16_t globalTextBgColor;
  uint8_t cmdTextSize;
  uint16_t cmdTextColor;
  uint16_t cmdTextBgColor;
};

extern GuiSettings guiSettings;
extern uint8_t globalTextSize;
extern uint16_t globalTextColor;
extern uint16_t globalTextBgColor;
extern uint8_t cmdTextSize;
extern uint16_t cmdTextColor;
extern uint16_t cmdTextBgColor;

void loadGuiSettings();
void saveGuiSettings();
void guiSettingsMenu();

// Новая функция для рисования рамки
void drawBorder();

#endif