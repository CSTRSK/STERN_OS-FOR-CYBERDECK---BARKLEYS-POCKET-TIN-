#include "dino_game.h"
#include <Adafruit_SSD1306.h>
#include "keypad.h"
#include <stdlib.h>

extern Adafruit_SSD1306 display;

void runDinoGame() {

    const int GROUND_Y = 30;
    const int DINO_W = 6;
    const int DINO_H = 10;
    const int DINO_X = 10;
    const float GRAVITY = 0.9;
    const float JUMP_SPEED = -7.0;
    const int OBSTACLE_W = 4;
    const int OBSTACLE_H = 5;
    const int MAX_JUMPS = 2;


    const int MIN_OBSTACLE_INTERVAL = 30;   


    int level = 1;
    float obstacleSpeed = 1.0;
    unsigned long levelStartTime = millis();
    const unsigned long LEVEL_DURATION = 60000;
    const float SPEED_INCREMENT = 0.4;
    const int MAX_LEVEL = 10;

    float dinoY = GROUND_Y - DINO_H;
    float dinoVy = 0;
    bool jumping = false;
    int jumpsLeft = MAX_JUMPS;

    struct Obstacle { int x, w, h; };
    Obstacle obstacles[3];
    int obstacleCount = 0;
    int frameCounter = 0;
    int score = 0;
    bool gameOver = false;

    
    frameCounter = MIN_OBSTACLE_INTERVAL;

    while (!gameOver) {

        KeyCode code = Keypad.getKey();
        if (code != KEY_NONE && jumpsLeft > 0) {
            dinoVy = JUMP_SPEED;
            jumping = true;
            jumpsLeft--;
        }

  
        dinoVy += GRAVITY;
        dinoY += dinoVy;
        if (dinoY >= GROUND_Y - DINO_H) {
            dinoY = GROUND_Y - DINO_H;
            dinoVy = 0;
            jumping = false;
            jumpsLeft = MAX_JUMPS;
        }

        frameCounter++;
        if (frameCounter >= MIN_OBSTACLE_INTERVAL && obstacleCount < 3) {
            if (random(0, 100) < 30) {
                obstacles[obstacleCount].x = 128;
                obstacles[obstacleCount].w = OBSTACLE_W;
                obstacles[obstacleCount].h = OBSTACLE_H + random(-1, 3);
                obstacleCount++;
            }
            frameCounter = 0;
        }

        for (int i = 0; i < obstacleCount; i++) {
            obstacles[i].x -= (int)obstacleSpeed;
        }


        int newCount = 0;
        for (int i = 0; i < obstacleCount; i++) {
            if (obstacles[i].x + obstacles[i].w > 0) {
                if (newCount != i) obstacles[newCount] = obstacles[i];
                newCount++;
            }
        }
        obstacleCount = newCount;


        int dinoY_int = (int)dinoY;
        for (int i = 0; i < obstacleCount; i++) {
            int ox = obstacles[i].x;
            int oy = GROUND_Y - obstacles[i].h;
            if (DINO_X < ox + obstacles[i].w &&
                DINO_X + DINO_W > ox &&
                dinoY_int < oy + obstacles[i].h &&
                dinoY_int + DINO_H > oy) {
                gameOver = true;
                break;
            }
        }

        if (!gameOver) score++;


        unsigned long now = millis();
        if (now - levelStartTime >= LEVEL_DURATION) {
            if (level < MAX_LEVEL) {
                level++;
                obstacleSpeed += SPEED_INCREMENT;
            }
            levelStartTime = now;
        }


        display.clearDisplay();
        display.drawLine(0, GROUND_Y, 127, GROUND_Y, SSD1306_WHITE);
        display.fillRect(DINO_X, (int)dinoY, DINO_W, DINO_H, SSD1306_WHITE);
        for (int i = 0; i < obstacleCount; i++) {
            int oy = GROUND_Y - obstacles[i].h;
            display.fillRect(obstacles[i].x, oy, obstacles[i].w, obstacles[i].h, SSD1306_WHITE);
        }
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.print("Score: ");
        display.print(score);
        display.setCursor(80, 0);
        display.print("L:");
        display.print(level);
        display.display();

        delay(16);
    }

    // ---- Game Over ----
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Game Over");
    display.print("Score: ");
    display.println(score);
    display.print("Level reached: ");
    display.println(level);
    display.println("Press any key");
    display.display();

    while (true) {
        if (Keypad.getKey() != KEY_NONE) break;
        delay(50);
    }

    display.clearDisplay();
    display.display();
}