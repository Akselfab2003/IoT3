#include <WiFi.h>
#include <wifi_configuration.h>

#define  WIFI_SSID  "your_ssid"
#define  WIFI_PASS "your_password" 

WiFiClient espClient;

void initialize_WiFi(){

    Serial.println("Attempting to connect to WiFi network");
    Serial.println("SSID: " + String(WIFI_SSID));
    WiFi.begin(WIFI_SSID,WIFI_PASS);

    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED){
        Serial.println("Connected to WiFi network");
        Serial.println("IP Address: " + WiFi.localIP().toString());
    }
}


void EnsureWiFiConnection(){
    if(WiFi.status() != WL_CONNECTED){
        Serial.println("WiFi connection lost. Attempting to reconnect...");
        initialize_WiFi();
    }
}