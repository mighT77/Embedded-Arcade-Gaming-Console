#ifndef TETRIS_H
#define TETRIS_H

#include <TFT_eSPI.h>

// Data structures
struct Point {
    int X, Y;
};

struct Block {
    Point square[4][4];
    int numRotate, color;
};

// Function declarations
void setupTetris();
void updateTetrisGame();
void handleTetrisInput(int button);
void drawTetromino();
void eraseTetromino();
void spawnTetromino();
void clearGrid();
void drawGrid();
bool checkCollision(Point pos, int rot);
void mergeTetromino();
void deleteLines();
bool getSquares(Block block, Point pos, int rot, Point* squares);
void gameOver();
void renderScore();
void renderGameScore();
void displayMenu();

#endif  // TETRIS_H
