#ifndef SENSORSLOG_H
#define SENSORSLOG_H
#include <ctime>

struct SensorsLog
{
    SensorsLog(int sensor_id, float value, std::time_t timestamp) : sensor_id(sensor_id), value(value), timestamp(timestamp) {}
    int id = 0;
    int sensor_id;
    float value;
    std::time_t timestamp;
};

#endif