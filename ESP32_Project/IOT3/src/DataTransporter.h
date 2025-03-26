#ifndef DataTransporter_h
#define DataTransporter_h

enum class Topics{
    Sensor1Triggered,
    Sensor2Triggered,
    PersonDetected,
    KeyCardDetected,
};

void InitializeMQTT();

void EnsureMQTTConnection();

bool PublishData(Topics topic, const char* payload);

#endif