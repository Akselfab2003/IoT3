#include <esp32-hal-gpio.h>
#include <HardwareSerial.h>
#include <PeopleCount.h>
#include <sensor.h>
#include <ArduinoJson.h>
#include <DataTransporter.h>
#include <MovementSensor.h>
#include <PeopleCounter.h>
#include <SensorsLog.h>

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
String createJsonBody(Move_Sensor sensor);
String createPeopleCountToJson(PeopleCounter peopleCounter);
String CreateSensorLogJson(SensorsLog sensorLog, String sensorName);
void SendUpdateForPeopleCount(int NewPeopleCountValue);
void SendUpdateForSensorLog(String sensorName, float value);
String GetCurrentTimeMQTT(time_t timestamp);

void setup1() {
  // Use INPUT_PULLUP if sensors pull the line LOW when activated
  pinMode(SENSOR_PIN_Lose, INPUT);
  pinMode(SENSOR_PIN_Board, INPUT);
  Serial.println("People counter system initialized.");

  Move_Sensor sensor1 = Move_Sensor("SENSOR_PIN_Lose", "Move","Digital", 0);
  Move_Sensor sensor2 = Move_Sensor("SENSOR_PIN_Board", "Move","Digital", 0);

  String jsonBody1 = createJsonBody(sensor1);
  String jsonBody2 = createJsonBody(sensor2);

  Serial.println(jsonBody1);
  Serial.println(jsonBody2);

  PublishData(Topics::RegisterSensor, jsonBody1.c_str());

  delay(1000);

  PublishData(Topics::RegisterSensor, jsonBody2.c_str());
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
        SendUpdateForSensorLog("SENSOR_PIN_Lose", stateLose);
        //Serial.println("Debug: Sensor Lose triggered first.");
      } else if (stateBoard == LOW) {
        currentState = SENSOR_B_TRIGGERED;
        triggerTime = currentTime;
        //Serial.println("Debug: Sensor Board triggered first.");
        SendUpdateForSensorLog("SENSOR_PIN_Board", stateBoard);
      }
      break;

    case SENSOR_A_TRIGGERED:

      
      // Waiting for the second sensor (Board) to confirm entry
      if (stateBoard == LOW && (currentTime - triggerTime <= timeWindow)) {
        peopleCount++;
        SendUpdateForPeopleCount(peopleCount);
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
          SendUpdateForPeopleCount(peopleCount);
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

void SendUpdateForPeopleCount(int NewPeopleCountValue) {
  time_t timestamp = time(NULL);

  PeopleCounter peopleCounter = PeopleCounter(NewPeopleCountValue, timestamp);
  String peopleCountJson = createPeopleCountToJson(peopleCounter);
  PublishData(Topics::PersonDetected, peopleCountJson.c_str());
}


void SendUpdateForSensorLog(String sensorName, float value) {
  time_t timestamp = time(NULL);
  SensorsLog sensorLog = SensorsLog(0, value, timestamp);
  String sensorLogJson = CreateSensorLogJson(sensorLog, sensorName);
  PublishData(Topics::SensorTriggered, sensorLogJson.c_str());
}

String createJsonBody(Move_Sensor sensor){
  ArduinoJson::StaticJsonDocument<200> doc;
  doc["id"] = sensor.id;
  doc["name"] = sensor.name;
  doc["type"] = sensor.type;
  doc["unit"] = sensor.unit;
  doc["value"] = sensor.value;
  String jsonString;
  serializeJson(doc, jsonString);
  Serial.println(jsonString);
  return jsonString;
}

String CreateSensorLogJson(SensorsLog sensorLog, String sensorName){
  ArduinoJson::StaticJsonDocument<200> doc;

  doc["SensorName"] = sensorName;

  JsonObject sensorLogObject = doc.createNestedObject("SensorLogObject");
  sensorLogObject["id"] = sensorLog.id;
  sensorLogObject["sensor_id"] = sensorLog.sensor_id;
  sensorLogObject["value"] = sensorLog.value;
  sensorLogObject["timestamp"] = GetCurrentTimeMQTT(sensorLog.timestamp);

  String jsonString;
  serializeJson(doc, jsonString);
  Serial.println(jsonString);
  return jsonString;
}

String createPeopleCountToJson(PeopleCounter peopleCounter){
  ArduinoJson::StaticJsonDocument<200> doc;
  doc["id"] = peopleCounter.id;
  doc["people"] = peopleCounter.people;
  doc["timestamp"] = GetCurrentTimeMQTT(peopleCounter.timestamp);
  String jsonString;
  serializeJson(doc, jsonString);
  Serial.println(jsonString);
  return jsonString;
}

String GetCurrentTimeMQTT(time_t timestamp){
  struct tm timeinfo;
  localtime_r(&timestamp, &timeinfo);
  char formattedTime[20];
  strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%d %H:%M:%S", &timeinfo);
  Serial.println(formattedTime);
  return String(formattedTime);
}