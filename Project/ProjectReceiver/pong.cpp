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
int updateDelay = 20;  // Decreased delay in milliseconds between each update (controls game speed)

// Pong game setup
void setupPong() {
    // Initialize the TFT display
    tft.init();
    tft.setRotation(1);  // Set to landscape mode

    // Clear the screen first to remove any remnants from the menu
    tft.fillScreen(TFT_BLACK);

    // Set the positions of paddles
    rpaddle_x = tft.width() - paddle_w;
    lpaddle_y = tft.height() / 2 - paddle_h / 2;
    rpaddle_y = lpaddle_y;

    // Set ball initial position
    ball_x = tft.width() / 2;
    ball_y = tft.height() / 2;

    // Set midline position
    dashline_x = tft.width() / 2 - 1;
    dashline_n = tft.height() / dashline_h;

    // Draw the midline, paddles, and ball
    drawMidline();
    drawPaddles();
    drawBall();

    // Display the initial scores
    displayScores();
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

// Display the scores
void displayScores() {
    // Define the score display positions and dimensions
    int scoreHeight = 200;
    int scorePadding = 10;
    int scoreWidth = 140;

    // Clear just the number part of the scores area to avoid flickering
    tft.fillRect(scorePadding, 5, scoreWidth, scoreHeight, TFT_BLACK);  // Player score area
    tft.fillRect(tft.width() - scoreWidth - scorePadding, 5, scoreWidth, scoreHeight, TFT_BLACK);  // Computer score area

    // Draw player score (P1)
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);  // White text with black background (to overwrite old text)
    tft.setCursor(scorePadding, 5);  // Position of player score
    tft.printf("P1: %d", playerScore);

    // Draw computer score (P2)
    tft.setCursor(tft.width() - scoreWidth - scorePadding, 5);  // Position of computer score
    tft.printf("P2: %d", computerScore);
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
        tft.fillRect(ball_x - ball_dx, ball_y - ball_dy, ball_w, ball_h, TFT_BLACK);

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

        // Check if ball goes out of bounds (left or right)
        if (ball_x < 0) {
            computerScore++;
            displayScores();  // Update scores after a point is scored
            resetBall();
        } else if (ball_x + ball_w > tft.width()) {
            playerScore++;
            displayScores();  // Update scores after a point is scored
            resetBall();
        }

        // Check if the game is over
        if (playerScore == 10 || computerScore == 10) {
            gameOverPong = true;
            tft.fillScreen(TFT_RED);
            tft.setTextSize(2);
            tft.setTextColor(TFT_WHITE);
            tft.setCursor(tft.width() / 2 - 40, tft.height() / 2 - 10);
            tft.println("Game Over");
            tft.setCursor(tft.width() / 2 - 60, tft.height() / 2 + 10);
            tft.printf("Player %s!", (playerScore == 10) ? "Wins" : "Loses");
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
        tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, TFT_BLACK);  // Clear entire paddle before moving
        rpaddle_y -= 12;  // Move paddle up
        tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, TFT_WHITE);  // Redraw paddle in new position
    } else if (button == 2 && rpaddle_y < tft.height() - paddle_h) {
        // Right paddle moving down
        tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, TFT_BLACK);  // Clear entire paddle before moving
        rpaddle_y += 12;  // Move paddle down
        tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, TFT_WHITE);  // Redraw paddle in new position
    }
}

void updateComputerPaddle() {
    int paddleSpeed = 1;  
    // Move the computer paddle towards the ball's y-position
    if (lpaddle_y + paddle_h / 2 < ball_y) {
        tft.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, TFT_BLACK);  // Clear paddle before moving
        lpaddle_y += paddleSpeed;  // Move down
        tft.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, TFT_WHITE);  // Redraw paddle in new position
    } else if (lpaddle_y + paddle_h / 2 > ball_y) {
        tft.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, TFT_BLACK);  // Clear paddle before moving
        lpaddle_y -= paddleSpeed;  // Move up
        tft.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, TFT_WHITE);  // Redraw paddle in new position
    }

    // Keep the computer paddle within the screen boundaries
    if (lpaddle_y < 0) lpaddle_y = 0;
    if (lpaddle_y + paddle_h > tft.height()) lpaddle_y = tft.height() - paddle_h;
}

// Reset the ball to the center after a point
void resetBall() {
    ball_x = tft.width() / 2;
    ball_y = tft.height() / 2;
    ball_dx = -ball_dx;  // Change direction of the ball
}
