#include <stdint.h>

typedef struct {
    const char* line;
    const char* destination;
    const char* track;
    uint16_t planned_time;
    int16_t  time_delta;
    const char* message;
} Simple_Timetable_Entry;

typedef struct {
    Simple_Timetable_Entry* entries;
    uint32_t                num_entries;
    const char*             message;
} Simple_Timetable;


enum Column_Types {
    LINE,
    DEST,
    TRACK,
    IN_MIN,
    PLUS_MINUS,
    DELAY,
};

typedef enum {
    SMALL,
    MEDIUM,
    LARGE,
} Font_Size;

typedef enum {
    GERMAN,
    ENGLISH,
} Language;

int init_display();
void deinit_display();
void display_timetable(Simple_Timetable, Language, Font_Size);
void display_mesage(const char*);
