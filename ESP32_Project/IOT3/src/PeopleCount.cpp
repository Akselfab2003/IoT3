#include <esp32-hal-gpio.h>
#include <HardwareSerial.h>
#include <PeopleCount.h>
#include <sensor.h>
#include <ArduinoJson.h>
#include <DataTransporter.h>
#include <MovementSensor.h>

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

const char* createJsonBody(Move_Sensor sensor){
  ArduinoJson::StaticJsonDocument<200> doc;
  doc["name"] = sensor.name;
  doc["type"] = sensor.type;
  doc["unit"] = sensor.unit;
  doc["value"] = sensor.value;
  return doc.as<String>().c_str();
}


void setup1() {
  // Use INPUT_PULLUP if sensors pull the line LOW when activated
  pinMode(SENSOR_PIN_Lose, INPUT);
  pinMode(SENSOR_PIN_Board, INPUT);
  Serial.println("People counter system initialized.");

  Move_Sensor sensor1 = Move_Sensor("SENSOR_PIN_Lose", "Move","Digital", 0);
  Move_Sensor sensor2 = Move_Sensor("SENSOR_PIN_Board", "Move","Digital", 0);

  const char* jsonBody1 = createJsonBody(sensor1);
  const char* jsonBody2 = createJsonBody(sensor2);

  Serial.println(jsonBody1);
  Serial.println(jsonBody2);

  PublishData(Topics::, jsonBody1);

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