#include "snake.h"
#include "back.h"
#include "gameOver.h"
#include "newGame.h"
#include "message_struct.h"  // Include message struct for button input
#include "game_state.h"

extern TFT_eSPI tft;       // External TFT object
extern TFT_eSprite sprite; // External sprite object
extern struct_message message;  // External message struct for button data
extern GameState currentGameState;

// Snake game variables
int size = 1;
int x_positions[120] = {0};
int y_positions[120] = {0};

unsigned long currentTime = 0;
int period = 200;          // Time delay between snake updates
int dirX = 1, dirY = 0;   // Direction of the snake
bool gOver = false;        // Game over state
int foodX = 0, foodY = 0; // Food position
int howHard = 0;          // Difficulty level
String diff[3] = {"EASY", "NORMAL", "HARD"}; // Difficulty names

// Function to initialize the Snake game
void setupSnake() {
    tft.init();
    tft.setRotation(0);  // Set to portrait mode
    tft.fillScreen(TFT_BLACK);  // Clear the screen

    // Display background images
    tft.pushImage(0, 0, 170, 320, back);
    tft.pushImage(0, 30, 170, 170, newGame);

    // Draw difficulty selection
    tft.setTextColor(TFT_PURPLE, 0x7DFD);
    tft.fillSmoothCircle(28, 102 + (howHard * 24), 5, TFT_RED, TFT_BLACK);
    tft.drawString("DIFFICULTY: " + diff[howHard], 26, 267);

    // Initialize snake position
    size = 1;
    x_positions[0] = random(5, 15);
    y_positions[0] = random(5, 15);
    dirX = 0;  // Initially stationary
    dirY = 0;

    // Get initial food position
    getFood(); 
    tft.setTextSize(3);
    tft.setTextDatum(4);
    tft.drawString(String(size), 44, 250);
    tft.drawString(String(500 - period), 124, 250);
}

// Function to generate a new food location
void getFood() {
    foodX = random(0, 17);
    foodY = random(0, 17);
    
    // Ensure food doesn't spawn on the snake
    for (int i = 0; i < size; i++) {
        if (foodX == x_positions[i] && foodY == y_positions[i]) {
            getFood();  // Regenerate food if it overlaps with the snake
            return;
        }
    }
}

void run() {
    // Move the snake body
    for (int i = size; i > 0; i--) {
        x_positions[i] = x_positions[i - 1];
        y_positions[i] = y_positions[i - 1];
    }

    // Update the head position
    x_positions[0] += dirX;
    y_positions[0] += dirY;

    // Check for food collision
    if (x_positions[0] == foodX && y_positions[0] == foodY) {
        size++;
        getFood();  // Get new food position
        period = max(50, period - 10);  // Increase speed
    }

    // Clear the sprite
    sprite.fillSprite(TFT_BLACK);
    checkGameOver();

    // Render the snake and food
    if (!gOver) {
        // Draw game area border
        sprite.drawRect(0, 0, 170, 170, 0x02F3); // Draw game area

        // Draw snake
        for (int i = 0; i < size; i++) {
            sprite.fillRoundRect(x_positions[i] * 10, y_positions[i] * 10, 10, 10, 2, TFT_WHITE); // Draw snake
        }
        // Draw food
        sprite.fillRoundRect(foodX * 10 + 1, foodY * 10 + 1, 8, 8, 1, TFT_RED);
    } else {
        // Display game over screen
        sprite.pushImage(0, 0, 170, 170, gameOver);
    }
    sprite.pushSprite(0, 30); // Render sprite to screen
}

// Function to check if the game is over due to collisions
void checkGameOver() {
    // Check if the snake hits the boundaries
    if (x_positions[0] < 0 || x_positions[0] >= 17 || y_positions[0] < 0 || y_positions[0] >= 17) {
        gOver = true;
        return;
    }
    // Check if the snake collides with itself
    for (int i = 1; i < size; i++) {
        if (x_positions[i] == x_positions[0] && y_positions[i] == y_positions[0]) {
            gOver = true;
            return;
        }
    }
}

// Function to handle input for snake movement based on button
void handleSnakeInput(int button) {
    // Prevent direction reversal
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

// Function to update the Snake game logic
void updateSnakeGame() {
    if (!gOver) {
        run();  // Call the main game logic
    }
}
