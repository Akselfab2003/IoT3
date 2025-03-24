#include <Arduino.h>


#define SENSOR_PIN 18

void ReadSensor();

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  pinMode(SENSOR_PIN, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  ReadSensor();
  delay(1000);

}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}



void ReadSensor(){
  int state = digitalRead(SENSOR_PIN);
  if (state == LOW)
  {
    Serial.println("Object Detected");
  }
  else
  {
    Serial.println("No Object Detected");
  }
}