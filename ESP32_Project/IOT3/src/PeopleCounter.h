#ifndef PEOPLECOUNTER_H
#define PEOPLECOUNTER_H

#include <ctime>

struct PeopleCounter
{
    PeopleCounter(int people, time_t timestamp) : people(people), timestamp(timestamp) {}
    int id = 0;
    int people;
    std::time_t timestamp;
};

#endif
