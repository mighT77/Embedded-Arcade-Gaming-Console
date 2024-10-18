#include "menu.h"  
#include "pong.h"
// #include "snake.h"
// #include "tetris.h"  
#include "game_state.h"
#include "ESP32S3VGA.h"
#include <GfxWrapper.h>

extern VGA vga;
extern Mode mode;
extern GfxWrapper<VGA> gfx;
extern GameState currentGameState;
int menuIndex = 0;
extern bool menuNeedsUpdate;

bool isPongSubMenu = false;

void displayMenu() {
    gfx.fillScreen(vga.rgb(0, 0, 0));
    gfx.setTextSize(2);
    
    const char *menuItems[] = {"Start Pong", "Start Snake", "Start Tetris", "Quit"};
    const char *pongSubMenuItems[] = {"Computer vs Player", "Player vs Player", "Back"};  
    
    int maxMenuItems = isPongSubMenu ? 3 : 4;  
    int itemHeight = 30;  // Set the height for each item
    int startY = (480 - (maxMenuItems * itemHeight)) / 2; // Adjusted for 480 height

    for (int i = 0; i < maxMenuItems; i++) {
        if (i == menuIndex) {
            gfx.setTextColor(vga.rgb(0, 0x80, 0), vga.rgb(0, 0, 0));  // Selected item color
        } else {
            gfx.setTextColor(vga.rgb(0x80, 0x80, 0), vga.rgb(0, 0, 0));  // Default item color
        }

        // Draw the menu items without setting the cursor
        if (isPongSubMenu) {
            gfx.println(pongSubMenuItems[i]);
        } else {
            gfx.println(menuItems[i]);
        }
    }
    vga.show();
}

void handleMenuInput(int button) {
    if (isPongSubMenu) {
        if (button == 2) {  // Up button
            menuIndex = (menuIndex > 0) ? menuIndex - 1 : 2;  // Wrap around
            menuNeedsUpdate = true;
        }
        if (button == 1) {  // Select button
            if (menuIndex == 0) {
                currentGameState = PONG;  
                vga.clear(vga.rgb(0, 0, 0));
                setupPong(); 
            } else if (menuIndex == 1) {
                currentGameState = PONG;  
                vga.clear(vga.rgb(0, 0, 0));
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
                // currentGameState = SNAKE;
                // vga.clear(vga.rgb(0, 0, 0));
                // setupSnake();
            } else if (menuIndex == 2) {
                // currentGameState = TETRIS;
                // vga.clear(vga.rgb(0, 0, 0));
                // setupTetris();
            } else if (menuIndex == 3) {
                Serial.println("Exiting...");
            }
        }
    }
}
