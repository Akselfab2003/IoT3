#ifndef NTP_TIME_H
#define NTP_TIME_H

#include <Arduino.h>

// Function to initialize NTP
void initializeNTP();

// Function to get the current time as a formatted string
String getFormattedTime();

// Function to get the current epoch time
unsigned long getEpochTime();

String getFormattedDateTime();

#endif