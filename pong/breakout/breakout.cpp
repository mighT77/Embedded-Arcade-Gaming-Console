#include "breakout.h"
//add gfx header files
 
int ballX = int(0.4 * screenWidth); // Middle of the screen horizontally
int ballY = int(0.4 * screenHeight); // Somewhere near the bottom


int ballSpeedX = 1; // Ball movement speed in X
int ballSpeedY = -1; // Ball movement speed in Y

int score = 0;          // Current score

int paddleX = int(0.05 * screenWidth);
int paddleY = int(0.9 * screenHeight);

#define brickRows 16   // Number of rows of bricks
#define brickCols 8  // Number of columns of bricks
int brickWidth = screenWidth/16; // Width of each brick
int brickHeight = screenHeight/20; // Height of each brick
 // Spacing between bricks

bool bricks[brickRows][brickCols]; 

void setupBreakout() {
    gfx.fillScreen(vga.rgb(0, 0, 0)); // Clear the screen
    vga.show();
 
    // Initialize ball position and speed
    ballX = 64; // Middle of the screen horizontally
    ballY = 160; // Somewhere near the bottom
    score = 0;
    
    initializeBricks();
    // Draw initial positions
    drawPaddle();
    drawBall();
    drawBricks();
    drawScore();
}

void updateBreakoutGame() {
    // Erase ball's previous position
    gfx.fillRect(ballX, ballY, 4, 4, vga.rgb(0, 0, 0));
 
    // Update ball position
    ballX += ballSpeedX;
    ballY += ballSpeedY;
 
    // Collision with walls (left, right, and top)
    if (ballX <= 0 || ballX >= screenWidth - 4) {  // Subtract ball width (4) to stay within bounds
        ballSpeedX = -ballSpeedX;
    }
    if (ballY <= 0) {
        ballSpeedY = -ballSpeedY;
    }
 
    // Collision with paddle
    if (ballY + 4 >= paddleY && ballX >= paddleX && ballX <= (paddleX + 24)) {
        ballSpeedY = -ballSpeedY;  // Reverse Y direction
        ballY = paddleY - 4;       // Move the ball above the paddle to avoid getting stuck
    }
    
    // Collision with bricks
    checkBallBrickCollision();

    // Game over if ball touches the bottom
    if (ballY >= screenHeight) {
        setupBreakout(); // Restart game
        return;
    }
}

void initializeBricks() {
    for (int row = 0; row < brickRows; row++) {
        for (int col = 0; col < brickCols; col++) {
            bricks[row][col] = true; // Set all bricks as present initially
        }
    }
}

void checkBallBrickCollision() {
    // Calculate the ball's size
    int ballSize = 4; // Assume the ball is a 4x4 square

    // Loop through each brick to check for collision
    for (int row = 0; row < brickRows; row++) {
        for (int col = 0; col < brickCols; col++) {
            if (bricks[row][col]) { // Check if the brick is still present
                // Calculate the position of the current brick
                int brickX = row * (brickWidth);
                int brickY = col * (brickHeight);

                // Check if the ball is within the brick's bounds
                if (ballX < brickX + brickWidth && ballX + ballSize > brickX && 
                    ballY < brickY + brickHeight && ballY + ballSize > brickY) {

                    // Collision detected, remove the brick
                    bricks[row][col] = false;

                    score += 10;  // Increment score by 10 (or any desired value)

                    // Reverse the ball's Y direction (bounce)
                    ballSpeedY = -ballSpeedY;

                    // Optional: Add sound or visual effects for the collision
                    // Example: gfx.fillRect(brickX, brickY, brickWidth, brickHeight, vga.rgb(0, 0, 0)); // Erase brick immediately
                    
                    return; // Exit the loop after the first collision to avoid multiple bounces
                }
            }
        }
    }
}

void drawScore() {
    // Clear the bottom area for score display
    gfx.fillRect(0, screenHeight - 20, screenWidth, 20, vga.rgb(0, 0, 0)); // Clear the bottom area

    gfx.setTextColor(vga.rgb(0x80, 0x80, 0x80)); // Set text color to white
    
    // Set cursor position for score display
    gfx.setCursor(screenWidth - 150, screenHeight - 15); // Adjust as needed

    // Display current score
    gfx.print("Score: ");
    gfx.print(score); // Print the current score
}


void drawBricks() {
    for (int row = 0; row < brickRows; row++) {
        for (int col = 0; col < brickCols; col++) {
            if (bricks[row][col]) {
                int brickX = row * (brickWidth);
                int brickY = col * (brickHeight);
                gfx.drawRect(brickX, brickY, brickWidth, brickHeight, vga.rgb(0, 0, 0x80)); // Brick color
            }
        }
    }
}


void drawPaddle() {
    gfx.fillRect(paddleX, paddleY, 24, 4, vga.rgb(0, 0x80, 0x80)); // Paddle color
}

// Draw all elements in the game
void drawBall() {
    // Draw ball
    gfx.fillRect(ballX, ballY, 4, 4, vga.rgb(0x80, 0, 0)); // Ball color
}

void drawBreakoutElements() {
  vga.clear(vga.rgb(0,0,0));
  drawPaddle();
  drawBall();
  drawBricks();
  drawScore();
  vga.show();
}

void movePaddleLeft() {
    if (paddleX > 0) {  // Prevent paddle from moving off the screen
        paddleX -= 10;
    }
}

void movePaddleRight() {
    if (paddleX < screenWidth - 24) {  // Prevent paddle from moving off the screen
        paddleX += 10;
    }
}
