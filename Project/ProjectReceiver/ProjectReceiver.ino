#include <esp_now.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "menu.h"
#include "pong.h"
#include "snake.h"
#include "game_state.h"
#include  "message_struct.h"
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
    } else if (currentGameState == SNAKE) {
        handleSnakeInput(message.button); // Handle Snake input
    }
}

void setup() {
    Serial.begin(115200);
    
    tft.init();  // Initialize the display
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(OnDataRecv);  // Register ESP-NOW receive callback
    
    displayMenu();  // Display the initial menu
}

void loop() {
    if (menuNeedsUpdate && currentGameState == MENU) {
        displayMenu();
        menuNeedsUpdate = false;
    } else if (currentGameState == PONG) {
        updatePongGame();  // Update the Pong game
    } else if (currentGameState == SNAKE) {
        updateSnakeGame();  // Update the Snake game
    }
}


