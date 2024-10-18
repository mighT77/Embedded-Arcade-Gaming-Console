#include <esp_now.h> 
#include <WiFi.h>
#include "message_struct.h"  

struct_message message;

uint8_t receiver_mac_addr[] = {0x30, 0x30, 0xF9, 0x59, 0x26, 0x14}; 

const int buttonA = 17;  
const int buttonB = 18;  
const int buttonC = 43;  
const int buttonD = 44; 
const int hardD = 6;
esp_now_peer_info_t peerInfo;
#define LCDpin 15
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Last Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
    Serial.begin(115200);
    pinMode(LCDpin, OUTPUT); 
    digitalWrite(LCDpin, HIGH);
    WiFi.mode(WIFI_STA);
    Serial.println("ESP-NOW initialized");

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_send_cb(OnDataSent);

    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, receiver_mac_addr, 6); // Replace with the receiver's MAC address
    peerInfo.channel = 0;  // Set to the same channel for both devices
    peerInfo.encrypt = false;

    // Add peer to send data
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    // Set button pins as inputs with pull-up resistors
    pinMode(buttonA, INPUT_PULLUP);
    pinMode(buttonB, INPUT_PULLUP);
    pinMode(buttonC, INPUT_PULLUP);
    pinMode(buttonD, INPUT_PULLUP);
}

void loop() {
    int buttonState = 0; // Default: no button pressed

    // Read button states
    if (digitalRead(buttonA) == LOW) {
        buttonState = 5; // Left button pressed
    } 
    if (digitalRead(buttonB) == LOW) {
        buttonState = 6; // Right button pressed
    } 
    else if (digitalRead(buttonC) == LOW) {
        buttonState = 7; // Up button pressed
    } else if (digitalRead(buttonD) == LOW) {
        buttonState = 8; // Down button pressed
    }


    
    if (buttonState != 0) {
        message.button = buttonState;
        // Send the struct message via ESP-NOW to the Receiver TTGO
        esp_now_send(receiver_mac_addr, (uint8_t *)&message, sizeof(message));

        Serial.print("Sent button state: ");
        Serial.println(buttonState);

        delay(90);
    }
}
