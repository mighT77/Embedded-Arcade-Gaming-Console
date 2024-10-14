#include <TFT_eSPI.h>
#include "message_struct.h"  // Include the struct_message definition

extern TFT_eSPI tft;  // External TFT object

// Snake variables
int snakeX[100], snakeY[100];  // Arrays to store snake segments
int snakeLength = 5;           // Initial length of the snake
int dirX = 1, dirY = 0;        // Snake direction (initially moving right)
int foodX, foodY;              // Food position
bool gameOverSnake = false;
int screenWidth = 170;
int screenHeight = 320;

// Timing variables for the Snake game
unsigned long lastSnakeUpdate = 0;  // Stores the last time the snake game was updated
int snakeUpdateDelay = 150;         // Delay in milliseconds between each snake update (controls speed)

// Function declarations
void setupSnake();
void updateSnakeGame();
void handleSnakeInput(int button);
void drawSnake();
void drawFood();
void resetSnakeGame();
void checkSnakeCollisions();
void placeSnakeFood();
void drawSnakeGameOver();

// Snake game setup
void setupSnake() {
    // Initialize the TFT display
    tft.init();
    tft.setRotation(1);  // Set to landscape mode
    tft.fillScreen(TFT_BLACK);

    // Initialize snake starting position
    for (int i = 0; i < snakeLength; i++) {
        snakeX[i] = 50 - i * 10;
        snakeY[i] = 50;
    }

    // Place initial food
    placeSnakeFood();

    // Draw initial snake and food
    drawSnake();
    drawFood();
}

// Place food at a random position
void placeSnakeFood() {
    foodX = (random(screenWidth / 10)) * 10;
    foodY = (random(screenHeight / 10)) * 10;
}

// Draw the snake on the screen
void drawSnake() {
    for (int i = 0; i < snakeLength; i++) {
        tft.fillRect(snakeX[i], snakeY[i], 10, 10, TFT_WHITE);  // Draw snake segments
    }
}

// Draw food on the screen
void drawFood() {
    tft.fillRect(foodX, foodY, 10, 10, TFT_RED);  // Draw food as a red square
}

// Update the game logic for Snake
void updateSnakeGame() {
    if (gameOverSnake) return;  // Don't update if the game is over

    unsigned long currentTime = millis();
    if (currentTime - lastSnakeUpdate >= snakeUpdateDelay) {
        // Move the snake body
        for (int i = snakeLength - 1; i > 0; i--) {
            snakeX[i] = snakeX[i - 1];
            snakeY[i] = snakeY[i - 1];
        }

        // Move the snake's head
        snakeX[0] += dirX * 10;
        snakeY[0] += dirY * 10;

        // Clear screen and redraw everything
        tft.fillScreen(TFT_BLACK);
        drawSnake();
        drawFood();

        // Check for collisions
        checkSnakeCollisions();

        lastSnakeUpdate = currentTime;  // Update the lastSnakeUpdate time
    }
}

// Check for collisions (walls, food, or self)
void checkSnakeCollisions() {
    // Check if snake hits the walls
    if (snakeX[0] < 0 || snakeX[0] >= screenWidth || snakeY[0] < 0 || snakeY[0] >= screenHeight) {
        gameOverSnake = true;
        drawSnakeGameOver();
    }

    // Check if snake eats the food
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        snakeLength++;
        placeSnakeFood();
    }

    // Check if snake collides with itself
    for (int i = 1; i < snakeLength; i++) {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
            gameOverSnake = true;
            drawSnakeGameOver();
        }
    }
}

// Handle snake input based on message button state
void handleSnakeInput(int button) {
    // Handle snake movement based on the button input
    if (button == 1 && dirX != 1) {  // Move left
        dirX = -1;
        dirY = 0;
    } else if (button == 2 && dirX != -1) {  // Move right
        dirX = 1;
        dirY = 0;
    } else if (button == 3 && dirY != 1) {  // Move up
        dirX = 0;
        dirY = -1;
    } else if (button == 4 && dirY != -1) {  // Move down
        dirX = 0;
        dirY = 1;
    }
}

// Draw the game over screen
void drawSnakeGameOver() {
    tft.fillScreen(TFT_RED);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(tft.width() / 2 - 40, tft.height() / 2 - 10);
    tft.println("Game Over");
}

// Reset the snake game
void resetSnakeGame() {
    gameOverSnake = false;
    snakeLength = 5;
    dirX = 1;
    dirY = 0;
    setupSnake();  // Reset snake position and food
}
