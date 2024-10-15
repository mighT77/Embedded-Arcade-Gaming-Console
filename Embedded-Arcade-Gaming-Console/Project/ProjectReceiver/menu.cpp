#include "menu.h"
#include "pong.h"
//#include "snake.h"
//#include "tetris.h"  // Include Tetris
#include "game_state.h"
#include "ESP32S3VGA.h"
#include <GfxWrapper.h>



extern VGA vga;
extern Mode mode;
extern GfxWrapper<VGA> gfx;
extern GameState currentGameState;
int menuIndex = 0;
extern bool menuNeedsUpdate;

void displayMenu() {
    gfx.fillScreen(vga.rgb(0,0,0));
    gfx.setTextSize(2);
    
    const char *menuItems[] = {"Start Pong", "Start Snake", "Start Tetris", "Quit"};
    int maxMenuItems = 4;

    for (int i = 0; i < maxMenuItems; i++) {
        gfx.setCursor(10, 30 + i * 30);
        if (i == menuIndex) {
            gfx.setTextColor(vga.rgb(0,0x80,0), vga.rgb(0,0,0));
            gfx.print("> ");
        } else {
            gfx.setTextColor(vga.rgb(0x80,0x80,0), vga.rgb(0,0,0));
        }
        gfx.println(menuItems[i]);
    }
    vga.show();
}

void handleMenuInput(int button) {
    if (button == 2) {
        menuIndex = (menuIndex > 0) ? menuIndex - 1 : 3;
        menuNeedsUpdate = true;
    }
    if (button == 1) {
        if (menuIndex == 0) {
            currentGameState = PONG;
            vga.clear(vga.rgb(0,0,0));
            setupPong();
        } else if (menuIndex == 1) {
            //currentGameState = SNAKE;
            vga.clear(vga.rgb(0,0,0));
            //setupSnake();
        } else if (menuIndex == 2) {
            //currentGameState = TETRIS;
            vga.clear(vga.rgb(0,0,0));
            //setupTetris();*/
        } else if (menuIndex == 3) {
            Serial.println("Exiting...");
        }
    }
}
