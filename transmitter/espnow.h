#include "esp32-hal.h"
#include <esp_now.h>
#include <WiFi.h>

TaskHandle_t esp_now_task;
extern int NUMBER;
extern int data;
extern bool update;
int tick;

// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0x40, 0x22, 0xD8, 0x5F, 0xF4, 0x7C};

// Structure example to send data
// Must match the receiver structure
struct struct_message {
    int id; // must be unique for each sender board
    int data;
    int tick;
};

// Create a struct_message called out_data
struct_message out_data;

// Create peer interface
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void espNowLoop(void * pvParameters) {
  for(;;) {
    out_data.id = NUMBER;
    out_data.data = data;
    out_data.tick = tick;
    if (tick == 300) {
      tick = 0;
      esp_now_send(broadcastAddress, (uint8_t *) &out_data, sizeof(out_data));
    } else if (update == true) {
      update = false;
      esp_now_send(broadcastAddress, (uint8_t *) &out_data, sizeof(out_data));
    }
    tick++;
    delay(10);
  }
}

void espNowInit() {
   // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  xTaskCreatePinnedToCore(espNowLoop, "esp_now_task", 10000, NULL, 1, &esp_now_task, 0);
}
