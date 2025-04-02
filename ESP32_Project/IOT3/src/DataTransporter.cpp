#include <PubSubClient.h>
#include <wifi_configuration.h>
#include <DataTransporter.h>
#include "CacheManager.h"

// Remember to change the IP address to the IP address of your MQTT broker
// const char* mqtt_server = ""; // Define your MQTT broker address
const int mqtt_port = 1883; // Default MQTT port

// MQTT client instance
PubSubClient client;

// Tracks the MQTT connection status
bool mqtt_connected = true;

SemaphoreHandle_t mqttMutex = xSemaphoreCreateMutex();

/**
 * Initializes the MQTT client.
 * Ensures WiFi is connected, sets up the MQTT client, and attempts to connect to the MQTT broker.
 */
void InitializeMQTT() {
    Serial.println("Ensuring WiFi connection...");
    EnsureWiFiConnection(); // Ensure the ESP32 is connected to WiFi

    Serial.println("Attempting to connect to MQTT broker");
    Serial.println("Server: " + String(mqtt_server));
    Serial.println("Port: " + String(mqtt_port));

    unsigned long beforeInit = millis();
    Serial.println("Before MQTT client initialization: " + String(beforeInit));

    // Set up the MQTT client with the broker details
    client.setClient(espClient);
    client.setServer(mqtt_server, mqtt_port);
    
    // Connect to the MQTT broker
    client.connect("ESP32Client");

    unsigned long afterInit = millis();
    Serial.println("After MQTT client initialization: " + String(afterInit));
    Serial.println("Time taken for MQTT client initialization: " + String(afterInit - beforeInit) + " ms");
}

/**
 * Ensures the MQTT connection is active.
 * If the connection is lost, it skips reconnecting synchronously.
 * 
 * @return True if the MQTT connection is active, false otherwise.
 */
bool EnsureMQTTConnection() {
    if (!client.connected()) {
        Serial.println("MQTT connection lost. Skipping synchronous reconnect.");
        return false;
    }
    return true;
}

/**
 * Maps a `Topics` enum value to its corresponding MQTT topic string.
 * 
 * @param topic The topic enum value.
 * @return The corresponding MQTT topic string, or nullptr if the topic is invalid.
 */
const char* GetSelectedTopic(Topics topic) {
    switch (topic) {
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

/**
 * Publishes data to the specified MQTT topic.
 * If the MQTT connection is unavailable, the payload is cached for later publishing.
 * 
 * @param topic The topic to publish to.
 * @param payload The data to publish.
 * @return True if the data was published successfully, false otherwise.
 */
bool PublishData(Topics topic, const char* payload) {
    bool mqttStatus = EnsureMQTTConnection(); // Check if MQTT is connected
    
    const char* topicString = GetSelectedTopic(topic);
    if (topicString == nullptr || String(topicString) == "") {
        Serial.println("Invalid topic");
        return false;
    }
    
    if (!mqttStatus) {
        // Cache the payload if MQTT is not connected
        Serial.println("MQTT connection not available. Caching payload.");
        saveToCache(topic, String(payload));
        mqtt_connected = false;
        return false;
    } 
    else if (mqttStatus && !mqtt_connected) {
        // Publish cached data if MQTT connection is re-established
        Serial.println("MQTT connection re-established. Publishing cached data.");
        publishAllCachedData();
        mqtt_connected = true;
    }
    
    bool success = false;
    //if (xSemaphoreTake(mqttMutex, portMAX_DELAY)){
        Serial.println("Publishing data to topic: " + String(topicString));
        Serial.println("Payload: " + String(payload));
        
        success = client.publish(topicString, payload);
        if (success) {
            Serial.println("Data published successfully");
        }
        //xSemaphoreGive(mqttMutex);
    //}
    Serial.println("Publishing data to topic: " + String(topicString));
    Serial.println("Payload: " + String(payload));
    
    bool success = client.publish(topicString, payload); // Publish the data
    if (success) {
        Serial.println("Data published successfully");
    }
    return success;
}

void ProcessMQTT(void* param) {
    while(true){
        //if (xSemaphoreTake(mqttMutex, portMAX_DELAY)){
            if (client.connected())
            {
                // Process MQTT client loop to handle incoming/outgoing messages.
                client.loop();
            }
            else
            {
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
            //xSemaphoreGive(mqttMutex);
        //}
        vTaskDelay(pdMS_TO_TICKS(10));
    }
   
/**
 * Processes MQTT client tasks.
 * Ensures the MQTT client loop is running and attempts to reconnect in the background if disconnected.
 */
void ProcessMQTT() {
    client.loop(); // Process incoming MQTT messages and maintain the connection
    
    static unsigned long lastReconnectAttempt = 0;
    if (!client.connected() && (millis() - lastReconnectAttempt > 5000)) {
        // Attempt to reconnect to the MQTT broker every 5 seconds
        Serial.println("Attempting background MQTT reconnect...");
        if (client.connect("ESP32Client")) {
            Serial.println("Background MQTT reconnect successful.");
            mqtt_connected = true;
            publishAllCachedData(); // Publish any cached data after reconnecting
        }
        lastReconnectAttempt = millis();
    }
}