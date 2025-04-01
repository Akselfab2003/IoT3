#include <ArduinoJson.h>
#include "FS.h"
#include "SPIFFS.h"
#include "DataTransporter.h"

#define CACHE_FILE_REGISTERSENSOR   "/register_sensor.json"
#define CACHE_FILE_SENSORTRIGGERED  "/sensor_triggered.json"
#define CACHE_FILE_PERSONDETECTED   "/person_detected.json"

void saveToCache(Topics topic, const String& payload) {
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    String fileName;
    switch (topic) {
        case Topics::RegisterSensor:
            fileName = CACHE_FILE_REGISTERSENSOR;
            break;
        case Topics::SensorTriggered:
            fileName = CACHE_FILE_SENSORTRIGGERED;
            break;
        case Topics::PersonDetected:
            fileName = CACHE_FILE_PERSONDETECTED;
            break;
        default:
            Serial.println("Invalid topic for caching.");
            return;
    }
    
    File file = SPIFFS.open(fileName, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file " + fileName + " for writing");
        return;
    }
    
    file.println(payload);
    file.close();
    
    Serial.println("Data saved to cache file " + fileName);
}

void publishCachedDataForTopic(Topics topic) {
    String fileName;
    switch (topic) {
        case Topics::RegisterSensor:
            fileName = CACHE_FILE_REGISTERSENSOR;
            break;
        case Topics::SensorTriggered:
            fileName = CACHE_FILE_SENSORTRIGGERED;
            break;
        case Topics::PersonDetected:
            fileName = CACHE_FILE_PERSONDETECTED;
            break;
        default:
            Serial.println("Invalid topic for cached publish.");
            return;
    }

    if (!SPIFFS.begin(true)) {
         Serial.println("SPIFFS Mount Failed");
         return;
    }
    
    if (!SPIFFS.exists(fileName)) {
         Serial.println("Cache file " + fileName + " does not exist. Nothing to publish.");
         return;
    }
    
    File file = SPIFFS.open(fileName, FILE_READ);
    if (!file) {
         Serial.println("Failed to open cache file " + fileName + " for reading.");
         return;
    }
    
    Serial.println("Publishing cached data from " + fileName);
    
    while (file.available()) {
         String line = file.readStringUntil('\n');
         if (line.length() == 0) continue;
         bool success = PublishData(topic, line.c_str());
         delay(100);
         if (!success) {
            Serial.println("Failed to publish cached message: " + line);
         }
    }
    file.close();
    
    SPIFFS.remove(fileName);
    Serial.println("Cleared cache file " + fileName);
}

void publishAllCachedData() {
    publishCachedDataForTopic(Topics::RegisterSensor);
    publishCachedDataForTopic(Topics::SensorTriggered);
    publishCachedDataForTopic(Topics::PersonDetected);
}