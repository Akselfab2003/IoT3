#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <Arduino.h>
#include "DataTransporter.h"  // For the Topics enum
#include "FS.h"
#include "SPIFFS.h"

// Save the payload to the appropriate cache file based on the topic
void saveToCache(Topics topic, const String& payload);

// Publish all cached data for a given topic and then clear the cache file
void publishCachedDataForTopic(Topics topic);

// Publish cached data for all topics and clear their respective cache files
void publishAllCachedData();

#endif // CACHEMANAGER_H
