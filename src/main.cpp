#include <cstdio>
#define FTB_PROFILER_IMPL
#include "db.hpp"
#include "ftb/arraylist.hpp"
#include "ftb/macros.hpp"
#include "ftb/profiler.hpp"
#include "ftb/print.hpp"
#include "net.h"
#include "time.hpp"
#include "utf-8.hpp"
#include "xml.hpp"
#include "displays/display.h"

int main() {
    db::init();
    defer { db::deinit(); };

    // NOTE(Felix): another way to search for stations (no eva_nr though)
    // https://www.img-bahn.de/bin/ajax-getstop.exe?S=Garching%20fors
    // or
    // https://reiseauskunft.bahn.de/bin/bhftafel.exe/

    // NOTE(Felix): Actually for mvg:
    //   (petershausen bhf ID = de:09174:6840)
    //   (petershausen p+r ID = de:09174:7167)
    // - For finding ID
    //   - https://www.mvg.de/api/fahrinfo/location/queryWeb?q={string}
    /*
     * {
     *    "locations": [
     *      {
     *        "type"         : "station",
     *         "latitude"    : 48.4126,
     *         "longitude"   : 11.46992,
     *         "id"          : "de:09174:6840",
     *         "divaId"      : 6840,
     *         "place"       : "Petershausen",
     *         "name"        : "Petershausen",
     *         "hasLiveData" : false,
     *         "hasZoomData" : false,
     *         "products"    : [ "BAHN", "SBAHN", "BUS" ],
     *         "aliases"     : "Bahnhof Bf. DAH MMPE",
     *         "tariffZones" : "4|5",
     *         "lines" : {
     *           "tram"       : [ ],
     *           "nachttram"  : [ ],
     *           "sbahn"      : [ ],
     *           "ubahn"      : [ ],
     *           "bus"        : [ ],
     *           "nachtbus"   : [ ],
     *           "otherlines" : [ ]
     *      }
     *    ]
     * }
     * */
    // NOTE(Felix): For departure (flavour 1)
    // - https://www.mvg.de/api/fahrinfo/departure/{ID}?footway=0
    /*
     * {
     *   "servingLines" : [
     *     {
     *       "destination" : "Erding",
     *       "sev"         : false,
     *       "network"     : "ddb",
     *       "product"     : "SBAHN",
     *       "lineNumber"  : "S2",
     *       "divaId"      : "92M02"
     *     }
     *   ],
     *   "departures" : [
     *     {
     *        "departureTime"       : 1650657120000,
     *        "product"             : "SBAHN",
     *        "label"               : "S2",
     *        "destination"         : "Markt Schwaben",
     *        "live"                : false,
     *        "delay"               : 0,
     *        "cancelled"           : false,
     *        "platform"            : "5",
     *        "sev"                 : false,
     *        "lineBackgroundColor" : "#9bc04c",
     *        "departureId"         : "f4f0d89b56de31aae8c117b483546cf4#1650657120000#de:09174:6840",
     *        "stopPositionNumber"  : 0,
     *        "infoMessages"        : [ "..." ],
     *        "bannerHash"          : "65c28a6eec770977418d0c5a13c1bab7"
     *      },
     *   ]
     * }
     * */
    // NOTE(Felix): For departure (flavour 2)
    // - https://www.mvg.de/api/fahrinfong/departure?stationGlobalId={ID}
    /*
     * [
     *    {
     *      "plannedDepartureTime" : 1650657120000,
     *      "transportType"        : "SBAHN",
     *      "label"                : "S2",
     *      "destination"          : "Markt Schwaben",
     *      "realtime"             : true,
     *      "delayInMinutes"       : 0,
     *      "cancelled"            : false,
     *      "platform"             : 5,
     *      "sev"                  : false,
     *      "network"              : "ddb",
     *      "trainType"            : "",
     *      "messages"             : [],
     *      "bannerHash"           : "65c28a6eec770977418d0c5a13c1bab7",
     *      "occupancy"            : "UNKNOWN"
     *    },
     * ]
     * */

    Time now = Time::now();
    db::Station station = db::find_station("Petershausen");
    println("Found %s", station.eva_nr);

    db::Timetable timetable = db::get_timetable(station.eva_nr.data, now, 5);

    defer {
        station.free();
        timetable.free();
    };

    station.print();
    timetable.print(now);

    {
        Simple_Timetable stt {};
        Array_List<Simple_Timetable_Entry> entries;
        entries.init();
        defer { entries.deinit(); };

        for (db::Timetable_Stop s : timetable.stops) {
            // NOTE(Felix): only show departures
            if (!s.departure_event)
                continue;

            db::Event& ev = s.departure_event;

            if (s.get_relevant_time().compare(now) >= 0) {
                Simple_Timetable_Entry entry {};
                entry.planned_time = (ev.planned_time - now).to_minutes();
                if (ev.changed_time) {
                    entry.time_delta = (ev.changed_time - ev.planned_time).to_minutes();
                }

                if (ev.changed_path) {
                    entry.destination = ev.changed_path[ev.changed_path.num_splits()-1].data;
                } else if (ev.planned_path) {
                    entry.destination = ev.planned_path[ev.planned_path.num_splits()-1].data;
                } else {
                    entry.destination = "???";
                }

                if (ev.changed_platform) {
                    entry.track = ev.changed_platform.data;
                } else if (ev.planned_platform) {
                    entry.track = ev.planned_platform.data;
                } else {
                    entry.track = "???";
                }

                char* line = (char*)malloc(12*sizeof(char));
                snprintf(line, 12, "%s %s",
                         s.trip_label.category.data,
                         (ev.line.data ? ev.line.data : s.trip_label.train_number.data));

                entry.line = line;
                entry.message = "";

                entries.append(entry);
            }
        }

        init_display();
        display_timetable({entries.data, entries.count, ""}, GERMAN, MEDIUM);
        // TODO(Felix): free the line strings
        deinit_display();
    }


    return 0;
}
