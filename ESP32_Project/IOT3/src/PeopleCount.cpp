#include <esp32-hal-gpio.h>
#include <HardwareSerial.h>
#include <PeopleCount.h>
#include <DataTransporter.h>

#define SENSOR_PIN_Lose 34    
#define SENSOR_PIN_Board 35

enum MovementState {
  WAITING,
  SENSOR_A_TRIGGERED,
  SENSOR_B_TRIGGERED
};

MovementState currentState = WAITING;
unsigned long triggerTime = 0;
const unsigned long   timeWindow = 500; // Time window in milliseconds for a valid sequence

int peopleCount = 0;

void setup1() {
  // Use INPUT_PULLUP if sensors pull the line LOW when activated
  pinMode(SENSOR_PIN_Lose, INPUT);
  pinMode(SENSOR_PIN_Board, INPUT);
  Serial.println("People counter system initialized.");
}



unsigned long StateLoseLastTrigger = millis();
unsigned long StateBoardLastTrigger = millis();

void CheckIfPersonEntered() {
  int stateLose = digitalRead(SENSOR_PIN_Lose);
  int stateBoard = digitalRead(SENSOR_PIN_Board);
  unsigned long currentTime = millis();

  // Debug: Log sensor states and timestamps
  //Serial.println("Debug: Checking sensors...");
  //Serial.println("State Lose: " + String(stateLose));
  //Serial.println("State Board: " + String(stateBoard));
  //Serial.println("Current Time: " + String(currentTime));
  //Serial.println("Current State: " + String(currentState));
  //Serial.println("Trigger Time: " + String(triggerTime));

  switch (currentState) {
    case WAITING:
      // Waiting for the first sensor to trigger
      if (stateLose == LOW) {
        currentState = SENSOR_A_TRIGGERED;
        triggerTime = currentTime;
        //Serial.println("Debug: Sensor Lose triggered first.");
      } else if (stateBoard == LOW) {
        currentState = SENSOR_B_TRIGGERED;
        triggerTime = currentTime;
        //Serial.println("Debug: Sensor Board triggered first.");
      }
      break;

    case SENSOR_A_TRIGGERED:

      PublishData(Topics::SensorTriggered, "Sensor A Triggered");
      
      // Waiting for the second sensor (Board) to confirm entry
      if (stateBoard == LOW && (currentTime - triggerTime <= timeWindow)) {
        peopleCount++;
        //Serial.println("Person Entered");
        //Serial.println("Debug: Updated peopleCount: " + String(peopleCount));
        currentState = WAITING; // Reset state
      } else if (currentTime - triggerTime > timeWindow) {
        // Timeout: Reset state if second sensor is not triggered in time
        //Serial.println("Debug: Timeout waiting for Sensor Board. Resetting state.");
        currentState = WAITING;
      }
      delay(100);
      break;

    case SENSOR_B_TRIGGERED:

      PublishData(Topics::SensorTriggered, "Sensor B Triggered");

      // Waiting for the first sensor (Lose) to confirm exit
      if (stateLose == LOW && (currentTime - triggerTime <= timeWindow)) {
        if (peopleCount > 0) {
          peopleCount--;
        }
        //Serial.println("Person Exited");
        //Serial.println("Debug: Updated peopleCount: " + String(peopleCount));
        currentState = WAITING; // Reset state
      } else if (currentTime - triggerTime > timeWindow) {
        // Timeout: Reset state if first sensor is not triggered in time
        //Serial.println("Debug: Timeout waiting for Sensor Lose. Resetting state.");
        currentState = WAITING;
      }
      delay(100);
      break;

    default:
      // Fallback to reset state in case of unexpected behavior
      //Serial.println("Debug: Unknown state. Resetting to WAITING.");
      currentState = WAITING;
      break;
  }
}