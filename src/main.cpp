#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <errno.h>
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


int sleep_in_sec(u64 sec) {
    // source: https://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds
    int res;
    timespec ts { .tv_sec = (long)sec };

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

void maybe_update_software() {
    profile_function;
    log_trace();

    log_info("running git fetch");
    if (system("git fetch")) {
        println("git fetch returned non zero");
        return;
    }

    char local_hash[40];
    char upstream_hash[40];

    {
        log_info("getting local commit hash");
        FILE* get_local = popen("git rev-parse HEAD", "r");
        if (fread(local_hash, sizeof(local_hash), 1, get_local) != 1) {
            log_error("local commit could not be read");
            return;
        }

        if (pclose(get_local) != 0) {
            log_error("reading local commit hash caused an error.");
            return;
        }
    }

    {
        log_info("getting upsream commit hash");
        FILE* get_upstream = popen("git rev-parse \"@{u}\"", "r");
        if (fread(upstream_hash, sizeof(upstream_hash), 1, get_upstream) != 1) {
            log_error("local commit could not be read");
            return;
        }

        if (pclose(get_upstream) != 0) {
            log_error("reading local commit hash caused an error.");
            return;
        }
    }

    log_info("local_hash:    %.*s", sizeof(local_hash), local_hash);
    log_info("upstream_hash: %.*s", sizeof(upstream_hash), upstream_hash);

    if (strncmp(local_hash, upstream_hash, sizeof(local_hash)) == 0) {
        log_info("-> hashes are equal, no action required");
    } else {
        log_info("-> hashes are not equal, updating...");

        log_info("resetting");
        if (system("git reset --hard")) {
            log_error("git reset --hard returned non zero");
            return;
        }

        log_info("pulling");

        // if (system("git pull")) {
        //     log_error("git pull returned non zero");
        //     return;
        // }
        {
            char git_reset_command[sizeof("git reset --hard a09cd199a7b414463772c093cca936d885a020a3")];
            memset(git_reset_command, 0, sizeof(git_reset_command));
            snprintf(git_reset_command, sizeof(git_reset_command), "git reset --hard %s", upstream_hash);
            log_info("running: %s", git_reset_command);
            if (system(git_reset_command)) {
                log_error("git reset returned non zero");
                return;
            }
        }


        char commit_msg_str[120];
        memset(commit_msg_str, 0, sizeof(commit_msg_str));

        {
            log_info("Reading commit msg");
            FILE* commit_msg = popen("git show -s --format=%s", "r");
            fread(commit_msg_str, 1, sizeof(commit_msg_str), commit_msg);
            pclose(commit_msg);

            char* c = commit_msg_str;
            while (*c) {
                if (*c == '\n' || *c == '\r' || *c == '\t')
                    *c = ' ';
                ++c;
            }

            log_info("-> %s", commit_msg_str);
        }


#if ON_RASPBERRY
        char display_string[200];
        memset(display_string, 0, sizeof(display_string));
        snprintf(display_string, sizeof(display_string), "Update: %s", commit_msg_str);
        log_info("display: '%s' with len: %llu",display_string, strlen(display_string));
        display_message(display_string);
#endif

        {
            log_info("compiling:");
            FILE* compiling = popen("./build.sh", "r");
            if (!compiling) {
                log_error("build script not found");
                return;
            }

            int i = 0;
            int ch;
            while((ch=fgetc(compiling)) != EOF) {
                putchar((char)ch);
                ++i;

                if ((i & 32) != 0)
                    fflush(stdout);
            }

            if (pclose(compiling) != 0) {
                log_error("compilation process returned non zero");
                return;
            }
        }

        log_info("deinit display");
        deinit_display();

        log_info("restarting");
        const char * argv[] = {"./tafel", nullptr};
        if (execv("./tafel", (char * const *)argv)) {
            log_error("error restarting");
            return;
        }
    }
}

bool read_env(const char* env_var_name, char* dest, int dest_size) {
    char* env_var_val = getenv(env_var_name);
    if (!env_var_val)
        return false;

    strncpy(dest, env_var_val, dest_size-1);
    dest[dest_size-1] = '\0';

    return true;
}

int main() {
    init_display();
    defer { deinit_display(); };

    char db_client_id[33];
    char db_client_secret[33];
    memset(db_client_id,     0, sizeof(db_client_id));
    memset(db_client_secret, 0, sizeof(db_client_secret));

    if(read_env("DB_CLENT_ID", db_client_id, sizeof(db_client_id))) {
        log_info("Got DB_CLENT_ID: %s.", db_client_id);
    } else {
        log_error("Environment variable DB_CLENT_ID is not set");
        return 1;
    }

    if (read_env("DB_CLENT_SECRET", db_client_secret, sizeof(db_client_secret))){
        log_info("Got DB_CLENT_SECRET: %s.", db_client_secret);
    } else {
        log_error("Environment variable DB_CLENT_SECRET is not set");
        return 1;
    }


    db::init(db_client_id, db_client_secret);
    defer { db::deinit(); };

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

        display_timetable({entries.data, entries.count, ""}, GERMAN, MEDIUM);
        // NOTE(Felix): free the line strings
        for (auto& entry : entries) {
            free((char*)entry.line);
        }

        log_info("entering update loop");
#if ON_RASPBERRY
        while(1) {
            maybe_update_software();

            // sleep_in_sec(30);
            int d;
            scanf("%d", &d);
        }
#endif
    }

    return 0;
}


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
