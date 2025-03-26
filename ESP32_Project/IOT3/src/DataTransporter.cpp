#include <PubSubClient.h>
#include <wifi_configuration.h>

const char* mqtt_server = "your_mqtt_server";
const int   mqtt_port = 1883;

enum class Topics{
    Sensor1Triggered,
    Sensor2Triggered,
    PersonDetected,
    KeyCardDetected,
};


PubSubClient client;

void InitializeMQTT(){

    Serial.println("Ensuring WiFi connection...");
    EnsureWiFiConnection();

    Serial.println("Attempting to connect to MQTT broker");
    Serial.println("Server: " + String(mqtt_server));
    Serial.println("Port: " + String(mqtt_port));

    client.setClient(espClient);
    client.setServer(mqtt_server, mqtt_port);

}

void EnsureMQTTConnection(){
    if(!client.connected()){
        Serial.println("MQTT connection lost. Attempting to reconnect...");
        InitializeMQTT();
    }
}

bool PublishData(Topics topic, const char* payload){

    EnsureMQTTConnection();
    
    const char* topicString = GetSelectedTopic(topic);

    if (topicString == ""){
        Serial.println("Invalid topic");
        return false;
    }

    Serial.println("Publishing data to topic: " + String(topicString));
    Serial.println("Payload: " + String(payload));

    bool success = client.publish(topicString, payload);

    if(success){
        Serial.println("Data published successfully");
    }else{
        Serial.println("Failed to publish data");
    }
    return success;
}

const char* GetSelectedTopic(Topics topic){
    switch(topic){
        case Topics::Sensor1Triggered:
            return "Sensor1Triggered";
        case Topics::Sensor2Triggered:
            return "Sensor2Triggered";
        case Topics::PersonDetected:
            return "PersonDetected";
        case Topics::KeyCardDetected:
            return "KeyCardDetected";
        default:
            return nullptr;
    }
}