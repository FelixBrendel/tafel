#include <bits/types/time_t.h>
#include <time.h>
#include "time.hpp"
#include "ftb/print.hpp"

time_t to_time_t(Time t) {
    tm this_tm {};

    this_tm.tm_year  = t.year - 1900;
    this_tm.tm_mon   = t.month - 1;
    this_tm.tm_mday  = t.day;
    this_tm.tm_hour  = t.hour;
    this_tm.tm_min   = t.minute;
    this_tm.tm_sec   = (s32)(t.seconds+0.5);
    this_tm.tm_isdst = -1;

    return mktime(&this_tm);
}

time_t to_time_t(Time_Diff delta) {
    tm this_tm {};
    this_tm.tm_mday  = delta.days+1;
    this_tm.tm_hour  = delta.hours;
    this_tm.tm_min   = delta.minutes;
    this_tm.tm_sec   = (s32)(delta.seconds+0.5);
    this_tm.tm_isdst = -1;

    return mktime(&this_tm);
}

Time to_Time(tm tm_type) {
    Time result;

    result.year    = tm_type.tm_year+1900;
    result.month   = tm_type.tm_mon+1;
    result.day     = tm_type.tm_mday;
    result.hour    = tm_type.tm_hour;
    result.minute  = tm_type.tm_min;
    result.seconds = tm_type.tm_sec;

    return result;
}

f32 to_seconds(Time_Diff diff) {
    return
        diff.seconds                +
        diff.minutes * 60           +
        diff.hours   * 60 * 60      +
        diff.days    * 24 * 60 * 60;
}


Time_Diff Time::operator-(Time other) {

    f32 delta_second = difftime(to_time_t(*this),
                                to_time_t(other));

    Time_Diff diff;
    s32 delta_minutes = ((s32)(delta_second))/60;
    diff.seconds = delta_second - (delta_minutes*60);
    diff.minutes = delta_minutes % 60;
    s32 delta_hours = delta_minutes / 60;
    diff.hours = delta_hours % 24;
    s32 delta_days = delta_hours / 60;
    diff.days = delta_days;

    return diff;
}

Time Time::operator+(Time_Diff delta) {
    s32 delta_seconds = (s32)(to_seconds(delta)+0.5);
    time_t this_time_t = to_time_t(*this);

    time_t result_time_t = this_time_t + delta_seconds;

    tm result_tm = *localtime(&result_time_t);

    return to_Time(result_tm);
}

Time Time::now() {
    time_t t = time(NULL);
    tm tm = *localtime(&t);

    return to_Time(tm);
}

void Time::print() {
    println("Time: %02d.%02d.%d %02d:%02d:%02.0f", day, month, year, hour, minute, seconds);
}
