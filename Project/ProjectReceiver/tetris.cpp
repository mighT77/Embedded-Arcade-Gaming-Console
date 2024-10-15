#include "tetris.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;  // External TFT object

// Game variables
int grid[10][19];     // Game grid (10 columns x 20 rows)
Block currentBlock;   // Current tetromino block
Point currentPos;     // Current position of the tetromino
int rot = 0;          // Rotation state of the block
bool gameOverTetris = false;
int score = 0;        // Game score

// Block colors (1: I, 2: O, 3: L, etc.)
uint16_t blockColors[8] = {TFT_BLACK, TFT_CYAN, TFT_YELLOW, TFT_BLUE, TFT_GREEN, TFT_ORANGE, TFT_RED, TFT_PURPLE};

// Tetromino shapes and rotations
Block blocks[7] = {
    // I Shape
    {{{{-1, 0}, {0, 0}, {1, 0}, {2, 0}},
      {{0, -1}, {0, 0}, {0, 1}, {0, 2}},
      {{-1, 1}, {0, 1}, {1, 1}, {2, 1}},
      {{1, -1}, {1, 0}, {1, 1}, {1, 2}}},
     4, 1},

    // O Shape
    {{{{0, 0}, {1, 0}, {0, 1}, {1, 1}}}, 1, 2},

    // L Shape
    {{{{-1, 0}, {0, 0}, {1, 0}, {1, 1}},
      {{0, -1}, {0, 0}, {0, 1}, {1, -1}},
      {{-1, -1}, {-1, 0}, {0, 0}, {1, 0}},
      {{-1, 1}, {0, -1}, {0, 0}, {0, 1}}},
     4, 3},

    // J Shape
    {{{{-1, 0}, {0, 0}, {1, 0}, {-1, 1}},
      {{0, -1}, {0, 0}, {0, 1}, {1, 1}},
      {{-1, 0}, {0, 0}, {1, 0}, {1, -1}},
      {{0, -1}, {0, 0}, {0, 1}, {-1, -1}}},
     4, 4},

    // S Shape
    {{{{0, 0}, {1, 0}, {-1, 1}, {0, 1}},
      {{0, -1}, {0, 0}, {1, 0}, {1, 1}}},
     2, 5},

    // Z Shape
    {{{{-1, 0}, {0, 0}, {0, 1}, {1, 1}},
      {{1, -1}, {0, 0}, {1, 0}, {0, 1}}},
     2, 6},

    // T Shape
    {{{{-1, 0}, {0, 0}, {1, 0}, {0, 1}},
      {{0, -1}, {0, 0}, {0, 1}, {1, 0}},
      {{0, -1}, {-1, 0}, {0, 0}, {1, 0}},
      {{-1, 0}, {0, -1}, {0, 0}, {0, 1}}},
     4, 7},
};

// Function to initialize the Tetris game
void setupTetris() {
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(0);
    clearGrid();
    drawGrid();
    score = 0;  // Initialize the score
    renderScore();  // Display the initial score
    spawnTetromino();
    gameOverTetris = false;
}

// Function to render the score on the screen
void renderScore() {
    tft.fillRect(0, 0, 160, 16, TFT_BLACK);  // Clear the score area
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 0);
    tft.printf("Score: %d", score);
}

// Function to clear the grid
void clearGrid() {
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 19; y++) {
            grid[x][y] = 0;  // 0 means empty cell
        }
    }
}

// Function to draw the grid
void drawGrid() {
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 19; y++) {
            uint16_t color = blockColors[grid[x][y]];
            tft.fillRect(x * 16, (y * 16) + 20, 16, 16, color);  // Offset for portrait
            tft.drawRect(x * 16, (y * 16) + 20, 16, 16, TFT_WHITE);  // Offset for portrait
        }
    }
}

// Function to spawn a new tetromino
void spawnTetromino() {
    currentBlock = blocks[random(0, 7)];
    currentPos.X = 4;  // Middle of the grid
    currentPos.Y = 0;
    rot = 0;
    if (checkCollision(currentPos, rot)) {
        gameOver();
    } else {
        drawTetromino();
    }
}

// Function to update the Tetris game state
void updateTetrisGame() {
    if (gameOverTetris) return;

    static unsigned long lastUpdate = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastUpdate >= 500) {
        lastUpdate = currentTime;
        Point nextPos = {currentPos.X, currentPos.Y + 1};

        if (!checkCollision(nextPos, rot)) {
            eraseTetromino();
            currentPos = nextPos;
            drawTetromino();
        } else {
            mergeTetromino();
            deleteLines();  // Call to update the score
            spawnTetromino();
        }
    }
}

// Function to handle user input
void handleTetrisInput(int button) {
    if (gameOverTetris) return;

    Point nextPos = currentPos;
    int nextRot = rot;

    if (button == 1) {
        // Move left
        nextPos.X -= 1;
    } else if (button == 2) {
        // Move right
        nextPos.X += 1;
    } else if (button == 3) {
        // Rotate
        nextRot = (rot + 1) % currentBlock.numRotate;
    } else if (button == 4) {
        // Soft drop
        nextPos.Y += 1;
    }

    if (!checkCollision(nextPos, nextRot)) {
        eraseTetromino();
        currentPos = nextPos;
        rot = nextRot;
        drawTetromino();
    }
}

// Function to check for collisions
bool checkCollision(Point pos, int rot) {
    Point squares[4];
    getSquares(currentBlock, pos, rot, squares);

    for (int i = 0; i < 4; i++) {
        int x = squares[i].X;
        int y = squares[i].Y;

        if (x < 0 || x >= 10 || y < 0 || y >= 19) {
            return true;  // Collision with walls or floor
        }
        if (grid[x][y] != 0) {
            return true;  // Collision with existing blocks
        }
    }
    return false;
}

// Function to get the squares occupied by the tetromino
bool getSquares(Block block, Point pos, int rot, Point* squares) {
    for (int i = 0; i < 4; i++) {
        squares[i].X = pos.X + block.square[rot][i].X;
        squares[i].Y = pos.Y + block.square[rot][i].Y;
    }
    return true;
}

// Function to draw the current tetromino
void drawTetromino() {
    Point squares[4];
    getSquares(currentBlock, currentPos, rot, squares);

    for (int i = 0; i < 4; i++) {
        int x = squares[i].X;
        int y = squares[i].Y;
        if (x >= 0 && x < 10 && y >= 0 && y < 19) {
            uint16_t color = blockColors[currentBlock.color];
            tft.fillRect(x * 16, (y * 16) + 20, 16, 16, color);  // Offset for portrait
            tft.drawRect(x * 16, (y * 16) + 20, 16, 16, TFT_WHITE);
        }
    }
}

// Function to erase the current tetromino
void eraseTetromino() {
    Point squares[4];
    getSquares(currentBlock, currentPos, rot, squares);

    for (int i = 0; i < 4; i++) {
        int x = squares[i].X;
        int y = squares[i].Y;
        if (x >= 0 && x < 10 && y >= 0 && y < 19) {
            tft.fillRect(x * 16, (y * 16) + 20, 16, 16, TFT_BLACK);  // Offset for portrait
            tft.drawRect(x * 16, (y * 16) + 20, 16, 16, TFT_WHITE);
        }
    }
}

// Function to merge the current tetromino into the grid
void mergeTetromino() {
    Point squares[4];
    getSquares(currentBlock, currentPos, rot, squares);

    for (int i = 0; i < 4; i++) {
        int x = squares[i].X;
        int y = squares[i].Y;
        if (x >= 0 && x < 10 && y >= 0 && y < 19) {
            grid[x][y] = currentBlock.color;
        }
    }
    drawGrid();
}

// Function to delete completed lines and update score
void deleteLines() {
    int linesCleared = 0;
    for (int y = 0; y < 19; y++) {
        bool lineComplete = true;
        for (int x = 0; x < 10; x++) {
            if (grid[x][y] == 0) {
                lineComplete = false;
                break;
            }
        }
        if (lineComplete) {
            linesCleared++;
            // Move all lines above down
            for (int ty = y; ty > 0; ty--) {
                for (int tx = 0; tx < 10; tx++) {
                    grid[tx][ty] = grid[tx][ty - 1];
                }
            }
            // Clear the top line
            for (int tx = 0; tx < 10; tx++) {
                grid[tx][0] = 0;
            }
            drawGrid();
        }
    }
    if (linesCleared > 0) {
        score += linesCleared * 100;  // Increase score by 100 per line cleared
        renderScore();  // Update score display
    }
}

// Function to handle game over
void gameOver() {
    gameOverTetris = true;
    tft.fillScreen(TFT_RED);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println("Game Over");
    tft.setCursor(10, 30);
    tft.printf("Final Score: %d", score);

    // Display countdown
    for (int i = 5; i > 0; i--) {
        tft.setCursor(10, 60);
        tft.printf("Returning to menu in %d", i);
        delay(1000);  // 1 second delay for countdown
        tft.fillRect(10, 60, 200, 20, TFT_RED);  // Clear previous countdown text
    }

    // Return to the menu screen (replace this with your actual menu function call)
    displayMenu();  // Assuming `displayMenu()` is the function to return to the menu
}
