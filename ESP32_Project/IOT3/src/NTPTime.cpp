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

    // Adjust for daylight saving time (DST)
    struct tm *timeinfo = gmtime((time_t *)&epochTime);
    if (isDST(timeinfo)) {
        epochTime += 3600; // Add 1 hour for DST
    }

    return epochTime; // Returns epoch time adjusted for CET/CEST
}

bool isDST(struct tm *timeinfo) {
    // DST starts last Sunday of March and ends last Sunday of October
    int month = timeinfo->tm_mon + 1; // tm_mon is 0-based
    int day = timeinfo->tm_mday;
    int weekday = timeinfo->tm_wday; // 0 = Sunday, 1 = Monday, ..., 6 = Saturday

    if (month < 3 || month > 10) {
        return false; // Not DST
    }
    if (month > 3 && month < 10) {
        return true; // DST
    }

    // Handle March and October
    int lastSunday = day - weekday; // Last Sunday of the month
    if (month == 3) {
        return lastSunday >= 25; // DST starts last Sunday of March
    }
    if (month == 10) {
        return lastSunday < 25; // DST ends last Sunday of October
    }

    return false;
}