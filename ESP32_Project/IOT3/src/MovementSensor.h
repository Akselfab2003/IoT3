#ifndef MOVEMENTSENSOR_H
#define MOVEMENTSENSOR_H
#include <string>

struct Move_Sensor 
{
    Move_Sensor(std::string name, std::string type,std::string unit, float value) : name(name), type(type), unit(unit), value(value) {}

    std::string name;
    std::string type;
    std::string unit;
    float value;
};

#endif