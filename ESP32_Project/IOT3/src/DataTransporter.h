#ifndef DataTransporter_h
#define DataTransporter_h

enum class Topics{
    SensorTriggered,
    PersonDetected,
    KeyCardDetected,
    RegisterSensor
};

void InitializeMQTT();

void EnsureMQTTConnection();

bool PublishData(Topics topic, const char* payload);

#endif