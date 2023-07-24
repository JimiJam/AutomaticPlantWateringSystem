#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int relayPin = 6;
int rainPin = A0;
int trigPin = 2;
int echoPin = 3;

int timeout = 60*60*1000; // 60 minutes
int wateringTime = 3;
unsigned long lastWatering = 0;

void setup(){
  // Register Pins
  pinMode(relayPin, OUTPUT);
  pinMode(rainPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);  // debug
  while (!Serial) ; // wait until Arduino Serial Monitor opens

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  display.clearDisplay();

  // init relay
  digitalWrite(relayPin, HIGH);
}

void loop() {
  unsigned long startMillis = millis();
  
  int sensorValue = analogRead(rainPin);
  String screen_data = "Last watering: \n" + String(lastWatering) + " hours ago\n" + "Moist value:" + String(sensorValue);

  // check water level
  bool water_full = checkWaterLevel();
  if(water_full == false){
    screen_data = "No water!";
  }

  showOnDisplay(screen_data);
  Serial.println(screen_data); // debug

  if(isMoist(sensorValue) == false){
    Serial.println("Ground is dry!"); // debug   
    if(water_full == true){
      Serial.println("Start watering"); // debug
      startWatering(wateringTime);
      lastWatering = millis() / 60*60*1000; // hours
    }
  }
  else{
    Serial.println("Everything ok"); // debug
  }

  while (millis() - startMillis < timeout); // delay timeout
}

/* This function takes a duration in seconds
 *  and power on water pump connected by relay
 */
void startWatering(int duration){
  unsigned long timing = millis();
  int timeout = duration * 1000;

	digitalWrite(relayPin, LOW);
	while (millis() - timing < timeout);
	digitalWrite(relayPin, HIGH);
}

/* This function check moisture of the ground
 *  and return True/False
 * YL-69 sensor 
 */
bool isMoist(int sensorValue){
  int thresholdValue = 350;
  return sensorValue < thresholdValue;
}

/* This function check water level of the tank
 *  and return True/False
 * HC-SR04 sensor 
 */
bool checkWaterLevel(){
  int borderValue = 11;  
  long echoTime, distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  
  digitalWrite(trigPin, LOW);
  
  echoTime = pulseIn(echoPin, HIGH);

  distance = (echoTime / 2) / 29.1; // Convert time to distance

  return distance < borderValue;
}

/* This function show screen_data on display
*/
bool showOnDisplay(String screen_data){
  display.clearDisplay(); // Clear display buffer
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(screen_data);
  display.display();      // Show initial text
}
