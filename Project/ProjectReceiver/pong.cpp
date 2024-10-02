#include "pong.h"
#include "message_struct.h"  // Include the struct_message definition

extern TFT_eSPI tft;  // External TFT object

// Pong game variables
int paddle_h = 30;
int paddle_w = 4;
int lpaddle_x = 0;
int rpaddle_x;
int lpaddle_y;
int rpaddle_y;
int ball_x, ball_y;
int ball_dx = 1, ball_dy = 1;
int ball_w = 6;
int ball_h = 6;
int dashline_x;
int dashline_h = 4;
int dashline_w = 2;
int dashline_n;
bool gameOverPong = false;

// Timing variables
unsigned long lastUpdate = 0;  // Stores the last time the game was updated
int updateDelay = 30;  // Delay in milliseconds between each update (controls game speed)

// Pong game setup
void setupPong() {
    // Set the position of the paddles
    rpaddle_x = tft.width() - paddle_w;
    lpaddle_y = tft.height() / 2 - paddle_h / 2;
    rpaddle_y = lpaddle_y;
    
    // Set ball initial position
    ball_x = tft.width() / 2;
    ball_y = tft.height() / 2;

    // Set midline position
    dashline_x = tft.width() / 2 - 1;
    dashline_n = tft.height() / dashline_h;

    // Clear the screen
    tft.fillScreen(TFT_BLACK);  

    // Draw midline, paddles, and ball
    drawMidline();
    drawPaddles();
    drawBall();

    Serial.println("Pong game initialized and drawn.");
}

// Draw the midline
void drawMidline() {
    tft.startWrite();
    tft.setAddrWindow(dashline_x, 0, dashline_w, tft.height());
    
    for (int i = 0; i < dashline_n; i += 2) {
        tft.pushColor(TFT_WHITE, dashline_w * dashline_h);  // Dash pixels
        tft.pushColor(TFT_BLACK, dashline_w * dashline_h);  // Gap pixels
    }

    tft.endWrite();
}

// Draw the paddles
void drawPaddles() {
    tft.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, TFT_WHITE);  // Left paddle
    tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, TFT_WHITE);  // Right paddle
}

// Draw the ball
void drawBall() {
    tft.fillRect(ball_x, ball_y, ball_w, ball_h, TFT_WHITE);
}

// Update the game logic for Pong
void updatePongGame() {
    if (gameOverPong) return;  // Don't update if the game is over

    unsigned long currentTime = millis();
    if (currentTime - lastUpdate >= updateDelay) {
        // Move the ball
        ball_x += ball_dx;
        ball_y += ball_dy;

        // Ball collision with top or bottom of the screen
        if (ball_y <= 0 || ball_y + ball_h >= tft.height()) {
            ball_dy = -ball_dy;
        }

        // Ball collision with paddles
        if (ball_x <= lpaddle_x + paddle_w && ball_y + ball_h >= lpaddle_y && ball_y <= lpaddle_y + paddle_h) {
            ball_dx = -ball_dx;
        } else if (ball_x + ball_w >= rpaddle_x && ball_y + ball_h >= rpaddle_y && ball_y <= rpaddle_y + paddle_h) {
            ball_dx = -ball_dx;
        }

        // Check if ball goes out of bounds
        if (ball_x < 0 || ball_x + ball_w > tft.width()) {
            gameOverPong = true;
            tft.fillS.creen(TFT_RED);
            tft.setTextSize(2);
            tft.setTextColor(TFT_WHITE);
            tft.setCursor(tft.width() / 2 - 40, tft.height() / 2 - 10);
            tft.println("Game Over");
        }

        // Redraw the ball
        tft.fillRect(ball_x - ball_dx, ball_y - ball_dy, ball_w, ball_h, TFT_BLACK);  // Clear previous ball
        drawBall();

        lastUpdate = currentTime;  // Update the lastUpdate time
    }
}

// Handle paddle input based on message button state
void handlePongInput(int button) {
    // Handle paddle movement based on the button input
    if (button == 1 && rpaddle_y > 0) {
        // Right paddle moving up
        tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, 1, TFT_BLACK);  // Clear part of the paddle
        rpaddle_y -= 5;  // Move paddle up
        tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, TFT_WHITE);  // Redraw paddle
    } else if (button == 2 && rpaddle_y < tft.height() - paddle_h) {
        // Right paddle moving down
        tft.fillRect(rpaddle_x, rpaddle_y + paddle_h, paddle_w, 1, TFT_BLACK);  // Clear part of the paddle
        rpaddle_y += 5;  // Move paddle down
        tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, TFT_WHITE);  // Redraw paddle
    }
}
