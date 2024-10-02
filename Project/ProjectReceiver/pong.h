#ifndef PONG_H
#define PONG_H

#include <TFT_eSPI.h>  // TFT library
#include "message_struct.h"  // Include message struct for button input

// Function declarations
void setupPong();
void updatePongGame();
void drawMidline();
void drawPaddles();
void drawBall();
void handlePongInput(int button);

#endif
