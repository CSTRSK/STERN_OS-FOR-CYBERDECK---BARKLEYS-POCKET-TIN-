#include "nes_emulator.h"
#include <SD.h>
#include <TFT_eSPI.h>
#include <esp_timer.h>
#include "keypad.h"
#include "core/Bus.h"
#include "core/Cartridge.h"
#include "gui_settings.h"
#include <vector>

extern TFT_eSPI tft;
extern bool sdCardOK;
extern KeypadManager Keypad;

extern bool gameRunning;
extern bool nesActive;

extern void terminalPrint(String text);
extern void drawTerminal();
extern void drawStatusBar();

// Подключаем цвета из настроек
extern uint16_t globalTextColor;
extern uint16_t cmdTextColor;

// ===== Константы терминала (скопированы из terminal_manager.cpp) =====
const int TERMINAL_START_Y = 15;
const int LINE_HEIGHT = 10;
#define LEFT_MARGIN 10

// ===============================
// NES GAME LIST
// ===============================

std::vector<String> nesGames;
int nesSelected = 0;
int nesScrollOffset = 0;

// ===============================
// Загрузка списка игр
// ===============================

void loadNESGames()
{
    nesGames.clear();

    File root = SD.open("/");
    if (!root) return;

    File file = root.openNextFile();
    while (file)
    {
        String name = file.name();
        if (!file.isDirectory() && name.endsWith(".nes"))
        {
            if (!name.startsWith("/"))
                name = "/" + name;
            nesGames.push_back(name);
        }
        file.close();
        file = root.openNextFile();
    }
    root.close();
    nesSelected = 0;
    nesScrollOffset = 0;
}

// ===============================
// Экран выбора игр (без мерцания и с прокруткой)
// ===============================

void drawNESMenu()
{
    // НЕ ОЧИЩАЕМ ВСЮ ОБЛАСТЬ – каждая строка перерисовывается сама

    int y = TERMINAL_START_Y + 2;
    int count = nesGames.size();
    int maxLines = (240 - TERMINAL_START_Y) / LINE_HEIGHT - 1; // оставляем место для подсказки

    // Вычисляем, с какой позиции начинать показ
    int startIdx = 0;
    if (count > maxLines)
    {
        int half = maxLines / 2;
        if (nesSelected > half)
        {
            startIdx = nesSelected - half;
            if (startIdx > count - maxLines)
                startIdx = count - maxLines;
        }
    }

    tft.setTextSize(globalTextSize);

    for (int i = 0; i < maxLines; i++)
    {
        int idx = startIdx + i;
        if (idx >= count) break;

        String name = nesGames[idx];
        if (name.length() > 30)
            name = name.substring(0, 28) + "...";

        bool isSelected = (idx == nesSelected);

        // Цвета: инверсия для выделенной строки
        uint16_t textColor = isSelected ? TFT_BLACK : cmdTextColor;
        uint16_t bgColor = isSelected ? cmdTextColor : TFT_BLACK;

        // Заливаем фон строки с учётом отступа
        tft.fillRect(LEFT_MARGIN, y, 320 - LEFT_MARGIN * 2, LINE_HEIGHT, bgColor);
        tft.setTextColor(textColor, bgColor);
        tft.setCursor(LEFT_MARGIN + 4, y);
        tft.print(name);
        y += LINE_HEIGHT;
    }

    // Нижняя подсказка – приглушённый цвет
    tft.fillRect(0, 240 - LINE_HEIGHT, 320, LINE_HEIGHT, TFT_BLACK);
    tft.setTextColor(globalTextColor, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(LEFT_MARGIN, 240 - LINE_HEIGHT);
    tft.print("S:X:Move  ENTER:Run  ESC:Back");

    drawBorder();   // рамка (цвет уже задан)
}

// ===============================
// Запуск NES
// ===============================

void startNESGame(String romPath)
{
    nesActive = true;

    terminalPrint("Loading ROM:");
    terminalPrint(romPath);
    drawTerminal();

    File romFile = SD.open(romPath, FILE_READ);
    if (!romFile)
    {
        terminalPrint("ROM open error");
        drawTerminal();
        return;
    }

    if (romFile.size() < 16)
    {
        romFile.close();
        terminalPrint("Bad ROM");
        drawTerminal();
        return;
    }

    uint8_t header[16];
    romFile.read(header, 16);
    romFile.close();

    if (header[0] != 'N' ||
        header[1] != 'E' ||
        header[2] != 'S' ||
        header[3] != 0x1A)
    {
        terminalPrint("Invalid NES ROM");
        drawTerminal();
        return;
    }

    Cartridge* cart = new Cartridge(romPath.c_str());
    if (!cart) return;

    Bus* nes = new Bus();
    if (!nes)
    {
        delete cart;
        return;
    }

    nes->insertCartridge(cart);
    nes->connectScreen(&tft);
    tft.setSwapBytes(true);
    nes->reset();

    gameRunning = true;

    // Целевое время кадра для NTSC: 60.098 FPS
    const uint32_t FRAME_TIME_US = 16639;
    uint64_t next_frame = esp_timer_get_time();

    bool running = true;
    while (running)
    {
        uint8_t buttons = 0;

        // Опрос кнопок (исходный порядок)
        if (Keypad.isPressed(KEY_SEMICOLON))
            buttons |= 0x01;                     // A (прыжок) — ;

        if (Keypad.isPressed(KEY_QUESTION_SLASH))
            buttons |= 0x02;                     // B (стрельба) — ?

        if (Keypad.isPressed(KEY_ENTER))
            buttons |= 0x04;                     // SELECT — Enter

        if (Keypad.isPressed(KEY_SPACE))
            buttons |= 0x08;                     // START — Space

        if (Keypad.isPressed(KEY_S))
            buttons |= 0x10;                     // UP — S

        if (Keypad.isPressed(KEY_X))
            buttons |= 0x20;                     // DOWN — X

        if (Keypad.isPressed(KEY_Z))
            buttons |= 0x40;                     // LEFT — Z

        if (Keypad.isPressed(KEY_C))
            buttons |= 0x80;                     // RIGHT — C

        nes->controller = buttons;

        // Проверка ESC
        KeyCode code = Keypad.getKey();
        if (code == KEY_ESC)
        {
            running = false;
            break;
        }

        // Эмуляция одного кадра
        nes->clock();

        // Поддержание скорости 60 FPS
        uint64_t now = esp_timer_get_time();
        int64_t diff = (int64_t)next_frame - (int64_t)now;
        if (diff > 0)
        {
            ets_delay_us(diff);
        }
        next_frame += FRAME_TIME_US;
    }

    gameRunning = false;
    nesActive = false;

    delete nes;
    delete cart;

    tft.fillScreen(TFT_BLACK);
    drawStatusBar();
    terminalPrint("NES stopped.");
    drawTerminal();
}

// ===============================
// Команда nes (с прокруткой и без мерцания)
// ===============================

void runNES()
{
    if (!sdCardOK)
    {
        terminalPrint("SD not found");
        drawTerminal();
        return;
    }

    loadNESGames();

    if (nesGames.size() == 0)
    {
        terminalPrint("No NES games");
        drawTerminal();
        return;
    }

    // Очищаем область терминала ОДИН РАЗ при входе
    tft.fillRect(0, TERMINAL_START_Y, 320, 240 - TERMINAL_START_Y, TFT_BLACK);

    bool menu = true;
    bool redraw = true;

    while (menu)
    {
        if (redraw)
        {
            drawNESMenu();
            redraw = false;
        }

        KeyCode code = Keypad.getKey();

        if (code != KEY_NONE)
        {
            switch (code)
            {
            case KEY_S:
                if (nesSelected > 0)
                {
                    nesSelected--;
                    redraw = true;
                }
                break;

            case KEY_X:
                if (nesSelected < (int)nesGames.size() - 1)
                {
                    nesSelected++;
                    redraw = true;
                }
                break;

            case KEY_ENTER:
                menu = false;
                startNESGame(nesGames[nesSelected]);
                break;

            case KEY_ESC:
                menu = false;
                tft.fillScreen(TFT_BLACK);
                drawTerminal();
                drawBorder();
                break;

            default:
                break;
            }
        }

        delay(10);
    }
}