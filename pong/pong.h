#ifndef GAME_H
#define GAME_H

extern const int button_A;
extern const int button_B;
extern const int button_C;
extern const int button_D;

#include "ESP32S3VGA.h"
#include <GfxWrapper.h>

extern VGA vga;
extern Mode mode;
extern GfxWrapper<VGA> gfx;
extern const PinConfig pins;

extern int screenWidth;
extern int screenHeight;

void setUpPong();
void updateGame();
void movePaddle1Up();
void movePaddle1Down();
void movePaddle2Up();
void movePaddle2Down();
void drawPaddles();
void resetBall();
void ballMovement();
void drawScoreboard();
void drawBall();
void drawScreen();
void gameOver();

#endif
