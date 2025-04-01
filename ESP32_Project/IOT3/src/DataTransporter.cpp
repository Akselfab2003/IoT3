#include <PubSubClient.h>
#include <wifi_configuration.h>
#include <DataTransporter.h>
#include "CacheManager.h"

//remember to change IP address to the IP address of your MQTT broker
//const char* mqtt_server = "";
const int   mqtt_port = 1883;

PubSubClient client;

void InitializeMQTT(){

    Serial.println("Ensuring WiFi connection...");
    EnsureWiFiConnection();

    Serial.println("Attempting to connect to MQTT broker");
    Serial.println("Server: " + String(mqtt_server));
    Serial.println("Port: " + String(mqtt_port));

    client.setClient(espClient);
    client.setServer(mqtt_server, mqtt_port);
    client.connect("ESP32Client");

}

bool EnsureMQTTConnection(){
    if(!client.connected()){
        Serial.println("MQTT connection lost. Attempting to reconnect...");
        InitializeMQTT();
    }

    if (&client != nullptr && client.connected()){
        return true;
    }
    Serial.println("Failed to connect to MQTT broker.");
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
        return false;
    }
    else{


        Serial.println("Publishing data to topic: " + String(topicString));
        Serial.println("Payload: " + String(payload));

        bool success = client.publish(topicString, payload);

        if (success) {
            Serial.println("Data published successfully");
        } else {
            Serial.println("Failed to publish data. Caching payload.");
            saveToCache(topic, String(payload));
        }
        return success;
    }
}