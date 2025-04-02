#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "NTPTime.h"

// NTP Client setup
WiFiUDP ntpUDP; // UDP instance for NTP communication
NTPClient timeClient(ntpUDP, "dk.pool.ntp.org", -18000, 60000); // UTC-5 (ETC Standard Time), updates every 60 seconds

/**
 * Initializes the NTP client.
 * Starts the NTP client and performs an initial time update.
 */
void initializeNTP() {
    Serial.println("Initializing NTP...");
    timeClient.begin(); // Start the NTP client
    timeClient.update(); // Perform an initial time update
    Serial.println("NTP initialized.");
}

/**
 * Gets the current time in HH:MM:SS format.
 * Updates the NTP client before retrieving the formatted time.
 * 
 * @return A string representing the current time in HH:MM:SS format.
 */
String getFormattedTime() {
    timeClient.update(); // Ensure the time is up-to-date
    return timeClient.getFormattedTime(); // Returns time in HH:MM:SS format
}

/**
 * Gets the current date and time in YYYY-MM-DD HH:MM:SS format.
 * Converts the epoch time to a human-readable format.
 * 
 * @return A string representing the current date and time.
 */
String getFormattedDateTime() {
    time_t now = getEpochTime(); // Get the current epoch time
    struct tm *timeinfo = localtime(&now); // Convert epoch time to local time
    char buffer[25];

    // Format the date and time as YYYY-MM-DD HH:MM:SS
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return String(buffer);
}

/**
 * Gets the current epoch time (seconds since 1970-01-01 00:00:00 UTC).
 * Updates the NTP client before retrieving the epoch time.
 * 
 * @return The current epoch time as an unsigned long.
 */
unsigned long getEpochTime() {
    timeClient.update(); // Ensure the time is up-to-date
    return timeClient.getEpochTime(); // Return the epoch time
}