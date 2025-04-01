#include <PubSubClient.h>
#include <wifi_configuration.h>
#include <DataTransporter.h>
#include "CacheManager.h"
//remember to change IP address to the IP address of your MQTT broker
//const char* mqtt_server = "";
const int   mqtt_port = 1883;

PubSubClient client;

bool mqtt_connected = true;

void InitializeMQTT(){

    Serial.println("Ensuring WiFi connection...");
    EnsureWiFiConnection();

    Serial.println("Attempting to connect to MQTT broker");
    Serial.println("Server: " + String(mqtt_server));
    Serial.println("Port: " + String(mqtt_port));

    unsigned long beforeInit = millis();
    Serial.println("Before MQTT client initialization: " + String(beforeInit));

    client.setClient(espClient);
    client.setServer(mqtt_server, mqtt_port);
   
    unsigned long startAttemptTime = millis();
    while (!client.connected() && (millis() - startAttemptTime < 500)) { // 1-second timeout
        client.connect("ESP32Client");
    }

    unsigned long afterInit = millis();
    Serial.println("After MQTT client initialization: " + String(afterInit));
    Serial.println("Time taken for MQTT client initialization: " + String(afterInit - beforeInit) + " ms");

}

bool EnsureMQTTConnection() {
    unsigned long start = millis();
    Serial.println("Starting MQTT connection check at: " + String(start));

    if (!client.connected()) {
        unsigned long beforeInit = millis();
        Serial.println("MQTT connection lost. Before InitializeMQTT: " + String(beforeInit));
        
        InitializeMQTT();
        
        unsigned long afterInit = millis();
        Serial.println("MQTT connection lost. After InitializeMQTT: " + String(afterInit));
        Serial.println("Time taken for InitializeMQTT: " + String(afterInit - beforeInit) + " ms");
    }

    if (&client != nullptr && client.connected()) {
        unsigned long end = millis();
        Serial.println("MQTT connection check successful at: " + String(end));
        Serial.println("Total time for EnsureMQTTConnection: " + String(end - start) + " ms");
        return true;
    }

    unsigned long end = millis();
    Serial.println("MQTT connection check failed at: " + String(end));
    Serial.println("Total time for EnsureMQTTConnection: " + String(end - start) + " ms");
    return false;
}

const char* GetSelectedTopic(Topics topic){
    switch(topic){
        case Topics::SensorTriggered:
            return "SensorTriggered";
        case Topics::PersonDetected:
            return "PersonDetected";
        case Topics::KeyCardDetected:
            return "KeyCardDetected";
        case Topics::RegisterSensor:
            return "RegisterSensor";
        default:
            return nullptr;
    }
}

bool PublishData(Topics topic, const char* payload){

    bool MQTT_Connection_status = EnsureMQTTConnection();
    

    const char* topicString = GetSelectedTopic(topic);

    if (topicString == ""){
        Serial.println("Invalid topic");
        return false;
    }

    if (!MQTT_Connection_status){
        Serial.println("MQTT connection failed. Caching payload.");
        saveToCache(topic, String(payload));
        mqtt_connected = false;
        return false;
    }
    else if (MQTT_Connection_status && !mqtt_connected )
    {
        Serial.println("MQTT connection established. Publishing cached data.");
        publishAllCachedData();
        mqtt_connected = true;
    }
    if(MQTT_Connection_status)
    {

        Serial.println("Publishing data to topic: " + String(topicString));
        Serial.println("Payload: " + String(payload));

        bool success = client.publish(topicString, payload);

        if (success) {
            Serial.println("Data published successfully");
        }

        return success;
    }
}