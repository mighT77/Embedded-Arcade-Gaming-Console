#include "pong.h"
#include "message_struct.h"  // Include the struct_message definition
#include "ESP32S3VGA.h"
#include <GfxWrapper.h>

extern VGA vga;
extern Mode mode;
extern GfxWrapper<VGA> gfx;  // GfxWrapper for drawing to the VGA

// Pong game variables
int paddle_h = 30;
int paddle_w = 4;
int lpaddle_x = 0;  // Left paddle
int rpaddle_x;      // Right paddle
int lpaddle_y;      // Left paddle position
int rpaddle_y;      // Right paddle position
int ball_x, ball_y;
int ball_dx = 2, ball_dy = 2;  // Increased ball speed
int ball_w = 6;
int ball_h = 6;
int dashline_x;
int dashline_h = 4;
int dashline_w = 2;
int dashline_n;
bool gameOverPong = false;

// Scores
int playerScore = 0;
int computerScore = 0;

// Timing variables
unsigned long lastUpdate = 0;  // Stores the last time the game was updated
int updateDelay = 20;          // Delay in milliseconds between each update (controls game speed)

// Pong game setup
void setupPong() {
    // Clear the screen first to remove any remnants from the menu
    vga.clear(vga.rgb(0, 0, 0));

    // Set the positions of paddles
    rpaddle_x = 640 - paddle_w;  // Assuming VGA width of 640
    lpaddle_y = 240 - paddle_h / 2; // Center the left paddle
    rpaddle_y = lpaddle_y;  // Initialize right paddle at the same height

    // Set ball initial position
    ball_x = 320;  // Center the ball on the screen
    ball_y = 240;

    // Set midline position
    dashline_x = 320 - 1; // Midpoint for dashline
    dashline_n = 480 / dashline_h; // Assuming VGA height of 480

    // Draw the midline, paddles, and ball
    drawMidline();
    drawPaddles();
    drawBall();

    // Display the initial scores
    displayScores();
}

// Draw the midline
void drawMidline() {
    for (int i = 0; i < dashline_n; i += 2) {
        gfx.fillRect(dashline_x, i * dashline_h, dashline_w, dashline_h, vga.rgb(255, 255, 255));  // Dash pixels
        gfx.fillRect(dashline_x, (i * dashline_h) + dashline_h, dashline_w, dashline_h, vga.rgb(0, 0, 0));  // Gap pixels
    }
}

// Draw the paddles
void drawPaddles() {
    gfx.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, vga.rgb(255, 255, 255));  // Left paddle
    gfx.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, vga.rgb(255, 255, 255));  // Right paddle
}

// Draw the ball
void drawBall() {
    gfx.fillRect(ball_x, ball_y, ball_w, ball_h, vga.rgb(255, 255, 255)); // Ball color white
}

// Display the scores
void displayScores() {
    // Clear just the number part of the scores area to avoid flickering
    gfx.fillRect(10, 5, 140, 40, vga.rgb(0, 0, 0));  // Player score area
    gfx.fillRect(640 - 150, 5, 140, 40, vga.rgb(0, 0, 0));  // Player 2 score area

    // Draw player score (P1)
    gfx.setTextColor(vga.rgb(255, 255, 255));  // White text
    gfx.setTextSize(2);
    gfx.setCursor(10, 5);  // Position of player score
    gfx.printf("P1: %d", playerScore);

    // Draw player 2 score (P2)
    gfx.setCursor(640 - 150, 5);  // Position of player 2 score
    gfx.printf("P2: %d", computerScore);
}

// Update the game logic for Pong
void updatePongGame() {
    if (gameOverPong) return;  // Don't update if the game is over

    unsigned long currentTime = millis();
    if (currentTime - lastUpdate >= updateDelay) {
        // Move the ball
        ball_x += ball_dx;
        ball_y += ball_dy;

        // Clear the previous ball position
        gfx.fillRect(ball_x - ball_dx, ball_y - ball_dy, ball_w, ball_h, vga.rgb(0, 0, 0)); // Clear previous ball position

        // Ball collision with top or bottom of the screen
        if (ball_y <= 0 || ball_y + ball_h >= 480) {  // Assuming VGA height of 480
            ball_dy = -ball_dy;
        }

        // Ball collision with paddles
        if (ball_x <= lpaddle_x + paddle_w && ball_y + ball_h >= lpaddle_y && ball_y <= lpaddle_y + paddle_h) {
            ball_dx = -ball_dx;
        } else if (ball_x + ball_w >= rpaddle_x && ball_y + ball_h >= rpaddle_y && ball_y <= rpaddle_y + paddle_h) {
            ball_dx = -ball_dx;
        }

        // Check if ball goes out of bounds (left or right)
        if (ball_x < 0) {
            computerScore++;  // Update Player 2's score
            displayScores();  // Update scores after a point is scored
            resetBall();
        } else if (ball_x + ball_w > 640) {  // Assuming VGA width of 640
            playerScore++;  // Update Player 1's score
            displayScores();  // Update scores after a point is scored
            resetBall();
        }

        // Check if the game is over
        if (playerScore == 10 || computerScore == 10) {
            gameOverPong = true;
            gfx.fillScreen(vga.rgb(255, 0, 0)); // Game over screen red
            gfx.setTextColor(vga.rgb(255, 255, 255));  // White text
            gfx.setTextSize(2);
            gfx.setCursor(320 - 40, 240 - 10);  // Centering for game over text
            gfx.println("Game Over");
            gfx.setCursor(320 - 60, 240 + 10);
            gfx.printf("Player %s!", (playerScore == 10) ? "1 Wins" : "2 Wins");
        }

        // Redraw the ball
        drawBall();

        // Redraw the midline in every update to avoid it getting erased
        drawMidline();

        // Update the computer player's paddle position
        updateComputerPaddle();

        lastUpdate = currentTime;  // Update the lastUpdate time
    }
}

// Handle paddle input based on message button state
void handlePongInput(int button) {
    // Handle paddle movement based on the button input
    if (button == 1 && rpaddle_y > 0) {
        // Right paddle moving up
        gfx.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, vga.rgb(0, 0, 0));  // Clear entire paddle before moving
        rpaddle_y -= 10;  // Move paddle up
        gfx.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, vga.rgb(255, 255, 255));  // Redraw paddle in new position
    } else if (button == 2 && rpaddle_y < 480 - paddle_h) { // Assuming VGA height of 480
        // Right paddle moving down
        gfx.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, vga.rgb(0, 0, 0));  // Clear entire paddle before moving
        rpaddle_y += 10;  // Move paddle down
        gfx.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, vga.rgb(255, 255, 255));  // Redraw paddle in new position
    } else if (button == 5 && lpaddle_y > 0) {  // Player 2 controls (up)
        gfx.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, vga.rgb(0, 0, 0));  // Clear entire paddle before moving
        lpaddle_y -= 10;  // Move left paddle up
        gfx.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, vga.rgb(255, 255, 255));  // Redraw paddle in new position
    } else if (button == 6 && lpaddle_y < 480 - paddle_h) {  // Player 2 controls (down)
        gfx.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, vga.rgb(0, 0, 0));  // Clear entire paddle before moving
        lpaddle_y += 10;  // Move left paddle down
        gfx.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, vga.rgb(255, 255, 255));  // Redraw paddle in new position
    }
}

// Update the computer-controlled paddle (left paddle)
void updateComputerPaddle() {
    int paddleSpeed = 2;  // Reduce speed to make the computer worse

    // Move the computer paddle towards the ball's y-position
    if (lpaddle_y + paddle_h / 2 < ball_y) {
        gfx.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, vga.rgb(0, 0, 0));  // Clear paddle before moving
        lpaddle_y += paddleSpeed;  // Move down
        gfx.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, vga.rgb(255, 255, 255));  // Redraw paddle in new position
    } else if (lpaddle_y + paddle_h / 2 > ball_y) {
        gfx.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, vga.rgb(0, 0, 0));  // Clear paddle before moving
        lpaddle_y -= paddleSpeed;  // Move up
        gfx.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, vga.rgb(255, 255, 255));  // Redraw paddle in new position
    }

    // Keep the computer paddle within the screen boundaries
    if (lpaddle_y < 0) lpaddle_y = 0;
    if (lpaddle_y + paddle_h > 480) lpaddle_y = 480 - paddle_h;  // Assuming VGA height of 480
}

// Reset the ball to the center after a point
void resetBall() {
    ball_x = 320;  // Center the ball
    ball_y = 240;  // Center the ball
    ball_dx = -ball_dx;  // Change direction of the ball
}
