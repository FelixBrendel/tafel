#pragma once
#include "ftb/types.hpp"

struct Time_Diff {
    u8  days;     // [1-31]
    u8  hours;    // [0-23]
    u8  minutes;  // [0-59]
    f32 seconds;  // [0-59.99…]
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

    static Time now();
};
