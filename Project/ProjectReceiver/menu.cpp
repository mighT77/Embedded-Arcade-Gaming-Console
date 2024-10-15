#include "menu.h" 
#include "pong.h"
#include "snake.h"
#include "tetris.h"  
#include "game_state.h"
#include <TFT_eSPI.h>
#include <ESP32S3VGA.h>
#include <GFXWrapper.h>

extern VGA vga;
extern Mode mode = Mode::MODE_640X480X60;
extern <VGA> gfx(vga, mode.hRes, mode.vRes);
//extern TFT_eSPI tft;
extern GameState currentGameState;
int menuIndex = 0;
extern bool menuNeedsUpdate;

bool isPongSubMenu = false;  

void displayMenu() {
    gfx.setRotation(0);
    gfx.fillScreen(TFT_BLACK);
    gfx.setTextSize(2);
    gfx.setTextDatum(MC_DATUM);  
    const char *menuItems[] = {"Start Pong", "Start Snake", "Start Tetris", "Quit"};
    const char *pongSubMenuItems[] = {"Computer vs Player", "Player vs Player", "Back"};
    
    int maxMenuItems = isPongSubMenu ? 3 : 4;  
    int screenWidth = gfx.width();
    int screenHeight = gfx.height();
    
    // Item height
    int itemHeight = 50;  
    int startY = (screenHeight - (maxMenuItems * itemHeight)) / 2;

    for (int i = 0; i < maxMenuItems; i++) {
        int y = startY + i * itemHeight;
        if (i == menuIndex) {
            gfx.fillRect(0, y - itemHeight / 2, screenWidth, itemHeight, TFT_GREEN);
            gfx.setTextColor(vga.rgb(0,0,0), vga.rgb(0,0x80,0));  
        } else {
            gfx.setTextColor(vga.rgb(0x80,0x80,0x80), vga.rgb(0,0,0));  
        }
        if (isPongSubMenu) {
            gfx.drawString(pongSubMenuItems[i], screenWidth / 2, y); 
        } else {
            gfx.drawString(menuItems[i], screenWidth / 2, y);  
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
                currentGameState = PONG;  // Start Computer vs Player
                vga.clear(vga.rgb(0,0,0));
                setupPong(); 
            } else if (menuIndex == 1) {
                currentGameState = PONG;  
                vga.clear(vga.rgb(0,0,0));
                setupPong();  
            } else if (menuIndex == 2) {
                isPongSubMenu = false;  // Go back to the main menu
                menuIndex = 0;  // Reset to the first item
                menuNeedsUpdate = true;
            }
        }
    } else {
        // Handle input for the main menu
        if (button == 2) {  // Up button
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
