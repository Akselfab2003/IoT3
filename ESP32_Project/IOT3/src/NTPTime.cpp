#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "NTPTime.h"

// NTP Client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // UTC time, update every 60 seconds

void initializeNTP() {
    Serial.println("Initializing NTP...");
    timeClient.begin();
    timeClient.update();
    Serial.println("NTP initialized.");
}

String getFormattedTime() {
    timeClient.update();
    return timeClient.getFormattedTime(); // Returns time in HH:MM:SS format
}

unsigned long getEpochTime() {
    timeClient.update();
    return timeClient.getEpochTime(); // Returns epoch time (seconds since Jan 1, 1970)
}