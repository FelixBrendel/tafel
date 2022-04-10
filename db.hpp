#pragma once
#include "ftb/arraylist.hpp"
#include "ftb/print.hpp"
#include "net.h"
#include "xml.hpp"

namespace db {
    struct Station {
        String name;
        String eva_nr;
        String ds100_nr;
        bool   is_db;

        void free();
        void print(u32 indentation = 0);
    };

    struct Trip_Label {
        String filter_tags;
        String trip_Type;
        String owner;
        String category;     // eg "ICE", "RE", "S"
        String train_number; // eg 16

        void free();
    };

    struct Time {
        u8 YY;
        u8 MM;
        u8 dd;
        u8 HH;
        u8 mm;
    };

    struct Message {
        String      id;
        String      type;
        Maybe<Time> valid_from;
        Maybe<Time> valid_to;
        int         code;
        String      text_internal;
        String      text_external;
        String      category;
        s32         category_external;
        Maybe<Time> time_stamp;
        String      priority;
        String      owner;
        String      external_link;
        s32         deleted;

        void free();
        void print(u32 indentation = 0);
    };

    struct Event {
        String      planned_path;
        String      changed_path;
        String      planned_platform;
        String      changed_platform;
        Maybe<Time> planned_time;
        Maybe<Time> changed_time;
        String      planned_status;
        String      changed_status;
        s32         hidden;
        Maybe<Time> cancellation_time;
        String      wings;
        String      transition;
        String      planned_distant_endpoint;
        String      changed_distant_endpoint;
        s32         distance_change;
        String      line;
        Array_List<Message> messages;

        void free();
        void print(u32 indentation = 0);
    };

    struct Timetable_Stop {
        String               id;
        String               eva_nr;

        Trip_Label           trip_label;
        Maybe<Event>         arrival_event;
        Maybe<Event>         depature_event;
        Array_List<Message>  messages;

        void free();
        void print(u32 indentation = 0);
    };

    struct Timetable {
        String                     station_name;
        String                     station_eva_nr;
        Array_List<Timetable_Stop> stops;
        Array_List<Message>        messages;

        void free();
        void print(u32 indentation = 0);
    };

    void init();
    void deinit();

    Station find_station(const char* name);

    Timetable get_timetable(const char* eva_nr, u8 year, u8 month, u8 day, u8 hour);
    Timetable get_full_changes(const char* eva_nr);
    Timetable get_recent_changes(const char* eva_nr);
    const char* message_code_to_display_string(int code);
    const char* message_type_to_display_string(String type);

}
