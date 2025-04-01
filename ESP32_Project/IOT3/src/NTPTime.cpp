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

String getFormattedDateTime() {
    time_t now = getEpochTime();
    struct tm *timeinfo = localtime(&now);
    char buffer[25];

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return String(buffer);
}

unsigned long getEpochTime() {
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();
}
