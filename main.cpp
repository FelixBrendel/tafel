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
    db::Timetable timetable = db::get_timetable(station.eva_nr.data, now, 3);

    defer {
        station.free();
        timetable.free();
    };

    station.print();
    timetable.print(now);


    return 0;
}
