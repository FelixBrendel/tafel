#pragma once

#include "ftb/arraylist.hpp"
#include "ftb/types.hpp"
#include "ftb/hashmap.hpp"

template <typename type>
struct Maybe : type {
    bool __exists;

    void operator=(type v) {
        memcpy(this, &v, sizeof(type));
        __exists = true;
    }

    operator bool() {
        return __exists;
    }
};

struct Empty {};

#define DEFINE_BASIC_TYPE_MAYBE(type)           \
    template <>                                 \
    struct Maybe<type> : Maybe<Empty> {         \
        type value;                             \
        void operator=(type v) {                \
            value = v;                          \
            __exists = true;                    \
        }                                       \
        type operator* () {                     \
            return value;                       \
        }                                       \
    }                                           \

DEFINE_BASIC_TYPE_MAYBE(bool);
DEFINE_BASIC_TYPE_MAYBE(u8);
DEFINE_BASIC_TYPE_MAYBE(u16);
DEFINE_BASIC_TYPE_MAYBE(u32);
DEFINE_BASIC_TYPE_MAYBE(u64);
DEFINE_BASIC_TYPE_MAYBE(s8);
DEFINE_BASIC_TYPE_MAYBE(s16);
DEFINE_BASIC_TYPE_MAYBE(s32);
DEFINE_BASIC_TYPE_MAYBE(s64);
DEFINE_BASIC_TYPE_MAYBE(f32);
DEFINE_BASIC_TYPE_MAYBE(f64);

#undef DEFINE_BASIC_TYPE_MAYBE

namespace xml {
    typedef u32 (*parser_function)(char* position, void* user_data);
    typedef u32 (*reader_function)(char* position, void* out_read_value);

    struct Parser_Stack_Entry {
        const char*     key;
        parser_function open_handler;
        parser_function close_handler;
        void*           user_data;
    };

    enum struct Data_Type : u8 {
        String = 1 << 7,
        Integer,
        Boolean,
        Maybe_Integer,
        Maybe_Boolean
    };

    void init();
    void deinit();

    void register_custom_reader_function(Data_Type, reader_function);
    void create_pipe(const char* attrib_name, Data_Type type, void* dst);
    void push_handler_to_parser_stack(Parser_Stack_Entry e);
    void pop_handler_from_parser_stack();

    Parser_Stack_Entry get_handler(const char* key);

    bool is_closing_tag(char* tag);
    bool is_quotes_char(char c);
    bool is_whitespace(char c);
    bool is_tag_char(char c);
    bool is_attrib_name_char(char c);

    u32 read_string(char* xml, String* out_string);
    u32 read_int(char* xml, s32* out_int);
    u32 read_bool(char* xml, bool* out_bool);

    u32 eat_whitespace(char* xml);
    u32 eat_comment(char* xml);
    u32 eat_string(char* xml);
    u32 eat_until_end_of_tag(char* xml);

    u32 peek_tag_name(char* xml, char** out_tag_name, u32* out_tag_len);

    u32 parse(char* string);
    u32 parse_attributes(char* string);
    u32 default_handler(char* position, void* user_data);
}
