//This example shows how to use the GfxWrapper to be able to use the Adafruit GFX library with VGA
//bitluni

#include "ESP32S3VGA.h"
#include <GfxWrapper.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>
#include <Fonts/FreeSerif24pt7b.h>

#define button_A 0
#define button_B 14
#define button_C 17
#define button_D 18


unsigned long lastMoveTimePaddleA = 0;
unsigned long lastMoveTimePaddleB = 0;  // Stores the last time the game was updated
const int updateDelay = 20;          // Delay in milliseconds between each update (controls game speed)
int screenWidth = 320;
int screenHeight = 240;
int ballX = 160;
int ballY = 120;
int bwidth = 5;
int bheight = 5;
int balldX = 3;
int balldY = 3;
unsigned long lastBallMoveTime = 0;  // Timer for ball movement
const unsigned long ballMoveInterval = 30; 


// Scores
int player1Score = 0;
int player2Score = 0;


//                   r,r,r,r,r,  g,g, g, g, g, g,   b, b, b, b, b,   h,v
const PinConfig pins(-1,-1,-1,-1,1,  -1,-1,-1,-1,-1,2,  -1,-1,-1,-1,3,  10,11);

//VGA Device
VGA vga;
Mode mode = Mode::MODE_320x240x60;
GfxWrapper<VGA> gfx(vga, mode.hRes, mode.vRes);

int p1_x = 20;
int p2_x = 280;
int p1_y = 150;
int p2_y = 150;

//initial setup
void setup()
{
  vga.bufferCount = 2;
  if(!vga.init(pins, mode, 16)) while(1) delay(1);
  vga.clear(vga.rgb(0, 0, 0));

  vga.start();
  gfx.setRotation(0);
  gfx.fillRect(p1_x, p1_y, 15, 50, vga.rgb(0, 0, 255));  // Blue paddle
  gfx.fillRect(p2_x, p2_y, 15, 50, vga.rgb(255, 0, 0));  // Red paddle
  gfx.drawLine(screenWidth/2, 0, screenWidth/2, screenHeight, vga.rgb(255,255,0));
  gfx.fillRect(ballX, ballY, bwidth, bheight, vga.rgb(0, 255, 0));
  gfx.setTextColor(vga.rgb(255, 255, 0)); // Set text color to white
  
  gfx.setTextSize(0);
  gfx.setCursor(10, 10); // Position for Player 1 score
  gfx.print("Player 1: ");
  gfx.print(player1Score); // Display Player 1 score

  gfx.setCursor(200, 10); // Position for Player 2 score
  gfx.print("Player 2: ");
  gfx.print(player2Score); // Display Player 2 score
  vga.show();
}




void movePaddle1() {
  
  bool paddleMoved = false;
  unsigned long currentTime = millis();
  if (currentTime - lastMoveTimePaddleA >= updateDelay) {
    if (digitalRead(button_A) == LOW) {  // Move paddle 1 down
      if (p1_y < 190) {
        p1_y += 10;
      }
    }
    
    if (digitalRead(button_B) == LOW) {  // Move paddle 1 up
      if (p1_y > 0) {
        p1_y -= 10;
      }
    }
  }
}


void movePaddle2() {
  bool paddleMoved = false;
  unsigned long currentTime = millis();
  if (currentTime - lastMoveTimePaddleB >= updateDelay) {
    if (digitalRead(button_C) == LOW) {  // Move paddle 2 down
      if (p2_y < 190) {
        p2_y += 10;
      }
    }
    
    if (digitalRead(button_D) == LOW) {  // Move paddle 2 up
      if (p2_y > 0) {
        p2_y -= 10;
      }
    }
  }
}

void drawPaddles() {
  gfx.fillRect(p1_x, p1_y, 15, 50, vga.rgb(0, 0, 255));  // Blue paddle for Player 1
  gfx.fillRect(p2_x, p2_y, 15, 50, vga.rgb(255, 0, 0));  // Red paddle for Player 2
}

void resetBall() {
  // Reset the ball to the center of the screen
  ballX = screenWidth / 2;
  ballY = screenHeight / 2;

  // Reset the velocity (can be randomized or set to default)
  
  balldX = (random(0, 2) == 0) ? -2 : 2;  // Randomly start left or right
  balldY = (random(0, 2) == 0) ? -1 : 1;  // Randomize vertical velocity

  // Optionally, add a delay before restarting
  delay(250);  // Half a second delay before resuming
}

void ballMovement() {
    unsigned long currentTime = millis();
    if (currentTime - lastBallMoveTime > ballMoveInterval) {
        gfx.fillRect(ballX, ballY, bwidth, bheight, vga.rgb(0, 0, 0)); // Clear previous ball position
        
        // Check for potential new position
        int newBallX = ballX + balldX;
        int newBallY = ballY + balldY;

        // Check for collisions with the top and bottom edges
        if (newBallY - bheight <= 0 || newBallY + bheight >= screenHeight) {
            balldY = -(balldY + ((random(0, 2) == 0) ? 0 : 2));  // Reverse Y direction (bounce)
            newBallY = ballY + balldY; // Update the new position after bounce
        }

        // Check for collisions with the left wall (Player 2 scores)
        if (newBallX - bwidth <= 0) {
            player2Score++;      // Increment Player 2's score
            resetBall();         // Reset the ball position
            return;              // Exit to prevent further processing
        }

        // Check for collisions with the right wall (Player 1 scores)
        if (newBallX + bwidth >= screenWidth) {
            player1Score++;      // Increment Player 1's score
            resetBall();         // Reset the ball position
            return;              // Exit to prevent further processing
        }

        // Check for collisions with Paddle 1 (on the left)
        if (newBallX - bwidth <= p1_x + 15 && newBallY >= p1_y && newBallY <= p1_y + 50) {
            balldX = -balldX;  // Reverse X direction (bounce)
            // Position the ball just outside the paddle
            newBallX = p1_x + 15 + bwidth; // Move ball to right edge of Paddle 1
        }

        // Check for collisions with Paddle 2 (on the right)
        if (newBallX + bwidth >= p2_x && newBallY >= p2_y && newBallY <= p2_y + 50) {
            balldX = -balldX;  // Reverse X direction (bounce)
            // Position the ball just outside the paddle
            newBallX = p2_x - bwidth; // Move ball to left edge of Paddle 2
        }

        // Update ball position
        ballX = newBallX;
        ballY = newBallY;

    }
}

void drawScoreboard() {
    gfx.setTextSize(0);
    gfx.setCursor(10, 10); // Position for Player 1 score
    gfx.print("Player 1: ");
    gfx.print(player1Score); // Display Player 1 score
  
    gfx.setCursor(200, 10); // Position for Player 2 score
    gfx.print("Player 2: ");
    gfx.print(player2Score); // Display Player 2 score
}


void drawBall() {
  gfx.fillRect(ballX, ballY, bwidth, bheight, vga.rgb(0, 255, 0));
  gfx.drawLine(screenWidth/2, 0, screenWidth/2, screenHeight, vga.rgb(255,255,0));
}

void drawScreen() {
  vga.clear(vga.rgb(0, 0, 0));  // Clear the screen
  drawPaddles();                 // Redraw both paddles
  drawBall();
  drawScoreboard();
  vga.show();                    // Show updated frame
  
}

void gameOver() {
  if (player1Score >= 10) {
    vga.clear(vga.rgb(0x80,0,0));
    gfx.setTextSize(0);
    gfx.setCursor(100, 110);
    gfx.print("Player A Wins!");
    gfx.setCursor(90, 120);
    gfx.print("Press reset to play again");
    vga.show();
  }
  else if (player2Score >= 10) {
    vga.clear(vga.rgb(0x80,0,0));
    gfx.setTextSize(0);
    gfx.setTextColor(vga.rgb(0,0,0));
    gfx.setCursor(100, 110);
    gfx.print("Player B Wins!");
    gfx.setCursor(90, 120);
    gfx.print("Press reset to again");
    vga.show(); 
  }
}

//the loop is done every frame
void loop() {
  bool gameIsOver = false;

  // Check if the game is over first
  if (player1Score >= 10 || player2Score >= 10) {
    gameIsOver = true;
    gameOver();  // Display the Game Over screen
  } else {
    // Move both paddles independently only if the game is still ongoing
    movePaddle1();  // Handle movement for paddle 1
    movePaddle2();  // Handle movement for paddle 2
    ballMovement();  // Update ball movement
    drawScreen();    // Draw the current game state
  }

  // If game is over, just show the game over screen
  if (gameIsOver) {
    gameOver();  // Ensure the Game Over screen is displayed
  }
  
}
