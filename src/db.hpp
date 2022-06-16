#pragma once
#include <time.h>
#include "ftb/arraylist.hpp"
#include "ftb/print.hpp"
#include "net.h"
#include "time.hpp"
#include "xml.hpp"

namespace db {

    struct Station {
        String name;
        String eva_nr;
        String ds100_nr;
        bool   is_db;

        void free();
        void print();
    };

    struct Trip_Label {
        String filter_tags;
        String trip_type;
        String owner;
        String category;     // eg "ICE", "RE", "S"
        String train_number; // eg 16

        void free();
        void update(Trip_Label* other);
    };

    struct Message {
        String      id;
        String      type;
        Maybe<Time> valid_from;
        Maybe<Time> valid_to;
        Maybe<int>  code;
        String      text_internal;
        String      text_external;
        String      category;
        Maybe<s32>  category_external;
        Maybe<Time> time_stamp;
        String      priority;
        String      owner;
        String      external_link;
        Maybe<s32>  deleted;

        void free();
        void print();
    };

    struct Message_List : Array_List<Message> {
        void update(Message_List* other);
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
        Message_List messages;

        void update(Event* other);
        void free();
        void print();
    };

    struct Timetable_Stop {
        String               id;
        String               eva_nr;

        Maybe<Trip_Label>    trip_label;
        Maybe<Event>         arrival_event;
        Maybe<Event>         departure_event;
        Message_List  messages;

        void free();
        void print();

        void update(Timetable_Stop* other);

        Time get_relevant_time() const;
    };

    enum struct Merge_Type {
        Merge_Stops,
        Update_Stops,
    };

    struct Timetable {
        String                     station_name;
        String                     station_eva_nr;
        Array_List<Timetable_Stop> stops;
        Message_List               messages;

        void update(Timetable* other, Merge_Type m_type);
        void free();
        void print(Time after = {});
    };

    void init();
    void deinit();

    Station find_station(const char* name);

    Timetable get_timetable(const char* eva_nr, Time time_start, u32 num_hours);
    Timetable get_full_changes(const char* eva_nr);
    Timetable get_recent_changes(const char* eva_nr);
    const char* message_code_to_display_string(int code);
    const char* message_type_to_display_string(String type);

}
