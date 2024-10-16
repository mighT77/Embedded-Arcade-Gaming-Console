#include "menu.h" 
#include "pong.h"
#include "snake.h"
#include "tetris.h"  
#include "game_state.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern GameState currentGameState;
int menuIndex = 0;
extern bool menuNeedsUpdate;

bool isPongSubMenu = false;  
bool isComputerVsPlayer = false;  
void displayMenu() {
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);  
    const char *menuItems[] = {"Start Pong", "Start Snake", "Start Tetris", "Quit"};
    const char *pongSubMenuItems[] = {"Computer vs Player", "Player vs Player", "Back"};  
    
    int maxMenuItems = isPongSubMenu ? 3 : 4;  
    int screenWidth = tft.width();
    int screenHeight = tft.height();
    
    // Item height
    int itemHeight = 50;  
    int startY = (screenHeight - (maxMenuItems * itemHeight)) / 2;

    for (int i = 0; i < maxMenuItems; i++) {
        int y = startY + i * itemHeight;
        if (i == menuIndex) {
            tft.fillRect(0, y - itemHeight / 2, screenWidth, itemHeight, TFT_GREEN);
            tft.setTextColor(TFT_BLACK, TFT_GREEN);  
        } else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK);  
        }
        if (isPongSubMenu) {
            tft.drawString(pongSubMenuItems[i], screenWidth / 2, y); 
        } else {
            tft.drawString(menuItems[i], screenWidth / 2, y);  
        }
    }
}

void handleMenuInput(int button) {
    if (isPongSubMenu) {
        if (button == 2) {  // Up button
            menuIndex = (menuIndex > 0) ? menuIndex - 1 : 2;  // Wrap around
            menuNeedsUpdate = true;
        }
        if (button == 1) {  // Select button
            if (menuIndex == 0) {
                isComputerVsPlayer = true; 
                currentGameState = PONG;  
                setupPong(); 
            } else if (menuIndex == 1) {
                isComputerVsPlayer = false; 
                currentGameState = PONG;  
                setupPong();  
            } else if (menuIndex == 2) {
                isPongSubMenu = false; 
                menuIndex = 0;  
                menuNeedsUpdate = true;
            }
        }
    } else {
        if (button == 2) {  
            menuIndex = (menuIndex > 0) ? menuIndex - 1 : 3;
            menuNeedsUpdate = true;
        }
        if (button == 1) { 
            if (menuIndex == 0) {
                isPongSubMenu = true; 
                menuIndex = 0;  
                menuNeedsUpdate = true;
            } else if (menuIndex == 1) {
                currentGameState = SNAKE;
                tft.fillScreen(TFT_BLACK);
                setupSnake();
            } else if (menuIndex == 2) {
                currentGameState = TETRIS;
                tft.fillScreen(TFT_BLACK);
                setupTetris();
            } else if (menuIndex == 3) {
                Serial.println("Exiting...");
            }
        }
    }
}
