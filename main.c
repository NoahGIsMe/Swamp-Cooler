#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ezButton.h>
#include <LiquidCrystal.h>

#define blueLED 13
#define greenLED 12
#define redLED 11
#define yellowLED 10
#define waterPin 9
#define DHTPIN 8
#define motorPin 7
#define buttonPin 6
#define waterSignal A5

#define OFF 0
#define ON 1
#define DHTTYPE DHT11

const int rs = 5, en = 4, d4 = 3, d5 = 2, d6 = 1, d7 = 0;

float h;
float t;
float f;

int waterLevel = 0;
int loopState = OFF;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
DHT dht = DHT(DHTPIN, DHTTYPE);
ezButton button(buttonPin);

void setup() {
  Serial.begin(9600);
  button.setDebounceTime(50);
  
  lcd.begin(16, 2);
  dht.begin();
  
  pinMode(waterPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(buttonPin, OUTPUT);
  
  digitalWrite(waterPin, LOW);
  
}

void loop() {
  button.loop();

  if (button.isPressed()) {
    if (loopState == OFF)
      loopState = ON;
    else
      loopState = OFF;
  }

  if (loopState == ON) {
    temperatureHumidity();
  }
  if (loopState == OFF) {
    turnOff();
  }
}

void motorControl() {
  digitalWrite(motorPin, HIGH);
}

void waterSensor() {
  digitalWrite(waterPin, HIGH);
  delay(10);
  
  waterLevel = analogRead(waterSignal);
  digitalWrite(waterPin, LOW);

  Serial.print("Sensor value: ");
  Serial.println(waterLevel);
  delay(1000);
}

void temperatureHumidity() {
  delay(2000);
  
  h = dht.readHumidity();
  t = dht.readTemperature();
  f = dht.readTemperature(true);
  
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" % ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" \xC2\xB0");
  Serial.print("C | ");
  Serial.print(f);
  Serial.print(" \xC2\xB0");
  Serial.print("F ");
}

void turnOff() {
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
