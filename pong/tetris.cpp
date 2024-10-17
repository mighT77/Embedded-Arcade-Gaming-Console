#include "tetris.h"
#include "leaderboard.h"

extern VGA vga;
extern GfxWrapper<VGA> gfx;

int grid[10][14];     
Block currentBlock;  
Point currentPos;     
int rot = 0;          // Rotation state of the block
bool gameOverTetris = false;
int score = 0;        // Game score

// Block colors (1: I, 2: O, 3: L, etc.)
uint16_t blockColors[8] = {
    vga.rgb(0, 0, 0),       // Empty
    vga.rgb(0, 0x80, 0x80), // Cyan
    vga.rgb(0x80, 0x80, 0), // Yellow
    vga.rgb(0, 0, 0x80),    // Blue
    vga.rgb(0, 0x80, 0),    // Green
    vga.rgb(0x80, 0x40, 0), // Orange
    vga.rgb(0x80, 0, 0),    // Red
    vga.rgb(0x40, 0, 0x40)  // Purple
};

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
    Serial.println("Entered Tetris");
    gfx.fillScreen(vga.rgb(0, 0, 0)); // Clear the screen
    vga.show(); // Update the screen
    clearGrid();
    drawGrid();
    score = 0;  // Initialize the score
    renderScore();  // Display the initial score
    spawnTetromino();
    gameOverTetris = false;
}

// Function to render the score on the screen
void renderScore() {
    gfx.fillRect(0, 0, 160, 16, vga.rgb(0, 0, 0));  // Clear the score area
    gfx.setTextColor(vga.rgb(0x80, 0x80, 0x80));
    gfx.setTextSize(2);
    gfx.setCursor(170, 0);
    gfx.print("Score:");
    gfx.print(score);
    vga.show(); // Update the screen
}

// Function to clear the grid
void clearGrid() {
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 14; y++) {
            grid[x][y] = 0;  // 0 means empty cell
        }
    }
}

// Function to draw the grid
void drawGrid() {
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 14; y++) {
            uint16_t color = blockColors[grid[x][y]];
            gfx.fillRect(x * 16, (y * 16), 16, 16, color);  // Draw each block
            gfx.drawRect(x * 16, (y * 16), 16, 16, vga.rgb(0x80, 0x80, 0x80));  // Draw outline
        }
    }
    vga.show(); // Update the screen
}

// Function to spawn a new tetromino
void spawnTetromino() {
    currentBlock = blocks[random(0, 7)];
    currentPos.X = 4;  // Middle of the grid
    currentPos.Y = 0;
    rot = 0;
    if (checkCollision(currentPos, rot)) {
        gameOverT();
    } else {
        drawTetromino();
        vga.show(); // Update the screen after spawning a new tetromino
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
            eraseTetromino(); // Clear previous position
            drawGrid();
            currentPos = nextPos;
            drawTetromino();
              
        } else {
            mergeTetromino();
            deleteLines();  
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
        vga.show(); // Update the screen after erasing
        currentPos = nextPos;
        rot = nextRot;
        drawTetromino();
        vga.show(); // Update the screen after drawing
    }
}

// Function to check collision
bool checkCollision(Point pos, int rot) {
    Point squares[4];
    getSquares(currentBlock, pos, rot, squares);

    for (int i = 0; i < 4; i++) {
        int x = squares[i].X;
        int y = squares[i].Y;

        if (x < 0 || x >= 10 || y < 0 || y >= 14) {
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
        if (x >= 0 && x < 10 && y >= 0 && y < 14) {
            uint16_t color = blockColors[currentBlock.color];
            gfx.fillRect(x * 16, (y * 16), 16, 16, color);  
            gfx.drawRect(x * 16, (y * 16), 16, 16, vga.rgb(0x80, 0x80, 0x80));
        }
    }
    vga.show();
}

// Function to erase the current tetromino
void eraseTetromino() {
    Point squares[4];
    getSquares(currentBlock, currentPos, rot, squares);

    for (int i = 0; i < 4; i++) {
        int x = squares[i].X;
        int y = squares[i].Y;
        if (x >= 0 && x < 10 && y >= 0 && y < 14) {
            gfx.fillRect(x * 16, (y * 16), 16, 16, vga.rgb(0, 0, 0));  // Clear block
            gfx.drawRect(x * 16, (y * 16), 16, 16, vga.rgb(0x80, 0x80, 0x80));
        }
    }
    vga.show();
}

// Function to merge the current tetromino into the grid
void mergeTetromino() {
    Point squares[4];
    getSquares(currentBlock, currentPos, rot, squares);

    for (int i = 0; i < 4; i++) {
        int x = squares[i].X;
        int y = squares[i].Y;
        if (x >= 0 && x < 10 && y >= 0 && y < 14) {
            grid[x][y] = currentBlock.color;
        }
    }
    score += 10;  // Increment score for placing a block
    renderScore(); // Update score display
    drawGrid();
    vga.show(); // Update the screen
}

// Function to delete completed lines and update score
void deleteLines() {
    int linesCleared = 0;
    for (int y = 0; y < 14; y++) {
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
            vga.show(); // Update the screen after deleting lines
        }
    }
    if (linesCleared > 0) {
        score += linesCleared * 100;  // Increase score by 100 per line cleared
        renderScore();  // Update score display
        vga.show(); // Update the screen after updating the score
    }
}

// Function to handle game over
void gameOverT() {
    gameOverTetris = true;
    gfx.fillScreen(vga.rgb(0x80, 0, 0));  
    gfx.setTextColor(vga.rgb(0x80, 0x80, 0x80));
    gfx.setTextSize(2);
    gfx.setCursor(10, 10);
    gfx.print("Game Over");
    gfx.setCursor(10, 30);
    gfx.print("Final Score:");
    gfx.print(score);
    vga.show(); // Update the screen after game over

    delay(200);
    updateLeaderboard(score); // Update the leaderboard with the new score
    renderLeaderboard(); // Display the leaderboard
}
