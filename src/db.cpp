#include "db.hpp"
#include "ftb/arraylist.hpp"
#include "ftb/macros.hpp"
#include "ftb/print.hpp"
#include "ftb/profiler.hpp"
#include "ftb/types.hpp"
#include "net.h"
#include "time.hpp"
#include "xml.hpp"
#include <cstdio>
#include <cstring>

namespace db {
    const char* authorization_token = "Bearer 77782dd921d7957515e62515f683374d";

    enum struct Custom_XML_Data_Types : u8 {
        Maybe_Time,
        Path_List
    };

    u32 numerical_value(char c) {
        // NOTE(Felix): assumes c is a number char
        return c - '0';
    }

    u32 read_time(char* pos, void* vp_time) {
        Maybe<Time>* time = (Maybe<Time>*)vp_time;
        time->__exists = true;

        u32 length = 10;

        if (xml::is_quotes_char(*pos)) {
            length += 2;
            pos++;
        }

        time->year   = numerical_value(pos[0]) * 10 + numerical_value(pos[1]) + 2000;
        time->month  = numerical_value(pos[2]) * 10 + numerical_value(pos[3]);
        time->day    = numerical_value(pos[4]) * 10 + numerical_value(pos[5]);
        time->hour   = numerical_value(pos[6]) * 10 + numerical_value(pos[7]);
        time->minute = numerical_value(pos[8]) * 10 + numerical_value(pos[9]);

        return length;
    }

    u32 read_path_list(char* pos, void* vp_ss) {
        String_Split* ss = (String_Split*)vp_ss;

        String s;
        u32 length = xml::read_string(pos, &s);
        s = html_decode_string(s, true);

        ss->init(s, '|');

        return length;
    }

    void init() {
        if (!net_init()) {
            printf("net init error.");
        }

        xml::init();
        xml::register_custom_reader_function((xml::Data_Type)Custom_XML_Data_Types::Maybe_Time, read_time);
        xml::register_custom_reader_function((xml::Data_Type)Custom_XML_Data_Types::Path_List, read_path_list);
    }

    void deinit() {
        xml::deinit();
        net_deinit();
    }

    void print_string_lines(char* s, int max_lines) {
        int cursor = 0;
        int lines = 0;
        if (s) {
            while (s[cursor]) {
                if (s[cursor] == '\n') {
                    ++lines;
                    if (lines == max_lines)
                        break;
                }

                ++cursor;
            }
            println("%.*s", cursor, s);

            if (s[cursor]) {
                println("...");
            }
            println("");
        }
    }

    void print_response(Response r, int string_lines = 10) {
        println("time:          %f", r.elapsed_time);
        println("response_code: %ld", r.response_code);
        println("");
        println("header:");
        println("-------");
        print_string_lines(r.header.data, string_lines);
        println("content:");
        println("--------");
        print_string_lines(r.response.data, string_lines);
    }

    u32 trip_label_parser (char* xml, void* vp_stop) {
        Maybe<Trip_Label>* trip_label = (Maybe<Trip_Label>*)vp_stop;
        trip_label->__exists = true;

        create_pipe("f", xml::Data_Type::String, &trip_label->filter_tags);
        create_pipe("t", xml::Data_Type::String, &trip_label->trip_type);
        create_pipe("o", xml::Data_Type::String, &trip_label->owner);
        create_pipe("c", xml::Data_Type::String, &trip_label->category);
        create_pipe("n", xml::Data_Type::String, &trip_label->train_number);

        return xml::parse_attributes(xml);
    }

    u32 parse_message(char* xml, void* vp_message_list) {
        Array_List<Message>* message_list = (Array_List<Message>*)vp_message_list;
        if (!message_list->data) {
            message_list->init();
        }
        message_list->append({});

        Message* message = &message_list->last_element();

        create_pipe("id",   xml::Data_Type::String,  &message->id);
        create_pipe("t",    xml::Data_Type::String,  &message->type);
        create_pipe("int",  xml::Data_Type::String,  &message->text_internal);
        create_pipe("ext",  xml::Data_Type::String,  &message->text_external);
        create_pipe("cat",  xml::Data_Type::String,  &message->category);
        create_pipe("pr",   xml::Data_Type::String,  &message->priority);
        create_pipe("o",    xml::Data_Type::String,  &message->owner);
        create_pipe("elnk", xml::Data_Type::String,  &message->external_link);

        create_pipe("c",    xml::Data_Type::Maybe_Integer, &message->code);
        create_pipe("ec",   xml::Data_Type::Maybe_Integer, &message->category_external);
        create_pipe("del",  xml::Data_Type::Maybe_Integer, &message->deleted);

        create_pipe("from", (xml::Data_Type)Custom_XML_Data_Types::Maybe_Time, &message->valid_from);
        create_pipe("to",   (xml::Data_Type)Custom_XML_Data_Types::Maybe_Time, &message->valid_to);
        create_pipe("ts",   (xml::Data_Type)Custom_XML_Data_Types::Maybe_Time, &message->time_stamp);

        return xml::parse_attributes(xml);
    }

    u32 parse_event_start(char* xml, void* vp_event) {
        Maybe<Event>* event = (Maybe<Event>*)vp_event;
        event->__exists = true;
        event->messages.init();

        create_pipe("ppth", (xml::Data_Type)Custom_XML_Data_Types::Path_List, &event->planned_path);
        create_pipe("cpth", (xml::Data_Type)Custom_XML_Data_Types::Path_List, &event->changed_path);
        create_pipe("pp",   xml::Data_Type::String,  &event->planned_platform);
        create_pipe("cp",   xml::Data_Type::String,  &event->changed_platform);
        create_pipe("pt",   (xml::Data_Type)Custom_XML_Data_Types::Maybe_Time, &event->planned_time);
        create_pipe("ct",   (xml::Data_Type)Custom_XML_Data_Types::Maybe_Time, &event->changed_time);
        create_pipe("ps",   xml::Data_Type::String,  &event->planned_status);
        create_pipe("cs",   xml::Data_Type::String,  &event->changed_status);
        create_pipe("hi",   xml::Data_Type::Integer, &event->hidden);
        create_pipe("clt",  (xml::Data_Type)Custom_XML_Data_Types::Maybe_Time, &event->cancellation_time);
        create_pipe("wings",xml::Data_Type::String,  &event->wings);
        create_pipe("tra",  xml::Data_Type::String,  &event->transition);
        create_pipe("pde",  xml::Data_Type::String,  &event->planned_distant_endpoint);
        create_pipe("cdr",  xml::Data_Type::String,  &event->changed_distant_endpoint);
        create_pipe("dc",   xml::Data_Type::String,  &event->distance_change);
        create_pipe("l",    xml::Data_Type::String,  &event->line);

        xml::push_handler_to_parser_stack({
            .key          = "m", // messages
            .open_handler = parse_message,
            .user_data    = &(event->messages)
        });

        return xml::parse_attributes(xml);
    }

    u32 parse_event_end(char* xml, void* vp_event) {
        xml::pop_handler_from_parser_stack();
        return xml::eat_until_end_of_tag(xml);
    }


    u32 timetable_parser_open(char* xml, void* vp_timetable) {
        /*
         * <timetable station='Petershausen(Obb)'>
         *   <s id="5555209269320973371-2203311032-1">
         *     <tl f="S" t="p" o="800725" c="S" n="6251"/>
         *     <dp pt="2203311032" pp="6" l="2" ppth="Vierkirchen-Esterhofen|R&#246;hrmoos|Hebertshausen|Dachau Bahnhof|M&#252;nchen-Karlsfeld|M&#252;nchen-Allach|M&#252;nchen-Untermenzing|M&#252;nchen-Obermenzing|M&#252;nchen-Laim|M&#252;nchen Hirschgarten|M&#252;nchen Donnersbergerbr&#252;cke|M&#252;nchen Hackerbr&#252;cke|M&#252;nchen Hbf (tief)|M&#252;nchen Karlsplatz|M&#252;nchen Marienplatz|M&#252;nchen Isartor|M&#252;nchen Rosenheimer Platz|M&#252;nchen Ost|M&#252;nchen Leuchtenbergring|M&#252;nchen-Berg am Laim|M&#252;nchen-Riem|Feldkirchen(b M&#252;nchen)|Heimstetten|Grub(Oberbay)|Poing|Markt Schwaben|Ottenhofen(Oberbay)|St Koloman|Aufhausen(b Erding)|Altenerding|Erding"/>
         *   </s>
         *   <s id="6452283056131474620-2203310738-27">
         *     <tl f="N" t="p" o="800765" c="RB" n="59089"/>
         *     <ar pt="2203311014" pp="4" l="16" ppth="N&#252;rnberg Hbf|Schwabach|Roth|Unterheckenhofen|Georgensgm&#252;nd|M&#252;hlstetten|Pleinfeld|Ellingen(Bay)|Wei&#223;enburg(Bay)|Treuchtlingen|Pappenheim|Solnhofen|Dollnstein|Eichst&#228;tt Bahnhof|Adelschlag|Tauberfeld|Eitensheim|Gaimersheim|Ingolstadt Audi|Ingolstadt Nord|Ingolstadt Hbf|Baar-Ebenhausen|Rohrbach(Ilm)|Pfaffenhofen(Ilm)|Reichertshausen(Ilm)|Paindorf"/>
         *     <dp pt="2203311015" pp="4" l="16" ppth="Dachau Bahnhof|M&#252;nchen Hbf"/>
         *   </s>
         *   <s id="8232795847690999194-2203310909-8">
         *     <tl f="N" t="p" o="800765" c="RE" n="4015"/>
         *     <ar pt="2203311037" pp="4" l="1" ppth="N&#252;rnberg Hbf|Allersberg(Rothsee)|Kinding(Altm&#252;hltal)|Ingolstadt Nord|Ingolstadt Hbf|Rohrbach(Ilm)|Pfaffenhofen(Ilm)"/>
         *     <dp pt="2203311038" pp="4" l="1" ppth="M&#252;nchen Hbf"/>
         *   </s>
         *   <s id="9210393654030543479-2203311052-1">
         *     <tl f="S" t="p" o="800725" c="S" n="6253"/>
         *     <dp pt="2203311052" pp="5" l="2" ppth="Vierkirchen-Esterhofen|R&#246;hrmoos|Hebertshausen|Dachau Bahnhof|M&#252;nchen-Karlsfeld|M&#252;nchen-Allach|M&#252;nchen-Untermenzing|M&#252;nchen-Obermenzing|M&#252;nchen-Laim|M&#252;nchen Hirschgarten|M&#252;nchen Donnersbergerbr&#252;cke|M&#252;nchen Hackerbr&#252;cke|M&#252;nchen Hbf (tief)|M&#252;nchen Karlsplatz|M&#252;nchen Marienplatz|M&#252;nchen Isartor|M&#252;nchen Rosenheimer Platz|M&#252;nchen Ost|M&#252;nchen Leuchtenbergring|M&#252;nchen-Berg am Laim|M&#252;nchen-Riem|Feldkirchen(b M&#252;nchen)|Heimstetten|Grub(Oberbay)|Poing|Markt Schwaben"/>
         *   </s>
         *   <s id="849499583125512445-2203310858-32">
         *     <tl f="S" t="p" o="800725" c="S" n="6240"/>
         *     <ar pt="2203311027" pp="5" l="2" ppth="Erding|Altenerding|Aufhausen(b Erding)|St Koloman|Ottenhofen(Oberbay)|Markt Schwaben|Poing|Grub(Oberbay)|Heimstetten|Feldkirchen(b M&#252;nchen)|M&#252;nchen-Riem|M&#252;nchen-Berg am Laim|M&#252;nchen Leuchtenbergring|M&#252;nchen Ost|M&#252;nchen Rosenheimer Platz|M&#252;nchen Isartor|M&#252;nchen Marienplatz|M&#252;nchen Karlsplatz|M&#252;nchen Hbf (tief)|M&#252;nchen Hackerbr&#252;cke|M&#252;nchen Donnersbergerbr&#252;cke|M&#252;nchen Hirschgarten|M&#252;nchen-Laim|M&#252;nchen-Obermenzing|M&#252;nchen-Untermenzing|M&#252;nchen-Allach|M&#252;nchen-Karlsfeld|Dachau Bahnhof|Hebertshausen|R&#246;hrmoos|Vierkirchen-Esterhofen"/>
         *   </s>
         *   <s id="-2480119884630018588-2203310838-32">
         *     <tl f="S" t="p" o="800725" c="S" n="6238"/>
         *     <ar pt="2203311007" pp="6" l="2" ppth="Erding|Altenerding|Aufhausen(b Erding)|St Koloman|Ottenhofen(Oberbay)|Markt Schwaben|Poing|Grub(Oberbay)|Heimstetten|Feldkirchen(b M&#252;nchen)|M&#252;nchen-Riem|M&#252;nchen-Berg am Laim|M&#252;nchen Leuchtenbergring|M&#252;nchen Ost|M&#252;nchen Rosenheimer Platz|M&#252;nchen Isartor|M&#252;nchen Marienplatz|M&#252;nchen Karlsplatz|M&#252;nchen Hbf (tief)|M&#252;nchen Hackerbr&#252;cke|M&#252;nchen Donnersbergerbr&#252;cke|M&#252;nchen Hirschgarten|M&#252;nchen-Laim|M&#252;nchen-Obermenzing|M&#252;nchen-Untermenzing|M&#252;nchen-Allach|M&#252;nchen-Karlsfeld|Dachau Bahnhof|Hebertshausen|R&#246;hrmoos|Vierkirchen-Esterhofen"/>
         *   </s>
         *   <s id="83079419740237979-2203311005-2">
         *     <tl f="N" t="p" o="800765" c="RE" n="4012"/>
         *     <ar pt="2203311023" pp="1" l="1" ppth="M&#252;nchen Hbf"/>
         *     <dp pt="2203311023" pp="1" l="1" ppth="Pfaffenhofen(Ilm)|Rohrbach(Ilm)|Ingolstadt Hbf"/></s><s id="-4897744624132342414-2203311027-3"><tl f="N" t="p" o="800765" c="RB" n="59150"/><ar pt="2203311048" pp="1" l="16" ppth="M&#252;nchen Hbf|Dachau Bahnhof"/><dp pt="2203311049" pp="1" l="16" ppth="Paindorf|Reichertshausen(Ilm)|Pfaffenhofen(Ilm)|Rohrbach(Ilm)|Baar-Ebenhausen|Ingolstadt Hbf|Ingolstadt Nord|Ingolstadt Audi|Gaimersheim|Eitensheim|Tauberfeld|Adelschlag|Eichst&#228;tt Bahnhof|Dollnstein|Solnhofen|Pappenheim|Treuchtlingen"/>
         *   </s>
         * </timetable>
         * */
        Timetable* timetable = (Timetable*)vp_timetable;

        create_pipe("station", xml::Data_Type::String, &timetable->station_name);
        create_pipe("eva",     xml::Data_Type::String, &timetable->station_eva_nr);
        defer {
            if (timetable->station_name.data)
                timetable->station_name = html_decode_string(timetable->station_name, true);
        };

        xml::push_handler_to_parser_stack({
            .key          = "s", // stops
            .open_handler = [] (char* xml, void* vp_timetable) -> u32 {
                Timetable* timetable = (Timetable*)vp_timetable;
                timetable->stops.append({});
                Timetable_Stop* last = &timetable->stops.last_element();
                last->messages.init();

                create_pipe("id", xml::Data_Type::String, &last->id);

                xml::push_handler_to_parser_stack({
                    .key          = "tl", // trip label
                    .open_handler = trip_label_parser,
                    .user_data = &last->trip_label
                });
                xml::push_handler_to_parser_stack({
                    .key           = "ar", // arrival event
                    .open_handler  = parse_event_start,
                    .close_handler = parse_event_end,
                    .user_data     = &last->arrival_event
                });
                xml::push_handler_to_parser_stack({
                    .key           = "dp", // departure event
                    .open_handler  = parse_event_start,
                    .close_handler = parse_event_end,
                    .user_data     = &last->departure_event
                });
                xml::push_handler_to_parser_stack({
                    .key          = "m", // messages
                    .open_handler = parse_message,
                    .user_data    = &(last->messages)
                });
                xml::push_handler_to_parser_stack({
                    .key          = "ref", // reference
                    .open_handler = trip_label_parser,
                    .user_data = &last->trip_label
                });

                return xml::parse_attributes(xml);
            },
            .close_handler = [] (char* xml, void*) -> u32 {
                xml::pop_handler_from_parser_stack(); // tl
                xml::pop_handler_from_parser_stack(); // ar
                xml::pop_handler_from_parser_stack(); // dp
                xml::pop_handler_from_parser_stack(); // m
                return xml::eat_until_end_of_tag(xml);
            },
            .user_data = vp_timetable
        });
        xml::push_handler_to_parser_stack({
            .key          = "m", // messages
            .open_handler = parse_message,
            .user_data    = &(timetable->messages)
        });

        return xml::parse_attributes(xml);
    }

    u32 timetable_parser_close(char* xml, void* vp_timetable) {
        xml::pop_handler_from_parser_stack(); // s
        xml::pop_handler_from_parser_stack(); // m
        return xml::eat_until_end_of_tag(xml);
    }


    u32 station_parser_open(char* xml, void* vp_station) {
        Station* station = (Station*)vp_station;

        create_pipe("name",  xml::Data_Type::String,  &station->name);
        create_pipe("eva",   xml::Data_Type::String,  &station->eva_nr);
        create_pipe("ds100", xml::Data_Type::String,  &station->ds100_nr);
        create_pipe("db",    xml::Data_Type::Boolean, &station->is_db);

        return xml::parse_attributes(xml);
    }

    u32 stations_parser_open(char* xml, void* vp_station) {
        xml::push_handler_to_parser_stack({
            .key          = "station",
            .open_handler = station_parser_open,
            .user_data    = vp_station
        });

        // no pipes, so just skip to end
        return xml::parse_attributes(xml);
    }

    u32 stations_parser_close(char* xml, void* vp_station) {
        xml::pop_handler_from_parser_stack();
        return xml::eat_until_end_of_tag(xml);
    }

    Station find_station(const char* station_name) {
        profile_function;

        String url_encoded_station = url_encode_string(station_name);
        defer {
            free(url_encoded_station.data);
        };

        Station station {};

        String_Builder sb = String_Builder::create_from({
            "https://api.deutschebahn.com/timetables/v1/station/",
            url_encoded_station.data
        });
        char* url = sb.build();
        defer {
            sb.deinit();
            free(url);
        };

        Request req {
            .authorization = authorization_token,
            .accept        = "application/xml",
            .url           = url,
        };

        Response res = net_request(req);
        print_response(res, 10000);
        if (res.response_code != 200) {
            println("sad response.");
            with_indentation(4) {
                print_response(res, 10000);
            }
            return {};
        }

        defer {
            res.free();
        };

        xml::push_handler_to_parser_stack({
            .key           = "stations",
            .open_handler  = stations_parser_open,
            .close_handler = stations_parser_close,
            .user_data     = &station
        });

        xml::parse(res.response.data);
        xml::pop_handler_from_parser_stack();

        return station;
    }

    s32 cmp_timetable_stop_by_id(const Timetable_Stop* t1, const Timetable_Stop* t2) {
        // TODO(Felix): depends on null termninator
        return strcmp(t1->id.data, t2->id.data);
    }

    void sort_timetable_stops_by_ids(Timetable* t) {
        t->stops.sort(cmp_timetable_stop_by_id);
    }

    void sort_timetable_stops_by_times(Timetable* t) {
        t->stops.sort([](const Timetable_Stop* tts1, const Timetable_Stop* tts2) -> s32 {
            Time t1 = tts1->get_relevant_time();
            Time t2 = tts2->get_relevant_time();

            return t1.compare(t2);
        });
    }

    Timetable get_timetable_from_url(String_Builder sb) {
        Timetable timetable {};
        timetable.messages.init();
        timetable.stops.init();

        char* url = sb.build();
        defer {
            sb.deinit();
            free(url);
        };

        Request req {
            .authorization = authorization_token,
            .accept        = "application/xml",
            .url           = url,
        };

        Response res = net_request(req);
        defer { res.free(); };

        if (res.response_code != 200) {
            println("sad response.");
            with_indentation(4) {
                print_response(res, 10000);
            }
            return {};
        }


        xml::push_handler_to_parser_stack({
            .key           = "timetable",
            .open_handler  = timetable_parser_open,
            .close_handler = timetable_parser_close,
            .user_data     = &timetable
        });

        xml::parse(res.response.data);
        xml::pop_handler_from_parser_stack();

        return timetable;
    }

    Timetable get_recent_changes(const char* eva_nr) {
        profile_function;
        String_Builder sb = String_Builder::create_from({
            "https://api.deutschebahn.com/timetables/v1/rchg/",
            eva_nr,
        });

        return get_timetable_from_url(sb);
    }

    Timetable get_full_changes(const char* eva_nr) {
        profile_function;
        String_Builder sb = String_Builder::create_from({
            "https://api.deutschebahn.com/timetables/v1/fchg/",
            eva_nr,
        });

        return get_timetable_from_url(sb);
    }

    Timetable get_timetable_one_hour(const char* eva_nr, Time time) {
        profile_function;
        char date_str[10];
        char hour_str[10];

        sprintf(date_str, "%02d%02d%02d", time.year-2000, time.month, time.day);
        sprintf(hour_str, "%02d", time.hour);
        String_Builder sb = String_Builder::create_from({
            "https://api.deutschebahn.com/timetables/v1/plan/",
            eva_nr, "/",
            date_str, "/",
            hour_str
        });

        return get_timetable_from_url(sb);
    }

    Timetable get_timetable(const char* eva_nr, Time time, u32 num_hours) {
        profile_function;

        if (num_hours == 0)
            return {};

        Timetable timetable = get_timetable_one_hour(eva_nr, time);

        for (s8 i = 1; i < num_hours; ++i) {
            Timetable extension =
                get_timetable_one_hour(eva_nr, time+Time_Diff{.hours=i});

            timetable.update(&extension, Merge_Type::Merge_Stops);

            extension.free();
        }

        Timetable update = get_full_changes(eva_nr);
        timetable.update(&update, Merge_Type::Update_Stops);
        update.free();

        update = get_recent_changes(eva_nr);
        timetable.update(&update, Merge_Type::Update_Stops);
        update.free();

        sort_timetable_stops_by_times(&timetable);

        return timetable;
    }

#define DB_MESSAGE_CODES_TPL_LST                                        \
    CODE( 0, R, "Begründung löschen",                  "keine Verspätungsbegründung" ) \
    CODE( 2, R, "BPOL/Polizei",                        "Polizeiliche Ermittlung" ) \
    CODE( 3, R, "Feuer",                               "Feuerwehreinsatz an der Strecke") \
    CODE( 4, R, "Personalausfall",                     "kurzfristiger Personalausfall") \
    CODE( 5, R, "Notarzt am Zug",                      "ärztliche Versorgung eines Fahrgastes") \
    CODE( 6, R, "Notbremse",                           "Betätigen der Notbremse") \
    CODE( 7, R, "Personen im Gl.",                     "Personen im Gleis" ) \
    CODE( 8, R, "Personenunfall",                      "Notarzteinsatz am Gleis") \
    CODE( 9, R, "Streik",                              "Streikauswirkungen") \
    CODE(10, R, "Tiere",                               "Tiere im Gleis") \
    CODE(11, R, "Unwetter",                            "Unwetter")      \
    CODE(12, R, "Anschluss Schiff",                    "Warten auf ein verspätetes Schiff") \
    CODE(13, R, "Zoll",                                "Pass- und Zollkontrolle") \
    CODE(14, R, "Technische Störung am Bahnhof",       "Technische Störung am Bahnhof") \
    CODE(15, R, "Vandalismus",                         "Beeinträchtigung durch Vandalismus") \
    CODE(16, R, "Fliegerbombe",                        "Entschärfung einer Fliegerbombe") \
    CODE(17, R, "Brückenbeschäd.",                     "Beschädigung einer Brücke") \
    CODE(18, R, "Baum im Gleis",                       "umgestürzter Baum im Gleis") \
    CODE(19, R, "BÜ-Unfall",                           "Unfall an einem Bahnübergang") \
    CODE(20, R, "Tiere(Wild) im Gleis",                "Tiere im Gleis") \
    CODE(21, R, "Anschluss Zug",                       "Warten auf Fahrgäste aus einem anderen Zug") \
    CODE(22, R, "Wetter",                              "Witterungsbedingte Störung") \
    CODE(23, R, "Feuer Bahngelände",                   "Feuerwehreinsatz auf Bahngelände") \
    CODE(24, R, "Ausland",                             "Verspätung im Ausland") \
    CODE(25, R, "Flügel/Kurswagen",                    "Warten auf weitere Wagen") \
    CODE(28, R, "Gegenst. im Gl.",                     "Gegenstände im Gleis") \
    CODE(29, R, "Ersatzverkehr",                       "Ersatzverkehr mit Bus ist eingerichtet") \
    CODE(31, R, "Bauarbeiten",                         "Bauarbeiten")   \
    CODE(32, R, "Haltezeit",                           "Verzögerung beim Ein-/Ausstieg") \
    CODE(33, R, "Oberleitung",                         "Oberleitungsstörung") \
    CODE(34, R, "Signalstörung",                       "Signalstörung") \
    CODE(35, R, "Streckensperrung",                    "Streckensperrung") \
    CODE(36, R, "techn. Stör. Zug",                    "technische Störung am Zug") \
    CODE(38, R, "techn. Stör.Strecke",                 "technische Störung an der Strecke") \
    CODE(39, R, "Zusatzwagen",                         "Anhängen von zusätzlichen Wagen") \
    CODE(40, R, "Stellwerk",                           "Stellwerksstörung /-ausfall") \
    CODE(41, R, "BÜ-Störung",                          "Störung an einem Bahnübergang") \
    CODE(42, R, "La-Stelle/EBA",                       "außerplanmäßige Geschwindigkeitsbeschränkung") \
    CODE(43, R, "Zugfolge/Abstand",                    "Verspätung eines vorausfahrenden Zuges") \
    CODE(44, R, "Kreuzung",                            "Warten auf einen entgegenkommenden Zug") \
    CODE(45, R, "Überholung",                          "Überholung")    \
    CODE(46, R, "besetzte Gleise",                     "Warten auf freie Einfahrt") \
    CODE(47, R, "Bereitstellung",                      "verspätete Bereitstellung des Zuges") \
    CODE(48, R, "Wende/Vorleistung",                   "Verspätung aus vorheriger Fahrt") \
    CODE(55, R, "techn. Stör. Folge",                  "technische Störung an einem anderen Zug") \
    CODE(56, R, "Anschluss Bus",                       "Warten auf Fahrgäste aus einem Bus") \
    CODE(57, R, "Zus. Halt",                           "Zusätzlicher Halt zum Ein-/Ausstieg für Reisende") \
    CODE(58, R, "Umleitung",                           "Umleitung des Zuges") \
    CODE(59, R, "Schnee und Eis",                      "Schnee und Eis") \
    CODE(60, R, "Geschw./Sturm",                       "Reduzierte Geschwindigkeit wegen Sturm") \
    CODE(61, R, "Türstörung",                          "Türstörung")    \
    CODE(62, R, "Stör. Zug behoben",                   "behobene technische Störung am Zug") \
    CODE(63, R, "techn. Untersuchung",                 "technische Untersuchung am Zug") \
    CODE(64, R, "Weichenstörung",                      "Weichenstörung") \
    CODE(65, R, "Erdrutsch",                           "Erdrutsch")     \
    CODE(66, R, "Hochwasser",                          "Hochwasser")    \
    CODE(70, Q, "WLAN Zug (q)",                        "WLAN im gesamten Zug nicht verfügbar") \
    CODE(71, Q, "WLAN Wagen (q)",                      "WLAN in einem/mehreren Wagen nicht verfügbar") \
    CODE(72, Q, "Info-/Entertainment (q)",             "Info-/Entertainment nicht verfügbar") \
    CODE(73, Q, "R: Mehrzweck vorn (q)",               "Heute: Mehrzweckabteil vorne") \
    CODE(74, Q, "R: Mehrzweck hinten (q)",             "Heute: Mehrzweckabteil hinten") \
    CODE(75, Q, "R: 1 Kl. vorn (q)",                   "Heute: 1. Klasse vorne") \
    CODE(76, Q, "R: 1 Kl. hinten (q)",                 "Heute: 1. Klasse hinten") \
    CODE(77, Q, "ohne 1. Kl. (q)",                     "ohne 1. Klasse") \
    CODE(79, Q, "R: ohne Mehrzweck (q)",               "ohne Mehrzweckabteil") \
    CODE(80, Q, "Abw. Reihung (q)",                    "andere Reihenfolge der Wagen") \
    CODE(82, Q, "Wagen fehlen (q)",                    "mehrere Wagen fehlen") \
    CODE(83, Q, "PRM-Einstiegshilfe",                  "Störung fahrzeuggebundene Einstiegshilfe") \
    CODE(84, Q, "Reihung ok (q)",                      "Zug verkehrt richtig gereiht") \
    CODE(85, Q, "Wagen fehlt (q)",                     "ein Wagen fehlt") \
    CODE(86, Q, "RES: Zug (q)",                        "gesamter Zug ohne Reservierung") \
    CODE(87, Q, "RES: Wagen (q)",                      "einzelne Wagen ohne Reservierung") \
    CODE(88, Q, "Qualität ok (q)",                     "keine Qualitätsmängel") \
    CODE(89, Q, "RES: ok (q)",                         "Reservierungen sind wieder vorhanden") \
    CODE(90, Q, "Bewirtschaftung fehlt (q)",           "kein gastronomisches Angebot") \
    CODE(91, Q, "fehlende Fahrradbef. (q)",            "fehlende Fahrradbeförderung") \
    CODE(92, Q, "eingeschr. Fahrradbef. (q)",          "Eingeschränkte Fahrradbeförderung") \
    CODE(93, Q, "PRM-Einrichtung (q)",                 "keine behindertengerechte Einrichtung") \
    CODE(94, Q, "Bewirtschaftung Ersatz Caddy/Abteil", "Ersatzbewirtschaftung") \
    CODE(95, Q, "PRM-WC (q)",                          "Ohne behindertengerechtes WC") \
    CODE(96, Q, "Überbesetzung Kulanz ",               "Überbesetzung mit Kulanzleistungen") \
    CODE(97, Q, "Überbes. ohne Kulanz",                "Überbesetzung ohne Kulanzleistungen") \
    CODE(98, Q, "sonstige Q-Mängel",                   "sonstige Qualitätsmängel") \
    CODE(99, R, "Sonstige Gründe",                     "Verzögerungen im Betriebsablauf") \

    const char* message_code_to_display_string(int code) {
        switch(code) {
#define CODE(num, t, s, l) case num: return l;
            DB_MESSAGE_CODES_TPL_LST
#undef CODE
            default: return "Unbekannter Nachrichten-Code";
        }
    }

    const char* message_type_to_display_string(String type) {
        if (type.data) {
            switch (type.data[0]) {
                case 'h': return "Hafas Information Manager message";
                case 'q': return "Quality Change";
                case 'f': return "Free text message";
                case 'd': return "Cause of delay";
                case 'i': return "IBIS message";
                case 'u': return "unassigned IBIS message";
                case 'r': return "Disruption";
                case 'w': return "Wagenstand";
                case 'c': return "Connection";
            }
        }

        return "Unbekannter Nachrichten-Typ";
    }

    void Station::free() {
        if (name)     name.free();
        if (eva_nr)   eva_nr.free();
        if (ds100_nr) ds100_nr.free();
    }

    void Station::print() {
        println("Station:");
        if (name)     println("  name:   %s", name.data);
        if (eva_nr)   println("  eva:    %s", eva_nr.data);
        if (ds100_nr) println("  ds100:  %s", ds100_nr.data);
        println("  ist DB: %s", is_db ? "Ja" : "Nein");
        println("");
    }

    void Trip_Label::free() {
        if (filter_tags)  filter_tags.free();
        if (trip_type)    trip_type.free();
        if (owner)        owner.free();
        if (category)     category.free();
        if (train_number) train_number.free();
    }

#define maybe_take(thing)                       \
    if (other->thing) {                         \
        thing = other->thing;                   \
        zero_out(other->thing);                 \
    }
#define maybe_take_maybe_free(thing)            \
    if (other->thing) {                         \
        if(thing) thing.free();                 \
        thing = other->thing;                   \
        zero_out(other->thing);                 \
    }

    void Trip_Label::update(Trip_Label* other) {
        maybe_take(filter_tags);
        maybe_take(trip_type);
        maybe_take(owner);
        maybe_take(category);
        maybe_take(train_number);
    }

    void Event::update(Event* other) {
        maybe_take_maybe_free(planned_path.string);
        maybe_take_maybe_free(changed_path.string);
        maybe_take_maybe_free(planned_platform);
        maybe_take_maybe_free(changed_platform);
        maybe_take(planned_time);
        maybe_take(changed_time);
        maybe_take_maybe_free(planned_status);
        maybe_take_maybe_free(changed_status);
        maybe_take(hidden);
        maybe_take(cancellation_time);
        maybe_take_maybe_free(wings);
        maybe_take_maybe_free(transition);
        maybe_take_maybe_free(planned_distant_endpoint);
        maybe_take_maybe_free(changed_distant_endpoint);
        maybe_take(distance_change);
        maybe_take_maybe_free(line);

        messages.update(&other->messages);
    }

    // NOTE(Felix): This sets the evalues from other to {} when they are used in
    //   the update, so that other can be freed later without freeing stuff in
    //   use by `this`
    void Timetable_Stop::update(Timetable_Stop* other) {
        maybe_take(eva_nr);

        if (other->trip_label) {
            if (!this->trip_label) {
                trip_label = other->trip_label;
                zero_out(other->trip_label);
            } else {
                trip_label.update(&other->trip_label);
            }
        }

        if (other->arrival_event) {
            if (!this->arrival_event) {
                arrival_event = other->arrival_event;
                zero_out(other->arrival_event);
            } else {
                arrival_event.update(&other->arrival_event);
            }
        }

        if (other->departure_event) {
            if (!this->departure_event) {
                departure_event = other->departure_event;
                zero_out(other->departure_event);
            } else {
                departure_event.update(&other->departure_event);
            }
        }

        messages.update(&other->messages);
    }

    s32 message_cmp(const Message* m1, const Message* m2) {
        // TODO(Felix): depends on null termninator
        return strcmp(m1->id.data, m2->id.data);
    }

    void Message_List::update(Message_List* other) {
        this->sort(message_cmp);
        other->sort(message_cmp);

        u32 old_message_count = count;
        Array_List<Message*> messages_to_zero_out;
        messages_to_zero_out.init();
        defer {
            for (Message* m: messages_to_zero_out) {
                zero_out(*m);
            }
            messages_to_zero_out.deinit();
        };

        for (Message& m : *other) {
            s32 index = sorted_find(m, message_cmp, 0, old_message_count);
            if (index == -1) {
                append(m);
                messages_to_zero_out.append(&m);
            }
        }
    }

    // NOTE(Felix): this removes used info from update timetable, so when it is
    //   freed later no data will be freed from the live version.
    void Timetable::update(Timetable* update, Merge_Type m_type) {
        if (update->station_eva_nr) {
            if (station_eva_nr)
                station_eva_nr.free();
            station_eva_nr = update->station_eva_nr;
            update->station_eva_nr = {};
        }

        switch (m_type) {
            case Merge_Type::Merge_Stops: {
                sort_timetable_stops_by_ids(this);

                u32 old_stop_count = stops.count;

                // NOTE(Felix): we zero it out so we can free the whole update
                //   struct wihout deleting live data, we also zero them out
                //   after adding them to the live data such that the IDs stay
                //   there, which is needed for sorted_find
                Array_List<Timetable_Stop*> indices_to_zero_out;
                indices_to_zero_out.init();
                defer {
                    for (Timetable_Stop* ts : indices_to_zero_out) {
                        zero_out(*ts);
                    }
                    indices_to_zero_out.deinit();
                };

                for (Timetable_Stop& s : update->stops) {
                    s32 index = stops.sorted_find(s, cmp_timetable_stop_by_id, 0, old_stop_count-1);
                    if (index == -1) {
                        // stop from update is not in base
                        stops.append(s);
                        indices_to_zero_out.append(&s);
                    }
                }

            } break;
            case Merge_Type::Update_Stops: {
                sort_timetable_stops_by_ids(update);

                for (Timetable_Stop& s : this->stops) {
                    s32 index = update->stops.sorted_find(s, cmp_timetable_stop_by_id);
                    if (index != -1) {
                        // stop from base has some update
                        Timetable_Stop* other = &update->stops[index];
                        s.update(other);
                    }
                }

                messages.update(&update->messages);

            } break;
        }
    }


    void Message::free() {
        if (id)            id.free();
        if (type)          type.free();
        if (text_internal) text_internal.free();
        if (text_external) text_external.free();
        if (category)      category.free();
        if (priority)      priority.free();
        if (owner)         owner.free();
        if (external_link) external_link.free();
    }

    void Message::print() {
        println( "Message: (ID: %s)", id.data);
        with_indentation (2) {
            if (text_external)     println("external text:  %s", text_external.data);
            if (text_internal)     println("internal text:  %s", text_internal.data);
            if (priority)          println("prio:           %s", priority.data);
            if (owner)             println("owner:          %s", owner.data);
            if (type)              println("type:          %2s (%s)", type.data, message_type_to_display_string(type));
            if (code)              println("code:          %2d (%s)", code.value, message_code_to_display_string(code.value));
            if (category)          println("category:       %s", category.data);
            if (category_external) println("category (ext): %d", category_external.value);
            if (external_link)     println("external link:  %s", external_link.data);
            if (deleted)           println("deleted:        %d", deleted.value);
        }
    }

    void Event::free() {
        planned_path.splits.deinit();
        planned_path.string.free();
        changed_path.splits.deinit();
        changed_path.string.free();
        planned_platform.free();
        changed_platform.free();
        planned_status.free();
        changed_status.free();
        wings.free();
        transition.free();
        planned_distant_endpoint.free();
        changed_distant_endpoint.free();
        line.free();

        if (messages.data) {
            for (Message m : messages) {
                m.free();
            }
            messages.deinit();
        }
    }

    void Event::print(bool is_arrival) {
        if (planned_time && changed_time && (changed_time.compare(planned_time)!=0)) {
               println("time: %02d:%02d Uhr (%+d)",
                       planned_time.hour, planned_time.minute,
                       (changed_time-planned_time).to_minutes());
        }
        else if (planned_time)
            println("time: %02d:%02d Uhr", planned_time.hour, planned_time.minute);


        if (is_arrival) {
            if (planned_path) {
                String origin = planned_path[0];
                println("planned origin: %{->Str}", &origin);
            }
            if (changed_path) {
                String origin = changed_path[0];
                println("changed origin: %{->Str}", &origin);
            }
        } else {
            if (planned_path) {
                String dest = planned_path[planned_path.num_splits()-1];
                println("planned dest: %{->Str}", &dest);
            }
            if (changed_path) {
                String dest = changed_path[changed_path.num_splits()-1];
                println("changed dest: %{->Str}", &dest);
            }
        }

        if (planned_platform)  println("platform: %s", planned_platform.data);
        if (messages.count) {
            println("Event Messages:");
            with_indentation(2) {
                for (Message m : messages) {
                    m.print();
                }
            }
        }
    }

    void Timetable_Stop::free() {
        id.free();
        eva_nr.free();

        trip_label.free();

        if (arrival_event)  arrival_event.free();
        if (departure_event) departure_event.free();

        if (messages.data) {
            for (Message m : messages) {
                m.free();
            }
            messages.deinit();
        }
    }

    void Timetable_Stop::print() {
        println("%s %s", trip_label.category.data,
                (departure_event)
                ? (departure_event.line.data
                   ? departure_event.line.data
                   : trip_label.train_number.data)
                : (arrival_event.line.data
                   ? arrival_event.line.data
                   : trip_label.train_number.data));

        with_indentation(2) {
            println("ID: %s", id.data);
            if (arrival_event) {
                println("<- Ankunft");
                with_indentation(2)
                    arrival_event.print(true);
            }

            if (departure_event) {
                println("-> Abfahrt");
                with_indentation(2)
                    departure_event.print(false);
            }

            if (messages.count) {
                println("Stop Messages:");
                with_indentation(2) {
                    for (Message m : messages) {
                        m.print();
                    }
                }
            }
        }

        println("");
    }

    Time Timetable_Stop::get_relevant_time() const  {
        // changed departure
        // planned departure
        // changed arrival
        // planned arrival
        if (departure_event && departure_event.changed_time) return departure_event.changed_time;
        if (departure_event && departure_event.planned_time) return departure_event.planned_time;
        if (arrival_event   && arrival_event.changed_time)   return arrival_event.changed_time;
        if (arrival_event   && arrival_event.planned_time)   return arrival_event.planned_time;
        return {};
    };


    void Timetable::free() {
        station_name.free();
        station_eva_nr.free();

        if (stops.data) {
            for (Timetable_Stop s : stops) {
                s.free();
            }
            stops.deinit();
        }
        if (messages.data) {
            for (Message m : messages) {
                m.free();
            }
            messages.deinit();
        }
    }

    void Timetable::print(Time after) {
        println("Timetable:");
        println("  name: %s", station_name.data);
        println("  eva:  %s", station_eva_nr.data);
        println("  stops:");

        with_indentation(4) {
            for (Timetable_Stop s : stops) {
                if (s.get_relevant_time().compare(after) > 0) {
                    s.print();
                }
            }
        }

        if (messages.count) {
            println("  Timetable Messages:");
            with_indentation(4) {
                for (Message m : messages) {
                    m.print();
                }
            }
        }

        println("");
    }

}
