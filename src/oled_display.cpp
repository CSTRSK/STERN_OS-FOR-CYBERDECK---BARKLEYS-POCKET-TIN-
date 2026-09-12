#include "oled_display.h"
#include <Wire.h>
#include "keypad.h"
#include "status_bar.h"

extern bool sdCardOK;
extern bool gameRunning;
extern bool editorActive;   // <-- добавлено

TwoWire oledWire = TwoWire(1);
Adafruit_SSD1306 display(128, 32, &oledWire, -1);
bool oledOK = false;
String lastKeyMessage = "Ready";

bool initOLED() {
  oledWire.begin(32, 25);
  oledWire.setClock(100000L);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
      Serial.println("SSD1306 NOT FOUND!");
      oledOK = false;
      return false;
    } else {
      oledOK = true;
    }
  } else {
    oledOK = true;
  }
  return oledOK;
}

void drawStandardOled() {
  if (!oledOK) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (editorActive) {
    // ===== Режим текстового редактора =====
    display.setCursor(0, 0);
    display.print("TXT MODE");
    display.println();
    display.print("Mods:");
    bool anyMod = false;
    if (Keypad.isShiftPressed()) { display.print(" SHIFT"); anyMod = true; }
    if (Keypad.isCtrlPressed())  { display.print(" CTRL"); anyMod = true; }
    if (Keypad.isAltPressed())   { display.print(" ALT"); anyMod = true; }
    if (Keypad.isFnPressed())    { display.print(" FN"); anyMod = true; }
    if (Keypad.isCapsLockOn())   { display.print(" CAPS"); anyMod = true; }
    if (!anyMod) display.print(" NONE");
  } else {
    // ===== Обычный режим =====
    display.setCursor(0, 0);
    display.print("Time: ");
    if (currentHour < 10) display.print("0");
    display.print(currentHour);
    display.print(":");
    if (currentMinute < 10) display.print("0");
    display.print(currentMinute);
    display.print(":");
    if (currentSecond < 10) display.print("0");
    display.print(currentSecond);

    display.println();
    display.print("Key: ");
    display.print(lastKeyMessage);

    display.println();
    display.print("G:");
    display.print(gameRunning ? "ON " : "OFF");
    display.print(" SD:");
    display.print(sdCardOK ? "ON" : "OFF");
    display.print(" W:");
    display.print(wifiConnected ? "ON" : "OFF");

    display.println();
    display.print("Mods:");
    bool anyMod = false;
    if (Keypad.isShiftPressed()) { display.print(" SHIFT"); anyMod = true; }
    if (Keypad.isCtrlPressed())  { display.print(" CTRL"); anyMod = true; }
    if (Keypad.isAltPressed())   { display.print(" ALT"); anyMod = true; }
    if (Keypad.isFnPressed())    { display.print(" FN"); anyMod = true; }
    if (Keypad.isCapsLockOn())   { display.print(" CAPS"); anyMod = true; }
    if (!anyMod) display.print(" NONE");
  }

  display.display();
}