int relayPin = 6;
int rainPin = A0;
int trigPin = 12;
int echoPin = 11;

void setup(){
  // Register Pins
  pinMode(relayPin, OUTPUT);
  pinMode(rainPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);  // debug
}

void loop() {
  int timeout = 500; //  milliseconds
  
  delay(timeout); 

  if(isMoist() == false){
    Serial.println("Ground is dry!"); // debug    
    if(checkWater() == true){
      Serial.println("Water full");
      startWatering(3);
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
bool isMoist(){
  int thresholdValue = 800;
  int sensorValue = analogRead(rainPin);

  Serial.println(sensorValue);
  return sensorValue < thresholdValue;
}

/* This function check water level of the tank
 *  and return True/False
 * HC-SR04 sensor 
 */
bool checkWater(){
  int borderValue = 2;  
  long echoTime, distance;

  // HC -SR04 example
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