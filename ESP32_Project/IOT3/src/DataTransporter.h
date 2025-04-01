#ifndef DataTransporter_h
#define DataTransporter_h

enum class Topics{
    SensorTriggered,
    PersonDetected,
    KeyCardDetected,
    RegisterSensor
};

void InitializeMQTT();

bool EnsureMQTTConnection();

bool PublishData(Topics topic, const char* payload);

void ProcessMQTT();

#endif