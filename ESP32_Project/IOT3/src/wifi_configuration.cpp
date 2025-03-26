#include <WiFi.h>
#include <PubSubClient.h>
#include <wifi_configuration.h>

const char* ssid = "your_ssid";
const char* password = "your_password";

const char* mqtt_server = "your_mqtt_server";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void initialize_WiFi(){

    Serial.println("Attempting to connect to WiFi network");
    Serial.println("SSID: " + String(ssid));
    WiFi.begin(ssid,password);

    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED){
        Serial.println("Connected to WiFi network");
        Serial.println("IP Address: " + WiFi.localIP().toString());
    }
}