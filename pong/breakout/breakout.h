#ifndef BREAKOUT_H
#define BREAKOUT_H

#include "ESP32S3VGA.h"
#include <GfxWrapper.h>

extern VGA vga;
extern GfxWrapper<VGA> gfx; 
extern int button;

extern int screenWidth;
extern int screenHeight;
 
// Declare setup and update functions for Breakout
void setupBreakout();
void updateBreakoutGame();
void drawBreakoutElements();
void drawPaddle();
void drawBall();
void movePaddleLeft();
void movePaddleRight();
void initializeBricks();
void drawBricks();
void checkBallBrickCollision();
void drawScore();

#endif // BREAKOUT_H
