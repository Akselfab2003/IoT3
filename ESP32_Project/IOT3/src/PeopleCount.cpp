#include <esp32-hal-gpio.h>
#include <HardwareSerial.h>
const int sensorA = 34;   // Outside sensor (assumed to trigger first when entering)
const int sensorB = 35;   // Inside sensor (assumed to trigger second when entering)

enum MovementState {
  WAITING,
  SENSOR_A_TRIGGERED,
  SENSOR_B_TRIGGERED
};

MovementState currentState = WAITING;
unsigned long triggerTime = 0;
const unsigned long timeWindow = 500; // Time window in milliseconds for a valid sequence

int peopleCount = 0;

void setup() {
  Serial.begin(115200);
  // Use INPUT_PULLUP if sensors pull the line LOW when activated
  pinMode(sensorA, INPUT_PULLUP);
  pinMode(sensorB, INPUT_PULLUP);
  Serial.println("People counter system initialized.");
}

void loop() {
  int stateA = digitalRead(sensorA);
  int stateB = digitalRead(sensorB);
  unsigned long currentTime = millis();

  // Reset the state if the time window has expired
  if (currentState != WAITING && (currentTime - triggerTime > timeWindow)) {
    currentState = WAITING;
  }

  if (currentState == WAITING) {
    if (stateA == LOW) {
      // Outside sensor triggered first
      currentState = SENSOR_A_TRIGGERED;
      triggerTime = currentTime;
      delay(50);  // Simple debounce delay
    }
    else if (stateB == LOW) {
      // Inside sensor triggered first
      currentState = SENSOR_B_TRIGGERED;
      triggerTime = currentTime;
      delay(50);
    }
  }
  else if (currentState == SENSOR_A_TRIGGERED) {
    // Waiting for the inside sensor to trigger to confirm an entry
    if (stateB == LOW) {
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
    if (stateA == LOW) {
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
