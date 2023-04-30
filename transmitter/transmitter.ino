#include "espnow.h"
#include "SPIFFS.h"

#define TRIG_PIN 13
#define ECHO_PIN 14
#define SOUND_SPEED 0.034
#define SET_MODE 15
#define SET_MIN 4
#define SET_MAX 2

const int dip[4] = {5, 18, 19, 21};

long duration;
int distanceCm;

int NUMBER = 0;
int data;
bool update = false;

int minVal; //change back later
int maxVal; //change back later
int pos;

void initSPIFFS() {
  Serial.println("hello");
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }
  Serial.println("SPIFFS mounted successfully");
  File file_min = SPIFFS.open("/min.txt");
  File file_max = SPIFFS.open("/max.txt");

  String max = "";
  String min = "";

  while(file_max.available()){
    max += char(file_max.read());
  }
  maxVal = max.toInt();  
  file_max.close();

  while(file_min.available()){
    min += char(file_min.read());
  }
  minVal = min.toInt();
  file_min.close();
  Serial.printf("Min, max %d, %d\n", minVal, maxVal);
}

void upFiles(int i, int val) {
  Serial.println("upfiles");
  File file;
  switch(i) {
    case 1:
      if (SPIFFS.exists("/min.txt")){
        Serial.println("File exists");
        SPIFFS.remove("/min.txt");
      } else {
        Serial.println("File does not exist");
      }
      file = SPIFFS.open("/min.txt", FILE_WRITE, true);
      file.println(val);
      break;
    case 2:
      if (SPIFFS.exists("/max.txt")){
        Serial.println("File exists");
        SPIFFS.remove("/max.txt");
      } else {
        Serial.println("File does not exist");
      }
      file = SPIFFS.open("/max.txt", FILE_WRITE, true);
      file.println(val);
      break;
  }
}

void initPins() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SET_MODE, INPUT_PULLUP);
  pinMode(SET_MIN, INPUT_PULLUP);
  pinMode(SET_MAX, INPUT_PULLUP);
  for(int i = 0; i < 4; i++) {
    pinMode(dip[i], INPUT_PULLUP);
  }
  if (digitalRead(dip[0]) == LOW) {
    NUMBER += 1;
  }
  if (digitalRead(dip[1]) == LOW) {
    NUMBER += 2;
  }
  if (digitalRead(dip[2]) == LOW) {
    NUMBER += 4;
  }
  if (digitalRead(dip[3]) == LOW) {
    NUMBER += 8;
  }
  Serial.println(NUMBER);
}

void setup() {
  Serial.begin(115200);
  delay(100);
  espNowInit();
  initSPIFFS();
  initPins();
}
 
void loop() {
  // Clears the trigPin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;

  if (digitalRead(SET_MODE) == LOW) {
    if (digitalRead(SET_MIN) == LOW) {
      minVal = distanceCm;
      upFiles(1, minVal);
    } else if (digitalRead(SET_MAX) == LOW) {
      maxVal = distanceCm;
      upFiles(2, maxVal);
    }
    Serial.printf("Min, max %d, %d\n", minVal, maxVal);
  } else { 
    // Prints the distance in the Serial Monitor
    if (distanceCm > maxVal) {
      distanceCm = maxVal;
    } else if (distanceCm < minVal) {
      distanceCm = minVal;
    }
    pos = map(distanceCm, minVal, maxVal, 0, 7);
    Serial.print("Distance (cm): ");
    Serial.println(distanceCm);
    if (data != pos) {
      update = true;
    }
    data = pos;
  }
  delay(100);
}
