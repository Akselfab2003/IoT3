#ifndef WiFi_Configuration_h
#define WiFi_Configuration_h
#include <WiFi.h>

extern WiFiClient espClient;

void initialize_WiFi();

void EnsureWiFiConnection();

#endif