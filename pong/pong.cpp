
unsigned long lastMoveTimePaddleA = 0;
unsigned long lastMoveTimePaddleB = 0;  // Stores the last time the game was updated
const int updateDelay = 20;          // Delay in milliseconds between each update (controls game speed)
int ballX = screenWidth/2;
int ballY = screenHeight/2;
int bwidth = 5;
int bheight = 5;
int balldX = 3;
int balldY = 3;
unsigned long lastBallMoveTime = 0;  // Timer for ball movement
const unsigned long ballMoveInterval = 30; 


// Scores
int player1Score = 0;
int player2Score = 0;

void initializeGame() {
    vga.bufferCount = 2;
    if (!vga.init(pins, mode, 16)) while (1) delay(1);
    vga.clear(vga.rgb(0, 0, 0));

    vga.start();
    gfx.setRotation(0);
    gfx.fillRect(p1_x, p1_y, 15, 50, vga.rgb(0, 0, 255));  // Blue paddle
    gfx.fillRect(p2_x, p2_y, 15, 50, vga.rgb(255, 0, 0));  // Red paddle
    gfx.drawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, vga.rgb(255, 255, 0));
    gfx.fillRect(ballX, ballY, bwidth, bheight, vga.rgb(0, 255, 0));
    drawScoreboard();
    vga.show();
}

// Paddle 1 Up movement
void movePaddle1Up() {
    if (p1_y > 0) {  // Prevent paddle from moving off the screen
        p1_y -= 10;
    }
}

// Paddle 1 Down movement
void movePaddle1Down() {
    if (p1_y < screenHeight - 50) {  // Prevent paddle from moving off the screen
        p1_y += 10;
    }
}

// Paddle 2 Up movement
void movePaddle2Up() {
    if (p2_y > 0) {  // Prevent paddle from moving off the screen
        p2_y -= 10;
    }
}

// Paddle 2 Down movement
void movePaddle2Down() {
    if (p2_y < screenHeight - 50) {  // Prevent paddle from moving off the screen
        p2_y += 10;
    }
}


void updateGame() {
    bool gameIsOver = false;

    // Check if the game is over
    if (player1Score >= 10 || player2Score >= 10) {
        gameIsOver = true;
        gameOver();
    } else {
        // Ball movement
        ballMovement();

        // Draw the current game state (paddles, ball, scoreboard)
        drawScreen();
    }

    // If the game is over, display the game over screen
    if (gameIsOver) {
        gameOver();
    }
}


void drawPaddles() {
    gfx.fillRect(p1_x, p1_y, 15, 50, vga.rgb(0, 0, 255));  // Blue paddle
    gfx.fillRect(p2_x, p2_y, 15, 50, vga.rgb(255, 0, 0));  // Red paddle
}

void resetBall() {
    ballX = screenWidth / 2;
    ballY = screenHeight / 2;
    balldX = (random(0, 2) == 0) ? -2 : 2;
    balldY = (random(0, 2) == 0) ? -1 : 1;
    delay(250); 
}

void ballMovement() {
    unsigned long currentTime = millis();
    if (currentTime - lastBallMoveTime > ballMoveInterval) {
        gfx.fillRect(ballX, ballY, bwidth, bheight, vga.rgb(0, 0, 0));

        int newBallX = ballX + balldX;
        int newBallY = ballY + balldY;

        if (newBallY - bheight <= 0 || newBallY + bheight >= screenHeight) {
            balldY = -(balldY + ((random(0, 2) == 0) ? 0 : 2));
            newBallY = ballY + balldY;
        }

        if (newBallX - bwidth <= 0) {
            player2Score++;
            resetBall();
            return;
        }

        if (newBallX + bwidth >= screenWidth) {
            player1Score++;
            resetBall();
            return;
        }

        if (newBallX - bwidth <= p1_x + 15 && newBallY >= p1_y && newBallY <= p1_y + 50) {
            balldX = -balldX;
            newBallX = p1_x + 15 + bwidth;
        }

        if (newBallX + bwidth >= p2_x && newBallY >= p2_y && newBallY <= p2_y + 50) {
            balldX = -balldX;
            newBallX = p2_x - bwidth;
        }

        ballX = newBallX;
        ballY = newBallY;
    }
}

void drawScoreboard() {
    gfx.setTextSize(0);
    gfx.setCursor(10, 10);
    gfx.print("Player 1: ");
    gfx.print(player1Score);
    gfx.setCursor(200, 10);
    gfx.print("Player 2: ");
    gfx.print(player2Score);
}

void drawBall() {
    gfx.fillRect(ballX, ballY, bwidth, bheight, vga.rgb(0, 255, 0));
    gfx.drawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, vga.rgb(255, 255, 0));
}

void drawScreen() {
    vga.clear(vga.rgb(0, 0, 0));
    drawPaddles();
    drawBall();
    drawScoreboard();
    vga.show();
}

void gameOver() {
    if (player1Score >= 10) {
        vga.clear(vga.rgb(0x80, 0, 0));
        gfx.setTextSize(0);
        gfx.setCursor(100, 110);
        gfx.print("Player A Wins!");
        gfx.setCursor(90, 120);
        gfx.print("Press reset to play again");
        vga.show();
    } else if (player2Score >= 10) {
        vga.clear(vga.rgb(0, 0x80, 0));
        gfx.setTextSize(0);
        gfx.setCursor(100, 110);
        gfx.print("Player B Wins!");
        gfx.setCursor(90, 120);
        gfx.print("Press reset to play again");
        vga.show();
    }
}