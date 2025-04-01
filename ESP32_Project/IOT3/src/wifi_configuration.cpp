#include <WiFi.h>
#include <wifi_configuration.h>
#include <CacheManager.h>

//#define  WIFI_SSID  Wifi
//#define  WIFI_PASS "your_password" 

WiFiClient espClient;

void initialize_WiFi(){

    Serial.println("Attempting to connect to WiFi network");
    Serial.println("SSID: " + String(WIFI_SSID));
    WiFi.begin(WIFI_SSID,WIFI_PASS);
    WiFi.setAutoReconnect(true);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi...");
    }
    
    


    if (WiFi.status() == WL_CONNECTED){
        Serial.println("Connected to WiFi network");
        Serial.println("IP Address: " + WiFi.localIP().toString());

        publishAllCachedData();
    }
}


void EnsureWiFiConnection(){
    unsigned long start = millis();
    Serial.println("Starting WiFi connection check at: " + String(start));

    if(WiFi.status() != WL_CONNECTED){
        Serial.println("WiFi connection lost. Attempting to reconnect...");
        initialize_WiFi();
    }
    Serial.println("Total time for EnsureWiFiConnection: " + String(millis() - start) + " ms");

}