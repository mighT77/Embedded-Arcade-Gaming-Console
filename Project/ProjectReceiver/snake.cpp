#include "snake.h"
#include "back.h"
#include "gameOver.h"
#include "newGame.h"
#include "message_struct.h"  // Include message struct for button input

extern TFT_eSPI tft;
extern TFT_eSprite sprite;
extern struct_message message;  // External message struct for button data

int size = 1;
int y[120] = {0};
int x[120] = {0};

unsigned long currentTime = 0;
int period = 200;
int deb = 0, deb2 = 0;
int dirX = 1;
int dirY = 0;
bool taken = false;
unsigned short colors[2] = {0x48ED, 0x590F};  // Terrain colors
unsigned short snakeColor[2] = {0x9FD3, 0x38C9};
bool chosen = false;
bool gOver = false;
int moves = 0;
int playTime = 0;
int foodX = 0;
int foodY = 0;
int howHard = 0;
String diff[3] = {"EASY", "NORMAL", "HARD"};
bool ready = true;
long readyTime = 0;

void setupSnake() {
    tft.init();
    tft.setRotation(0);  // Set the screen to portrait mode
    tft.fillScreen(TFT_BLACK);  // Clear the screen
    tft.setSwapBytes(true);

    tft.pushImage(0, 0, 170, 320, back);
    tft.pushImage(0, 30, 170, 170, newGame);

    tft.setTextColor(TFT_PURPLE, 0x7DFD);
    tft.fillSmoothCircle(28, 102 + (howHard * 24), 5, TFT_RED, TFT_BLACK);
    tft.drawString("DIFFICULTY:   " + diff[howHard] + "   ", 26, 267);

    sprite.createSprite(170, 170);
    sprite.setSwapBytes(true);

    // Simulating difficulty selection with button input through message struct
    while (message.button != 1) {  // Wait for button 1 press (start)
        if (message.button == 0) {  // Button 0 pressed (difficulty change)
            if (deb2 == 0) {
                deb2 = 1;
                tft.fillCircle(28, 102 + (howHard * 24), 6, TFT_BLACK);
                howHard++;
                if (howHard == 3) howHard = 0;
                tft.fillSmoothCircle(28, 102 + (howHard * 24), 5, TFT_RED, TFT_BLACK);
                tft.drawString("DIFFICULTY:   " + diff[howHard] + "   ", 26, 267);
                period = 200 - howHard * 20;
                delay(200);
            }
        } else {
            deb2 = 0;
        }
    }

    // Initialize snake position
    size = 1;
    x[0] = random(5, 15);
    y[0] = random(5, 15);
    dirX = 0;  // Initially stationary
    dirY = 0;
    gOver = false;  // Reset the game over flag
    getFood();  // Get initial food position
    tft.setTextSize(3);
    tft.setTextDatum(4);
    tft.drawString(String(size), 44, 250);
    tft.drawString(String(500 - period), 124, 250);
    
    delay(1000);  // Delay to give the player time to get ready
}

void getFood() {
    foodX = random(0, 17);
    foodY = random(0, 17);
    taken = false;
    for (int i = 0; i < size; i++) {
        if (foodX == x[i] && foodY == y[i]) {
            taken = true;
        }
    }
    if (taken) getFood();
}

void run() {
    for (int i = size; i > 0; i--) {
        x[i] = x[i - 1];
        y[i] = y[i - 1];
    }

    x[0] = x[0] + dirX;
    y[0] = y[0] + dirY;

    if (x[0] == foodX && y[0] == foodY) {
        size++;
        getFood();
        tft.drawString(String(size), 44, 250);
        period = period - 1;
        tft.drawString(String(500 - period), 124, 250);
    }

    sprite.fillSprite(TFT_BLACK);
    checkGameOver();
    if (!gOver) {
        sprite.drawRect(0, 0, 170, 170, 0x02F3);
        for (int i = 0; i < size; i++) {
            sprite.fillRoundRect(x[i] * 10, y[i] * 10, 10, 10, 2, snakeColor[0]);
            sprite.fillRoundRect(2 + x[i] * 10, 2 + y[i] * 10, 6, 6, 2, snakeColor[1]);
        }
        sprite.fillRoundRect(foodX * 10 + 1, foodY * 10 + 1, 8, 8, 1, TFT_RED);
        sprite.fillRoundRect(foodX * 10 + 3, foodY * 10 + 3, 4, 4, 1, 0xFE18);
    } else {
        sprite.pushImage(0, 0, 170, 170, gameOver);
    }
    sprite.pushSprite(0, 30);
}

void checkGameOver() {
    // Check if the snake hits the boundary
    if (x[0] < 0 || x[0] >= 17 || y[0] < 0 || y[0] >= 17) {
        gOver = true;
        return;  // End game
    }

    // Check if the snake collides with itself
    for (int i = 1; i < size; i++) {
        if (x[i] == x[0] && y[i] == y[0]) {
            gOver = true;
            return;  // End game
        }
    }
}

// Handle input for the Snake game
void handleSnakeInput(int button) {
    // Debounce the input
    delay(200);

    // Only two button values: 0 and 1
    Serial.print("Button pressed: ");
    Serial.println(button);
    
    if (button == 1 && dirX != -1) {  // Move right or up
        dirX = 1;
        dirY = 0;
    } else if (button == 0 && dirX != 1) {  // Move left or down
        dirX = -1;
        dirY = 0;
    }
}

// Update the Snake game logic
void updateSnakeGame() {
    if (!gOver) {
        run();  // Call the main game logic
    }
}

void loopSnake() {
    // Ensure game logic runs based on the period (timing control)
    if (millis() > currentTime + period) {
        run();  // Call the main game logic
        currentTime = millis();
    }

    if (millis() > readyTime + 100 && !ready) {
        ready = true;
    }

    if (ready) {
        // Handle left button press (turn left or down) via message struct
        if (message.button == 0 && deb == 0) {
            deb = 1;  // Set debounce flag
            if (dirX == 1) { dirY = dirX * -1; dirX = 0; }  // Change direction
            if (dirX == -1) { dirY = dirX * -1; dirX = 0; }
            if (dirY == 1) { dirX = dirY * 1; dirY = 0; }
            if (dirY == -1) { dirX = dirY * 1; dirY = 0; }
            ready = false;  // Reset ready flag
            readyTime = millis();  // Record time of button press
        } else {
            deb = 0;  // Reset debounce flag when no press is detected
        }

        // Handle right button press (turn right or up) via message struct
        if (message.button == 1 && deb2 == 0) {
            deb2 = 1;  // Set debounce flag
            if (dirX == 1) { dirY = dirX * 1; dirX = 0; }  // Change direction
            if (dirX == -1) { dirY = dirX * 1; dirX = 0; }
            if (dirY == 1) { dirX = dirY * -1; dirY = 0; }
            if (dirY == -1) { dirX = dirY * -1; dirY = 0; }
            ready = false;  // Reset ready flag
            readyTime = millis();  // Record time of button press
        } else {
            deb2 = 0;  // Reset debounce flag when no press is detected
        }
    }
}
