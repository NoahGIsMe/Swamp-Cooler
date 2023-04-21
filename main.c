#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ezButton.h>

#define LOOP_STATE_STOPPED 0
#define LOOP_STATE_STARTED 1
#define DHTPIN 8
#define motorPin 3
#define waterPin 9
#define waterSignal A5
#define DHTTYPE DHT11

float h;
float t;
float f;

int waterLevel = 0;
int loopState = LOOP_STATE_STOPPED;

DHT dht = DHT(DHTPIN, DHTTYPE);
ezButton button(6);

void setup() {
  Serial.begin(9600);
  button.setDebounceTime(50);

  dht.begin();
  
  pinMode(waterPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  
}

void loop() {
  button.loop();

  if (button.isPressed()) {
    if (loopState == LOOP_STATE_STOPPED)
      loopState = LOOP_STATE_STARTED;
    else
      loopState = LOOP_STATE_STOPPED;
  }

  if (loopState == LOOP_STATE_STARTED) {
    temperatureHumidity();
  }
}

void motorControl() {
  digitalWrite(motorPin, HIGH);
}

void waterSensor() {
  digitalWrite(waterPin, HIGH);
  delay(10);
  
  waterLevel = analogRead(SIGNAL_PIN);
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
