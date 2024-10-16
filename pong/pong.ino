#include <esp_now.h>
#include <WiFi.h>
#include "pong.h"

#include "ESP32S3VGA.h"
#include <GfxWrapper.h>

const int button_A = 10;
const int button_B = 14;
const int button_C = 17;
const int button_D = 18;


//VGA Device
VGA vga;
Mode mode = Mode::MODE_320x240x60;
GfxWrapper<VGA> gfx(vga, mode.hRes, mode.vRes);

int screenWidth = mode.hRes;
int screenHeight = mode.vRes;


const PinConfig pins(-1,-1,-1,-1,1,  -1,-1,-1,-1,-1,2,  -1,-1,-1,-1,3,  10,11);

void setup() {
    Serial.begin(115200);

    pinMode(button_A, INPUT_PULLUP);
    pinMode(button_B, INPUT_PULLUP);

    pinMode(button_C, INPUT_PULLUP);
    pinMode(button_D, INPUT_PULLUP);

    vga.bufferCount = 2;
    if(!vga.init(pins, mode, 16)) while(1) delay(1);
    vga.clear(vga.rgb(0, 0, 0));
  
    vga.start();

    /*WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(OnDataRecv); */ // Register ESP-NOW receive callback
    initializeGame();
    vga.show();
}


void loop() {
  // Check button states and move paddles accordingly
    if (digitalRead(button_A) == LOW) {  // Assuming active-low buttons
        movePaddle1Up();
    } else if (digitalRead(button_B) == LOW) {
        movePaddle1Down();
    }

    if (digitalRead(button_C) == LOW) {
        movePaddle2Up();
    } else if (digitalRead(button_D) == LOW) {
        movePaddle2Down();
    }

    // Update the rest of the game
    updateGame();
}
