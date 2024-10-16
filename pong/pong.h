#ifndef GAME_H
#define GAME_H

extern const int button_A;
extern const int button_B;
extern const int button_C;
extern const int button_D;

extern VGA vga;
extern MODE mode;
extern GFXWrapper<VGA> gfx;

extern int screenWidth;
extern int screenHeight;

void initializeGame();
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