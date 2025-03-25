#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "your_ssid";
const char* password = "your_password";

const char* mqtt_server = "your_mqtt_server";



WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}


void SetupWifiConfiguration(){
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    Serial.println("Wifi Configuration Setup Done");
}



void setup_wifi(){
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);


    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }


    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
}