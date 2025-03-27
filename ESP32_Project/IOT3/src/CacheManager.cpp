#include <ArduinoJson.h>
#include "FS.h"
#include "SPIFFS.h"

#define CACHE_FILE "/data.json"

void saveToCache(int peopleCount, String timestamp) {
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    
    File file = SPIFFS.open(CACHE_FILE, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    
    ArduinoJson::StaticJsonDocument<256> doc;
    doc["peopleCount"] = peopleCount;
    doc["timestamp"] = timestamp;
    
    String jsonString;
    serializeJson(doc, jsonString);
    file.println(jsonString);
    file.close();
    Serial.println("Data saved to cache.");
}

