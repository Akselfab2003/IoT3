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
    //client.connect("ESP32Client",NULL,NULL,); // Persistent session
    //client.connect("ESP32Client", const char *user, const char *pass, const char *willTopic, uint8_t willQos, boolean willRetain, const char *willMessage, boolean cleanSession)
    
    client.connect("ESP32Client", NULL,NULL, NULL,0,false,NULL,false);

    unsigned long afterInit = millis();
    Serial.println("After MQTT client initialization: " + String(afterInit));
    Serial.println("Time taken for MQTT client initialization: " + String(afterInit - beforeInit) + " ms");

}

bool EnsureMQTTConnection() {
    if (!client.connected()) {
        Serial.println("MQTT connection lost. Skipping synchronous reconnect.");
        return false;
    }
    return true;
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

bool PublishData(Topics topic, const char* payload) {
    // Check MQTT connection without trying to reconnect synchronously
    bool mqttStatus = EnsureMQTTConnection();
    
    const char* topicString = GetSelectedTopic(topic);
    if (topicString == nullptr || String(topicString) == "") {
        Serial.println("Invalid topic");
        return false;
    }
    
    if (!mqttStatus) {
        Serial.println("MQTT connection not available. Caching payload.");
        saveToCache(topic, String(payload));
        mqtt_connected = false;
        return false;
    } 
    // If connection is back after a previous outage, publish cached data
    else if (mqttStatus && !mqtt_connected) {
        Serial.println("MQTT connection re-established. Publishing cached data.");
        publishAllCachedData();
        mqtt_connected = true;
    }
    
    Serial.println("Publishing data to topic: " + String(topicString));
    Serial.println("Payload: " + String(payload));
    
    bool success = client.publish(topicString, payload);
    if (success) {
        Serial.println("Data published successfully");
    }
    return success;
}

void ProcessMQTT() {
    // Process MQTT client loop to handle incoming/outgoing messages.
    client.loop();
    
    // Attempt reconnect every 5 seconds if not connected.
    static unsigned long lastReconnectAttempt = 0;
    if (!client.connected() && (millis() - lastReconnectAttempt > 5000)) {
        Serial.println("Attempting background MQTT reconnect...");
        if (client.connect("ESP32Client")) {
            Serial.println("Background MQTT reconnect successful.");
            mqtt_connected = true;
            publishAllCachedData();
        }
        lastReconnectAttempt = millis();
    }
}