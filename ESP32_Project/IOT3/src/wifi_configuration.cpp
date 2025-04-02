#include <WiFi.h>
#include <wifi_configuration.h>
#include <CacheManager.h>

WiFiClient espClient;
void WifiEventHandler(WiFiEvent_t event);

void initialize_WiFi(){

    Serial.println("Attempting to connect to WiFi network");
    Serial.println("SSID: " + String(WIFI_SSID));
    WiFi.onEvent(WifiEventHandler);
    WiFi.begin(WIFI_SSID,WIFI_PASS);
}

void WifiEventHandler(WiFiEvent_t event){

    switch(event){
        case SYSTEM_EVENT_STA_START:
            Serial.println("WiFi started");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.println("WiFi connected, IP address: " + WiFi.localIP().toString());
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("WiFi disconnected. Attempting to reconnect...");
            initialize_WiFi();
            break;
        default:
            break;
    }
}

void EnsureWiFiConnection(){
    if(WiFi.status() != WL_CONNECTED){
        Serial.println("WiFi connection lost. Attempting to reconnect...");
        initialize_WiFi();
    }
}