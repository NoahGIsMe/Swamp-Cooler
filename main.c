#include <Adafruit_Sensor.h> //Water level sensor library
#include <DHT.h> //Temperature and humidity sensor library
#include <ezButton.h> //Button library
#include <LiquidCrystal.h> //LCD library

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
const int rs = 6, en = 5, d4 = 3, d5 = 2, d6 = 1, d7 = 0;

DHT dht = DHT(DHTPIN, DHTTYPE); //DHT11 initialization
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
ezButton button(buttonPin); //Button initialization

void setup() {
  Serial.begin(9600);
  button.setDebounceTime(50);
  
  lcd.begin(16,2); //16x2 LCD screen size
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
    loopState =! loopState; //If the button is pressed, the state of void loop() is updated using SWITCH-CASE
  }

  switch (loopState) {
    case ON: //If the button is pressed, turn ON all of the sensors and the green LED
      temperatureHumidity();
      waterSensor();
      digitalWrite(greenLED, HIGH);
      
      LCD();

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

      if (waterLevel > 100 && h < 70 && f > 75) { //If the water level is high enough AND the humidity is low enough AND the temperature is high enough, turn ON the motor and blue LED, and turn OFF the red and yellow LEDs
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

void waterSensor() { //Measures the water level of the fill tank when called. waterPin is periodically toggled from HIGH to LOW with a delay to prevent damage to the sensor
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
}

void LCD() {
  lcd.clear(); //Resets the screen for new data to be displayed
  
  //Displays humidity in "%" on the top row of the LCD
  lcd.setCursor(0,0);
  lcd.print("Humidity: ");
  lcd.print(h);
  lcd.print(" % ");
  
  //Displays temperature in "F" on the bottom row of the LCD
  lcd.setCursor(0,1);
  lcd.print("Temperature: ");
  lcd.print(f);
  lcd.print(" \xC2\xB0");
  cd.print("F ");
      
  delay(2000); //Syncs the LCD print speed with the operation speed of void temperatureHumidity()
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
  lcd.noDisplay();
}
