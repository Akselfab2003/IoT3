#include <esp32-hal-gpio.h>
#include <HardwareSerial.h>
#include <PeopleCount.h>

#define SENSOR_PIN_Lose 34    
#define SENSOR_PIN_Board 35

enum MovementState {
  WAITING,
  SENSOR_A_TRIGGERED,
  SENSOR_B_TRIGGERED
};

MovementState currentState = WAITING;
unsigned long triggerTime = 0;
const unsigned long timeWindow = 500; // Time window in milliseconds for a valid sequence

int peopleCount = 0;

void setup1() {
  // Use INPUT_PULLUP if sensors pull the line LOW when activated
  pinMode(SENSOR_PIN_Lose, INPUT);
  pinMode(SENSOR_PIN_Board, INPUT);
  Serial.println("People counter system initialized.");
}

void loop1() {
  Serial.println("Loopi loop");
  int stateLose = digitalRead(SENSOR_PIN_Lose);
  int stateBoard = digitalRead(SENSOR_PIN_Board);
  unsigned long currentTime = millis();

  Serial.println("State Lose =" + String(stateLose));
  Serial.println("State Board =" + String(stateBoard));
  Serial.println("Current Time =" + String(currentTime));

  // Reset the state if the time window has expired
  if (currentState != WAITING && (currentTime - triggerTime > timeWindow)) {
    currentState = WAITING;
  }

  
  if (currentState == WAITING) {
    if (stateLose == LOW) {
      // Outside sensor triggered first
      currentState = SENSOR_A_TRIGGERED;
      triggerTime = currentTime;
      delay(50);  // Simple debounce delay
    }
    else if (stateBoard == LOW) {
      // Inside sensor triggered first
      currentState = SENSOR_B_TRIGGERED;
      triggerTime = currentTime;
      delay(50);
    }
    Serial.println("Current State: " + String(currentState));
  }
  else if (currentState == SENSOR_A_TRIGGERED) {
    
    Serial.println("Current State SENSOR A TRIGGER: " + String(currentState));
    // Waiting for the inside sensor to trigger to confirm an entry
    if (stateBoard == LOW) {
      // Sequence: sensorA then sensorB → Person entering
      peopleCount++;
      Serial.print("Person entered. Count: ");
      Serial.println(peopleCount);
      currentState = WAITING;
      delay(50);
    }
  }
  else if (currentState == SENSOR_B_TRIGGERED) {
    // Waiting for the outside sensor to trigger to confirm an exit

    Serial.println("Current State SENSOR B TRIGGER: " + String(currentState));

    if (stateLose == LOW) {
      // Sequence: sensorB then sensorA → Person exiting
      peopleCount--;
      if (peopleCount < 0) peopleCount = 0;
      Serial.print("Person exited. Count: ");
      Serial.println(peopleCount);
      currentState = WAITING;
      delay(50);
    }
  }
  
  delay(10);
}


unsigned long StateLoseLastTrigger = millis();
unsigned long StateBoardLastTrigger = millis();


void CheckIfPersonEntered(){
  int stateLose = digitalRead(SENSOR_PIN_Lose); # First Sensor 
  int stateBoard = digitalRead(SENSOR_PIN_Board); # Second Sensor
 
  if (stateLose == LOW){
    StateLoseLastTrigger = millis();

    if (StateLoseLastTrigger - StateBoardLastTrigger < 500){
      Serial.println("Person Entered");
      peopleCount++;
    }

  }


  if (stateBoard == LOW){
    StateBoardLastTrigger = millis();

    if (StateBoardLastTrigger - StateLoseLastTrigger < 500){
      Serial.println("Person Exited");
      peopleCount--;
    }

  }
  
}