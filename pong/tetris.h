#ifndef TETRIS_H
#define TETRIS_H

#include "ESP32S3VGA.h" 
#include <GfxWrapper.h>  

struct Point {
    int X;
    int Y;
};

struct Block {
    Point square[4][4]; 
    int numRotate;      
    int color;          
};

void setupTetris();             // Initialize the Tetris game
void updateTetrisGame();        // Update the Tetris game state
void handleTetrisInput(int button); // Handle user input for Tetris
void renderScore();             // Render the current score on the screen
void drawGrid();                // Draw the Tetris grid
void spawnTetromino();          // Spawn a new tetromino
bool checkCollision(Point pos, int rot); // Check for collisions
void eraseTetromino();          // Erase the current tetromino
void drawTetromino();           // Draw the current tetromino
void mergeTetromino();          // Merge the current tetromino into the grid
void deleteLines();             // Delete completed lines from the grid
void gameOverT();                // Handle game over logic
void clearGrid();
bool getSquares(Block block, Point pos, int rot, Point* squares);

// extern int grid[10][19];        // Game grid (10 columns x 20 rows)
// extern Block currentBlock;      // Current tetromino block
// extern Point currentPos;        // Current position of the tetromino
// extern int rot;                 // Rotation state of the block
// extern bool gameOverTetris;     // Game over flag
// extern int score;               // Game score
// extern uint16_t blockColors[8]; // Block colors

#endif 