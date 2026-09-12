#ifndef BMP_H
#define BMP_H

#include <Arduino.h>

// Показать BMP-изображение (24-бит, без сжатия) с SD-карты
// Размер должен быть 320x240 (или будет масштабироваться? мы будем поддерживать только 320x240)
// Возвращает true, если файл успешно показан, иначе false
// Ждёт нажатия ESC для выхода
bool showBMPImage(const char* filename);

#endif