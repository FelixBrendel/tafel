#pragma once
#include "ftb/types.hpp"

struct Time_Diff {
    s8  days;     // [1-31]
    s8  hours;    // [0-23]
    s8  minutes;  // [0-59]
    f32 seconds;  // [0-59.99…]

    f32 to_seconds();
    s32 to_minutes();
};

struct Time {
    s32 year;
    u8  month;   // [1-12]
    u8  day;     // [1-31]
    u8  hour;    // [0-23]
    u8  minute;  // [0-59]
    f32 seconds; // [0-59.99…]

    void print();
    Time_Diff operator-(Time other);
    Time operator+(Time_Diff delta);

    s32 compare(Time other); // returns -1, 0 or 1

    static Time now();
};
