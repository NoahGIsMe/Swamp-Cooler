#include <Adafruit_Sensor.h> //Water level sensor library
#include <DHT.h> //Temperature and humidity sensor library
#include <ezButton.h> //Button library

//Pinout definitions to make the program easier to read
#define blueLED 13
#define greenLED 12
#define redLED 11
#define yellowLED 10
#define waterPin 9
#define DHTPIN 8
#define motorPin 7
#define buttonPin 4
#define waterSignal A5
#define OFF 0
#define ON 1
#define DHTTYPE DHT11

float h;
float f;

int waterLevel = 0;
int loopState = OFF; //program is initially OFF, needs button press to turn ON

DHT dht = DHT(DHTPIN, DHTTYPE); //DHT11 initialization
ezButton button(buttonPin); //Button initialization

void setup() {
  Serial.begin(9600);
  button.setDebounceTime(50);

  dht.begin();
  
  pinMode(waterPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(blueLED, OUTPUT); //Indicates that the motor is ON
  pinMode(greenLED, OUTPUT); //Indicates that the swamp cooler is ON
  pinMode(redLED, OUTPUT); //Indicates that water level is too LOW
  pinMode(yellowLED, OUTPUT); //Indicates that either the humidity is too high or the ambient temperature is too low
  pinMode(buttonPin, INPUT_PULLUP); //This program uses a pullup resistor with the ON/OFF button
  
  digitalWrite(waterPin, LOW);
}

void loop() {
  button.loop(); //Button library initialization
  
  if (button.isPressed()) {
    loopState =! loopState; //If the button is pressed, the state of void loop() is updated
  }

  switch (loopState) {
    case ON: //If the button is pressed, turn ON all of the sensors and the green LED
      temperatureHumidity();
      waterSensor();
      digitalWrite(greenLED, HIGH);

      if (waterLevel < 100) { //If the water level is too low, turn ON the red LED and turn OFF the motor and blue LED
        digitalWrite(redLED, HIGH);
        digitalWrite(blueLED, LOW);
        digitalWrite(motorPin, LOW);
      }

      if (h > 70 || f < 75) { //If the humidity is too high OR the temperature is too LOW, turn ON the yellow LED and turn OFF the motor and blue LED
        digitalWrite(yellowLED, HIGH);
        digitalWrite(blueLED, LOW);
        digitalWrite(motorPin, LOW);
      }

      if (waterLevel > 100 && h < 70 && f > 75) { //If the water ;evel is high enough AND the humidity is low enough AND the temperature is high enough, turn ON the motor and blue LED, and turn OFF the red and yellow LEDs
        digitalWrite(motorPin, HIGH);
        digitalWrite(blueLED, HIGH);
        digitalWrite(yellowLED, LOW);
        digitalWrite(redLED, LOW);    
      }
    break;

    case OFF: //If the button is pressed again, turn OFF the swamp cooler
      turnOff();
    break;
  }
}

void waterSensor() { //Measures the water level of the fill tank when called. waterPin is periodically set to LOW with a delay to prevent damage to the sensor
  digitalWrite(waterPin, HIGH);
  delay(10);
  
  waterLevel = analogRead(waterSignal);
  digitalWrite(waterPin, LOW);

  Serial.print("Sensor value: ");
  Serial.println(waterLevel);
  delay(1000);
}

void temperatureHumidity() { //Measures the ambient temperature in "F" and humidity in "%" when called
  delay(2000);
  
  h = dht.readHumidity();
  f = dht.readTemperature(true);
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" % ");
  Serial.print("Temperature: ");
  Serial.print(f);
  Serial.print(" \xC2\xB0");
  Serial.print("F ");
}

void turnOff() { //Turns OFF the swamp cooler when called
  digitalWrite(blueLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(waterPin, LOW);
  digitalWrite(DHTPIN, LOW);
  digitalWrite(motorPin, LOW);
  digitalWrite(buttonPin, LOW);
  analogWrite(waterSignal, LOW);
}
