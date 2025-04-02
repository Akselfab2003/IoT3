#include <ArduinoJson.h>
#include "FS.h"
#include "SPIFFS.h"
#include "DataTransporter.h"

// Define cache file paths for different topics
#define CACHE_FILE_REGISTERSENSOR   "/register_sensor.json"
#define CACHE_FILE_SENSORTRIGGERED  "/sensor_triggered.json"
#define CACHE_FILE_PERSONDETECTED   "/person_detected.json"

/**
 * Saves a payload to the cache file corresponding to the given topic.
 * If the SPIFFS filesystem is not mounted, it will attempt to mount it.
 * 
 * @param topic The topic for which the payload is being cached.
 * @param payload The data to be saved in the cache.
 */
void saveToCache(Topics topic, const String& payload) {
    if (!SPIFFS.begin(true)) { // Mount SPIFFS if not already mounted
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
    
    // Open the cache file in append mode
    File file = SPIFFS.open(fileName, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file " + fileName + " for writing");
        return;
    }
    
    file.println(payload); // Write the payload to the file
    file.close();
}

/**
 * Publishes all cached data for a specific topic.
 * Reads the cache file line by line and publishes each line.
 * Deletes the cache file after publishing all data.
 * 
 * @param topic The topic for which cached data should be published.
 */
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

    if (!SPIFFS.begin(true)) { // Mount SPIFFS if not already mounted
         Serial.println("SPIFFS Mount Failed");
         return;
    }
    
    if (!SPIFFS.exists(fileName)) { // Check if the cache file exists
         Serial.println("Cache file " + fileName + " does not exist. Nothing to publish.");
         return;
    }
    
    // Open the cache file in read mode
    File file = SPIFFS.open(fileName, FILE_READ);
    if (!file) {
         Serial.println("Failed to open cache file " + fileName + " for reading.");
         return;
    }
    
    // Read and publish each line from the cache file
    while (file.available()) {
         String line = file.readStringUntil('\n');
         if (line.length() == 0) continue; // Skip empty lines
         bool success = PublishData(topic, line.c_str()); // Publish the data
         delay(100); // Small delay to avoid overwhelming the system
         if (!success) {
            Serial.println("Failed to publish cached message: " + line);
         }
    }
    file.close();
    
    // Remove the cache file after publishing
    SPIFFS.remove(fileName);
}

/**
 * Publishes all cached data for all topics.
 * Calls `publishCachedDataForTopic` for each predefined topic.
 */
void publishAllCachedData() {
    publishCachedDataForTopic(Topics::RegisterSensor);
    publishCachedDataForTopic(Topics::SensorTriggered);
    publishCachedDataForTopic(Topics::PersonDetected);
}