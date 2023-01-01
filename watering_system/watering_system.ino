#include <SPI.h>
#include <SD.h>

int relayPin = 6;
int rainPin = A0;
int trigPin = 2;
int echoPin = 3;

int misoPin = 12;
int mosiPin = 11;
int sckPin = 13;
int csPin = 4;

int timeout = 43200000; // milliseconds
int wateringTime = 3;

void setup(){
  // Register Pins
  pinMode(relayPin, OUTPUT);
  pinMode(rainPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // init SD card module
  if (!SD.begin(csPin)) {
    return;
  }

  Serial.begin(9600);  // debug
}

void loop() {
  // delay 12 hours
  unsigned long startMillis = millis();
  while (millis() - startMillis < timeout);
  
  // write sensor data
  String fileOutput = generateFileName();
  int sensorValue = analogRead(rainPin);
  bool flag = saveToFile(fileOutput, String(sensorValue));

  if(isMoist(sensorValue) == false){
    Serial.println("Ground is dry!"); // debug    
    if(checkWater() == true){
      Serial.println("Water full");
      startWatering(wateringTime);
    }
    else{
      Serial.println("No water!"); // debug
      // TODO
    }
  }
  else{
    Serial.println("Everything ok"); // debug    
  }
}

/* This function takes a duration in seconds
 *  and power on water pump connected by relay
 */
void startWatering(int duration){
  int millis = duration * 1000;
	digitalWrite(relayPin, LOW);
	delay(millis);
	digitalWrite(relayPin, HIGH);
}

/* This function check moisture of the ground
 *  and return True/False
 * YL-69 sensor 
 */
bool isMoist(int sensorValue){
  int thresholdValue = 750;
  return sensorValue < thresholdValue;
}

/* This function check water level of the tank
 *  and return True/False
 * HC-SR04 sensor 
 */
bool checkWater(){
  int borderValue = 2;  
  long echoTime, distance;

  // HC-SR04 example
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  
  digitalWrite(trigPin, LOW);
  
  echoTime = pulseIn(echoPin, HIGH);

  // Convert time to distance
  distance = (echoTime / 2) / 29.1;

  // TODO Add check > 3000 It value then sensor too close
  return distance > borderValue;
}

String generateFileName(){
  unsigned long day = 86400000;
  unsigned long daysFromLaunch = millis() / day;
  String a = String(daysFromLaunch);
  String fileName = a + ".txt";

  return fileName;
}

/* This function save string data to file on SD card
 */
bool saveToFile(String fn, String data){
  File dataFile = SD.open(fn, FILE_WRITE);
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
    
    return true;
  }
 
  return false;
}