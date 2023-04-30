#include <LedControl.h>

#include <esp_now.h>
#include <WiFi.h>

LedControl lc=LedControl(23,18,13,1);

int tick;

// Structure example to receive data
// Must match the sender structure
struct struct_message {
  int id;
  int data;
  int tick;
};

struct struct_data {
  int id;
  int data;
  int tick;
  bool connected;
};

bool disconnected[8];

// Create a struct_message called myData
struct_message in_data;

// Create an array with all the structures
struct_data controller_data[8];

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&in_data, incomingData, sizeof(in_data));
  controller_data[in_data.id].data = in_data.data;
  if (controlelr_data[in_data.id].connected == true) {
    SetRow(in_data.id, controller_data[in_data.id].data);
  }
  controller_data[in_data.id].tick = in_data.tick;
  disconnected[in_data.id] = false;
  if (controller_data[in_data.id].tick == in_data.tick) {
    tick++;
    if (tick == 20) {
      tick = 0;
      disconnected[in_data.id] = true;
    }
  }
  
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
  lc.setRow(0, row, B00000000);
  for (int x = 0; x <= 7-height; x++) {
    lc.setLed(0, row, x, true);
  }
}

void flash(int row) {
  lc.setRow(0, row, B00000000);
  for (int x = 0; x <= 7; x++) {
    if (x % 2 = 0) {
      lc.setLed(0, row, x, true);
    }
  }
}

void loop() {
  for (int i = 0; i < 8; i++) {
    if (disconnected[i]) {
      flash(i);
    }
  }
}
