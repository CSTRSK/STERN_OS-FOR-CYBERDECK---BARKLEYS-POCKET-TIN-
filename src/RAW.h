#ifndef RAW_H
#define RAW_H

#include <Arduino.h>

extern bool rawImageJustViewed;  // флаг, что только что было показано RAW-изображение

bool showRAWImage(const char* filename);

#endif