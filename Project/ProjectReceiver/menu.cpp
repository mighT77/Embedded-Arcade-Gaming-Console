#include "menu.h"
#include "pong.h"
#include "snake.h"
#include "tetris.h"  // Include Tetris
#include "game_state.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern GameState currentGameState;
int menuIndex = 0;
extern bool menuNeedsUpdate;

void displayMenu() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    const char *menuItems[] = {"Start Pong", "Start Snake", "Start Tetris", "Quit"};
    int maxMenuItems = 4;

    for (int i = 0; i < maxMenuItems; i++) {
        tft.setCursor(10, 30 + i * 30);
        if (i == menuIndex) {
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.print("> ");
        } else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        tft.println(menuItems[i]);
    }
}

void handleMenuInput(int button) {
    if (button == 2) {
        menuIndex = (menuIndex > 0) ? menuIndex - 1 : 3;
        menuNeedsUpdate = true;
    }
    if (button == 1) {
        if (menuIndex == 0) {
            currentGameState = PONG;
            tft.fillScreen(TFT_BLACK);
            setupPong();
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
