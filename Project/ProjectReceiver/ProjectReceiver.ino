#include <esp_now.h>
#include <WiFi.h>
#include "menu.h"
#include "pong.h"
//#include "snake.h"
//#include "tetris.h"  // Include Tetris
#include "game_state.h"
#include "message_struct.h"

#include "ESP32S3VGA.h"
#include <GfxWrapper.h>


//VGA Device
VGA vga;
Mode mode = Mode::MODE_640x480x60;
GfxWrapper<VGA> gfx(vga, mode.hRes, mode.vRes);


const PinConfig pins(-1,-1,-1,-1,1,  -1,-1,-1,-1,-1,2,  -1,-1,-1,-1,3,  10,11);

// Global variables
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
struct_message message;
GameState currentGameState = MENU;
bool menuNeedsUpdate = true;

// Callback when data is received via ESP-NOW
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {

    memcpy(&message, incomingData, sizeof(message));
    
    if (currentGameState == MENU) {
        handleMenuInput(message.button);  // Handle menu navigation
    } else if (currentGameState == PONG) {
        handlePongInput(message.button);  // Handle Pong input
    }/* else if (currentGameState == SNAKE) {
        handleSnakeInput(message.button); // Handle Snake input
    } else if (currentGameState == TETRIS) {
        handleTetrisInput(message.button);  // Handle Tetris input
    }*/
}

void setup() {
    Serial.begin(115200);
    
    /*tft.init();  // Initialize the display
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);*/

    vga.bufferCount = 2;
    if(!vga.init(pins, mode, 16)) while(1) delay(1);
    vga.clear(vga.rgb(0, 0, 0));
  
    vga.start();
    
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(OnDataRecv);  // Register ESP-NOW receive callback
    
    displayMenu();  // Display the initial menu
    vga.show();
}

void loop() {
    if (menuNeedsUpdate && currentGameState == MENU) {
        displayMenu();
        menuNeedsUpdate = false;
    } else if (currentGameState == PONG) {
        updatePongGame();  // Update the Pong game
   /* } else if (currentGameState == SNAKE) {
        updateSnakeGame();  // Update the Snake game
    } else if (currentGameState == TETRIS) {
        updateTetrisGame();  // Update the Tetris game
    }*/
  }
}
