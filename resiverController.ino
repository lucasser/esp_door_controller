#include <LedControl.h>

#include <esp_now.h>
#include <WiFi.h>

LedControl lc=LedControl(23,18,13,1);

// Structure example to receive data
// Must match the sender structure
struct struct_message {
  int id;
  int data;
};

// Create a struct_message called myData
struct_message in_data;

// Create an array with all the structures
struct_message controller_data[8];

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&in_data, incomingData, sizeof(in_data));
  controller_data[in_data.id].data = in_data.data;
  SetRow(in_data.id, controller_data[in_data.id].data);
}
 
void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);

  lc.shutdown(0,false);
}

void SetRow(int row, int height) {
  Serial.print(row);
  Serial.print(height);
  lc.setColumn(0, 7 - row, B00000000);
  for (int x = 0; x <= height; x++) {
    lc.setLed(0, x, 7 - row, true);
  }
}
 
void loop() {
  /*for (int i = 0; i < 8; i++) {
    Serial.print(controller_data[i].data);
  }
  Serial.println("");*/
}
