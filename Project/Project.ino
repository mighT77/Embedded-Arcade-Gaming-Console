#include <esp_now.h>
#include <WiFi.h>
#include "message_struct.h"  // Include the struct definition

// Create a message instance
struct_message message;

// Replace with the actual MAC address of the Receiver TTGO
uint8_t receiver_mac_addr[] = {0x30, 0x30, 0xF9, 0x59, 0x26, 0x14};  // Example MAC Address

// Button pins
const int buttonA = 0;  // Left movement
const int buttonB = 14; // Right movement
esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station (STA mode)
  WiFi.mode(WIFI_STA);
  Serial.println("ESP-NOW initialized");

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register send callback function
  esp_now_register_send_cb(OnDataSent);

  // Add peer (Receiver TTGO)
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
}

void loop() {
  int buttonState = 0; // Default: no button pressed

  // Read button states
  if (digitalRead(buttonA) == LOW) {
    buttonState = 1; // Left button pressed
  } else if (digitalRead(buttonB) == LOW) {
    buttonState = 2; // Right button pressed
  }

  // Send button data if any movement is detected
  if (buttonState != 0) {
    message.button = buttonState;
    // Send the struct message via ESP-NOW to the Receiver TTGO
    esp_now_send(receiver_mac_addr, (uint8_t *)&message, sizeof(message));

    Serial.print("Sent button state: ");
    Serial.println(buttonState);

    // Debounce delay to avoid multiple signals for a single press
    delay(200);
  }
}
