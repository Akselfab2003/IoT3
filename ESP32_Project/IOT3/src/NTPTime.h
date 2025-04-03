#ifndef NTP_TIME_H
#define NTP_TIME_H

#include <Arduino.h>

// Function to initialize NTP
void initializeNTP();


// Function to get the current epoch time
unsigned long getEpochTime();

// Function to get the formatted date and time as a string
// Returns the date and time in the format YYYY-MM-DD HH:MM:SS
String getFormattedDateTime();

#endif