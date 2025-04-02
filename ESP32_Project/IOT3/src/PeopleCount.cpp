#include <esp32-hal-gpio.h>
#include <HardwareSerial.h>
#include <PeopleCount.h>
#include <sensor.h>
#include <ArduinoJson.h>
#include <DataTransporter.h>
#include <MovementSensor.h>
#include <PeopleCounter.h>
#include <SensorsLog.h>
#include <NTPTime.h>

// Define pins for the sensors
#define SENSOR_PIN_Lose 34    
#define SENSOR_PIN_Board 35

// Enum to represent the movement state of the system
enum MovementState {
  WAITING,
  SENSOR_A_TRIGGERED,
  SENSOR_B_TRIGGERED
};

// Global variables for state management and people counting
MovementState currentState = WAITING;
unsigned long triggerTime = 0;
const unsigned long timeWindow = 1000; // Time window in milliseconds for a valid sequence
int peopleCount = 0;

// Function declarations
String createJsonBody(Move_Sensor sensor);
String createPeopleCountToJson(PeopleCounter peopleCounter);
String CreateSensorLogJson(SensorsLog sensorLog, String sensorName);
void SendUpdateForPeopleCount(int NewPeopleCountValue);
void SendUpdateForSensorLog(String sensorName, float value);
String GetCurrentTimeMQTT(time_t timestamp);

/**
 * Initializes the people counting system.
 * Sets up sensor pins and registers the sensors via MQTT.
 */
void setup1() {
  pinMode(SENSOR_PIN_Lose, INPUT);
  pinMode(SENSOR_PIN_Board, INPUT);
  Serial.println("People counter system initialized.");

  // Register the sensors
  Move_Sensor sensor1 = Move_Sensor("SENSOR_PIN_Lose", "Move", "Digital", 0);
  Move_Sensor sensor2 = Move_Sensor("SENSOR_PIN_Board", "Move", "Digital", 0);

  String jsonBody1 = createJsonBody(sensor1);
  String jsonBody2 = createJsonBody(sensor2);

  Serial.println(jsonBody1);
  Serial.println(jsonBody2);

  PublishData(Topics::RegisterSensor, jsonBody1.c_str());
  delay(1000);
  PublishData(Topics::RegisterSensor, jsonBody2.c_str());
}

// Variables to track the last trigger times and states of the sensors
unsigned long StateLoseLastTrigger = millis();
unsigned long StateBoardLastTrigger = millis();
int lastStateLose = HIGH;
int lastStateBoard = HIGH;

/**
 * Checks if a person has entered or exited based on sensor triggers.
 * Updates the people count and publishes the data via MQTT.
 */
void CheckIfPersonEntered() {
  int stateLose = digitalRead(SENSOR_PIN_Lose);
  int stateBoard = digitalRead(SENSOR_PIN_Board);
  unsigned long currentTime = millis();

  // Log sensor state changes
  if (lastStateLose != stateLose) {
    lastStateLose = stateLose;
    SendUpdateForSensorLog("SENSOR_PIN_Lose", stateLose);
  }
  if (stateBoard != lastStateBoard) {
    lastStateBoard = stateBoard;
    SendUpdateForSensorLog("SENSOR_PIN_Board", stateBoard);
  }

  // State machine to handle sensor triggers
  switch (currentState) {
    case WAITING:
      // Waiting for the first sensor to trigger
      if (stateLose == LOW) {
        currentState = SENSOR_A_TRIGGERED;
        triggerTime = currentTime;
      } else if (stateBoard == LOW) {
        currentState = SENSOR_B_TRIGGERED;
        triggerTime = currentTime;
      }
      break;

    case SENSOR_A_TRIGGERED:
      // Waiting for the second sensor (Board) to confirm entry
      if (stateBoard == LOW && (currentTime - triggerTime <= timeWindow)) {
        peopleCount++;
        SendUpdateForPeopleCount(peopleCount);
        currentState = WAITING; // Reset state
      } else if (currentTime - triggerTime > timeWindow) {
        // Timeout: Reset state if second sensor is not triggered in time
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
        currentState = WAITING; // Reset state
      } else if (currentTime - triggerTime > timeWindow) {
        // Timeout: Reset state if first sensor is not triggered in time
        currentState = WAITING;
      }
      delay(100);
      break;

    default:
      // Fallback to reset state in case of unexpected behavior
      currentState = WAITING;
      break;
  }
}

/**
 * Sends an update for the current people count via MQTT.
 * 
 * @param NewPeopleCountValue The updated people count value.
 */
void SendUpdateForPeopleCount(int NewPeopleCountValue) {
  time_t timestamp = time(NULL);
  PeopleCounter peopleCounter = PeopleCounter(NewPeopleCountValue, timestamp);
  String peopleCountJson = createPeopleCountToJson(peopleCounter);
  PublishData(Topics::PersonDetected, peopleCountJson.c_str());
}

/**
 * Sends an update for a sensor log via MQTT.
 * 
 * @param sensorName The name of the sensor.
 * @param value The value of the sensor.
 */
void SendUpdateForSensorLog(String sensorName, float value) {
  time_t timestamp = time(NULL);
  SensorsLog sensorLog = SensorsLog(0, value, timestamp);
  String sensorLogJson = CreateSensorLogJson(sensorLog, sensorName);
  PublishData(Topics::SensorTriggered, sensorLogJson.c_str());
}

/**
 * Creates a JSON body for registering a sensor.
 * 
 * @param sensor The sensor object to serialize.
 * @return A JSON string representing the sensor.
 */
String createJsonBody(Move_Sensor sensor) {
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

/**
 * Creates a JSON string for a sensor log.
 * 
 * @param sensorLog The sensor log object to serialize.
 * @param sensorName The name of the sensor.
 * @return A JSON string representing the sensor log.
 */
String CreateSensorLogJson(SensorsLog sensorLog, String sensorName) {
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

/**
 * Creates a JSON string for the people count.
 * 
 * @param peopleCounter The people counter object to serialize.
 * @return A JSON string representing the people count.
 */
String createPeopleCountToJson(PeopleCounter peopleCounter) {
  ArduinoJson::StaticJsonDocument<200> doc;
  doc["id"] = peopleCounter.id;
  doc["people"] = peopleCounter.people;
  doc["timestamp"] = GetCurrentTimeMQTT(peopleCounter.timestamp);
  String jsonString;
  serializeJson(doc, jsonString);
  Serial.println(jsonString);
  return jsonString;
}

/**
 * Formats a timestamp into a human-readable string for MQTT.
 * 
 * @param timestamp The timestamp to format.
 * @return A formatted string representing the timestamp.
 */
String GetCurrentTimeMQTT(time_t timestamp) {
  struct tm timeinfo;
  localtime_r(&timestamp, &timeinfo);
  char formattedTime[20];
  strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%d %H:%M:%S", &timeinfo);
  Serial.println(formattedTime);

  return getFormattedDateTime();
}