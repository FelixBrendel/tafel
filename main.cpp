#include <stdio.h>
#include <time.h>
#include "db.hpp"
#include "ftb/macros.hpp"
#include "ftb/print.hpp"
#include "net.h"
#include "utf-8.hpp"
#include "xml.hpp"

int main() {
    time_t t = time(NULL);
    tm tm = *localtime(&t);

    println("Now: %d.%02d.%d %02d:%02d:%02d\n",
            tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900,
            tm.tm_hour, tm.tm_min, tm.tm_sec);

    u8 year  = tm.tm_year+1900-2000;
    u8 month = tm.tm_mon+1;
    u8 day   = tm.tm_mday;
    u8 hour  = tm.tm_hour;

    db::init();
    defer {
        db::deinit();
    };

    // NOTE(Felix): another way to search for stations (no eva_nr though)
    // https://www.img-bahn.de/bin/ajax-getstop.exe?S=Garching%20fors
    db::Station station = db::find_station("Petershausen");
    db::Timetable timetable = db::get_timetable(station.eva_nr.data,
                                                year, month, day, hour);
    defer {
        station.free();
        timetable.free();
    };

    station.print();
    timetable.print();


    return 0;
}
