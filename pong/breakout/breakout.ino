#include "ESP32S3VGA.h"
#include <GfxWrapper.h>
#include "breakout.h"

const int button_A = 44;
const int button_B = 18;
const int button_C = 17;
const int button_D = 11;


unsigned long lastPaddleMoveTime = 0; // Time when the paddle last moved
const unsigned long paddleMoveInterval = 150;

//VGA Device
VGA vga;
Mode mode = Mode::MODE_320x240x60;
GfxWrapper<VGA> gfx(vga, mode.hRes, mode.vRes);

int screenWidth = mode.hRes;
int screenHeight = mode.vRes;


//                   r,r,r,r,r,  g,g, g, g, g, g,   b, b, b, b, b,   h,v
const PinConfig pins(-1,-1,-1,-1,1,  -1,-1,-1,-1,-1,2,  -1,-1,-1,-1,43,  10,11);

void setup() {
  // put your setup code here, to run once:
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
    setupBreakout();
    vga.show();
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime = millis();
  if (currentTime - lastPaddleMoveTime >= paddleMoveInterval) {
        if (digitalRead(button_A) == LOW) {  // Move paddle left
            movePaddleLeft();// Update the last movement time
        } else if (digitalRead(button_B) == LOW) {  // Move paddle right
            movePaddleRight(); // Update the last movement time
        }
    }

   updateBreakoutGame();
   drawBreakoutElements();
}
