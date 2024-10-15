#include <esp_now.h>
#include <ESP32S3VGA.h>
#include <GFXWrapper.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "menu.h"
#include "pong.h"
#include "snake.h"
#include "tetris.h"  
#include "game_state.h"
#include "message_struct.h"


//VGA Setup
VGA vga;
Mode mode = Mode::MODE_640X480X60;
GFXWrapper<VGA> gfx(vga, mode.hRes, mode.vRes);

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
        handleMenuInput(message.button);  
    } else if (currentGameState == PONG) {
        if (message.button >= 1 && message.button <= 4) {
            handlePongInput(message.button);  
        } else if (message.button >= 5 && message.button <= 8) {
            int player2Button = message.button - 4;  
            handlePongInput(player2Button);  
        }
    } else if (currentGameState == SNAKE) {
        Serial.println("Snake inputs");
        Serial.println(message.button);
        handleSnakeInput(message.button); 
    } else if (currentGameState == TETRIS) {
        handleTetrisInput(message.button); 
    }
}

void setup() {
    Serial.begin(115200);
    
    /*tft.init();  
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);*/
    vga.clear(vga.rgb(0,0,0));
    vga.start();
    gfx.setRotation(1);
    
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(OnDataRecv);  
    
    displayMenu(); 


}

void loop() {
    if (menuNeedsUpdate && currentGameState == MENU) {
        displayMenu();
        menuNeedsUpdate = false;
    } else if (currentGameState == PONG) {
        updatePongGame();  
    } else if (currentGameState == SNAKE) {
        updateSnakeGame();  
    } else if (currentGameState == TETRIS) {
        updateTetrisGame();  
    }
}
