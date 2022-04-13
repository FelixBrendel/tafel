#include "db.hpp"
#include "ftb/macros.hpp"
#include "ftb/print.hpp"
#include "net.h"
#include "time.hpp"
#include "utf-8.hpp"
#include "xml.hpp"

int main() {
    db::init();
    defer {
        db::deinit();
    };

    // NOTE(Felix): another way to search for stations (no eva_nr though)
    // https://www.img-bahn.de/bin/ajax-getstop.exe?S=Garching%20fors

    db::Station station = db::find_station("Petershausen");
    db::Timetable timetable = db::get_timetable(station.eva_nr.data,
                                                Time::now()+Time_Diff{.hours = 1});
    defer {
        station.free();
        timetable.free();
    };

    station.print();
    timetable.print();


    return 0;
}
