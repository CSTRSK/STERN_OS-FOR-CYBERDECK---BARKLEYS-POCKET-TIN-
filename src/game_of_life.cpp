#include "game_of_life.h"
#include <TFT_eSPI.h>
#include "keypad.h"

extern TFT_eSPI tft;
extern KeypadManager Keypad;

// ========== НАСТРОЙКИ ==========
#define BAR_HEIGHT 0
#define GAME_DURATION_MS 30000
// =================================

// ---------- Размеры поля ----------
#define MAX_CELL_SIZE 2
#define MAX_GRID_W 160
#define MAX_GRID_H ( 240 / MAX_CELL_SIZE )

int currentCellSize = 3;
int GRID_W = 320 / currentCellSize;
int GRID_H = 240 / currentCellSize;

bool grid[MAX_GRID_H][MAX_GRID_W];
bool newGrid[MAX_GRID_H][MAX_GRID_W];

#define HASH_HISTORY 16
uint32_t hashHistory[HASH_HISTORY];
int hashIndex = 0;

bool gameRunning = false;
unsigned long lastGeneration = 0;
int generationDelay = 100;

uint16_t cellColor = TFT_RED;
const uint16_t colors[] = { TFT_RED, TFT_YELLOW, TFT_ORANGE, TFT_WHITE };
int colorIndex = 0;

// ---- Хеш и циклы ----
uint32_t computeHash() {
  uint32_t h = 0;
  for (int y = 0; y < GRID_H; y++) {
    for (int x = 0; x < GRID_W; x++) {
      h = h * 31 + grid[y][x];
    }
  }
  return h;
}

bool isCycle(uint32_t hash) {
  for (int i = 0; i < HASH_HISTORY; i++) {
    if (hashHistory[i] == hash) return true;
  }
  return false;
}

void pushHash(uint32_t hash) {
  hashHistory[hashIndex] = hash;
  hashIndex = (hashIndex + 1) % HASH_HISTORY;
}

void clearHashHistory() {
  for (int i = 0; i < HASH_HISTORY; i++) hashHistory[i] = 0;
  hashIndex = 0;
}

// ---- Логика игры ----
int countNeighbors(int x, int y) {
  int count = 0;
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dx == 0 && dy == 0) continue;
      int nx = (x + dx + GRID_W) % GRID_W;
      int ny = (y + dy + GRID_H) % GRID_H;
      if (grid[ny][nx]) count++;
    }
  }
  return count;
}

void drawCell(int x, int y) {
  int px = x * currentCellSize;
  int py = y * currentCellSize;
  uint16_t color = grid[y][x] ? cellColor : TFT_BLACK;
  tft.fillRect(px, py, currentCellSize, currentCellSize, color);
}

void drawFullGrid() {
  tft.fillRect(0, 0, 320, 240, TFT_BLACK);
  for (int y = 0; y < GRID_H; y++) {
    for (int x = 0; x < GRID_W; x++) {
      drawCell(x, y);
    }
  }
}

void randomizeGrid() {
  for (int y = 0; y < GRID_H; y++) {
    for (int x = 0; x < GRID_W; x++) {
      grid[y][x] = random(2);
    }
  }
  drawFullGrid();
}

void changeCellSize(int delta) {
  int newSize = currentCellSize + delta;
  if (newSize < 2) newSize = 2;
  if (newSize > 6) newSize = 6;
  if (newSize == currentCellSize) return;

  currentCellSize = newSize;
  GRID_W = 320 / currentCellSize;
  GRID_H = 240 / currentCellSize;

  for (int y = 0; y < GRID_H; y++) {
    for (int x = 0; x < GRID_W; x++) {
      grid[y][x] = random(2);
    }
  }
  clearHashHistory();
  drawFullGrid();
}

void changeColor() {
  colorIndex = (colorIndex + 1) % 4;
  cellColor = colors[colorIndex];
  for (int y = 0; y < GRID_H; y++) {
    for (int x = 0; x < GRID_W; x++) {
      if (grid[y][x]) drawCell(x, y);
    }
  }
}

void nextGeneration() {
  for (int y = 0; y < GRID_H; y++) {
    for (int x = 0; x < GRID_W; x++) {
      int n = countNeighbors(x, y);
      if (grid[y][x]) {
        newGrid[y][x] = (n == 2 || n == 3);
      } else {
        newGrid[y][x] = (n == 3);
      }
    }
  }

  bool anyChange = false;
  for (int y = 0; y < GRID_H; y++) {
    for (int x = 0; x < GRID_W; x++) {
      if (newGrid[y][x] != grid[y][x]) {
        grid[y][x] = newGrid[y][x];
        drawCell(x, y);
        anyChange = true;
      }
    }
  }

  if (!anyChange) {
    randomizeGrid();
    clearHashHistory();
    return;
  }

  uint32_t currentHash = computeHash();
  if (isCycle(currentHash)) {
    randomizeGrid();
    clearHashHistory();
    return;
  }
  pushHash(currentHash);
}

// ---------- ГЛАВНАЯ ФУНКЦИЯ ИГРЫ ----------
void runGameOfLife() {
  Serial.println("runGameOfLife started");
  randomSeed(esp_random());

  currentCellSize = 3;
  GRID_W = 320 / currentCellSize;
  GRID_H = 240 / currentCellSize;
  randomizeGrid();
  clearHashHistory();
  gameRunning = true;
  generationDelay = 100;
  lastGeneration = millis();
  colorIndex = 0;
  cellColor = TFT_RED;

  unsigned long startTime = millis();
  bool exitGame = false;

  while (!exitGame) {
    // Проверяем кнопку (нажатие, а не отпускание)
    if (Keypad.available() > 0) {
      int k = Keypad.getEvent();
      bool pressed = k & 0x80;
      k &= 0x7F;
      k--;
      uint8_t row = k / 10;
      uint8_t col = k % 10;

      Serial.printf("Game loop event: row=%d col=%d pressed=%d\n", row, col, pressed);

      // Выходим по нажатию row0-col0 (ESC)
      if (row == 0 && col == 0 && pressed) {
        exitGame = true;
        gameRunning = false;
        Serial.println("Exit by button press");
        Keypad.flush();
        break;
      }
    }

    if (millis() - startTime >= GAME_DURATION_MS) {
      exitGame = true;
      gameRunning = false;
      Serial.println("Exit by timer");
      break;
    }

    if (gameRunning && (millis() - lastGeneration >= generationDelay)) {
      lastGeneration = millis();
      nextGeneration();
    }

    delay(20);
  }

  tft.fillScreen(TFT_WHITE);
  gameRunning = false;
  Serial.println("runGameOfLife finished, gameRunning = false");
}