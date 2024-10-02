// snake.h
#ifndef SNAKE_H
#define SNAKE_H

#include <TFT_eSPI.h>

// Declare the TFT object as extern to avoid redefinition
extern TFT_eSPI tft;
extern TFT_eSprite sprite;

// Function declarations
void setupSnake();
void run();
void getFood();
void checkGameOver();
void loopSnake();
void handleSnakeInput(int button);
void updateSnakeGame();

#endif
