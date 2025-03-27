#ifndef SENSORSLOG_H
#define SENSORSLOG_H
#include <ctime>



class SensorsLog {
    public:
        int id;
        int sensor_id;
        float value;
        std::time_t timestamp;
    
        // Constructor
        SensorsLog(int sensor_id, float value, std::time_t timestamp)
            : id(0), sensor_id(sensor_id), value(value), timestamp(timestamp) {}
    };




#endif