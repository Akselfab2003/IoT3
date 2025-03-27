#ifndef PEOPLECOUNTER_H
#define PEOPLECOUNTER_H

#include <ctime>

class PeopleCounter {
    public:
        PeopleCounter(int count, time_t timestamp);
        int getCount();
        time_t getTimestamp();
        void setCount(int count);
        void setTimestamp(time_t timestamp);
    private:
        int count;
        time_t timestamp;
};


#endif
