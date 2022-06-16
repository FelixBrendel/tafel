#include "ftb/macros.hpp"
#define FTB_PRINT_IMPL
#define FTB_HASHMAP_IMPL

#include "xml.hpp"
#include "ftb/arraylist.hpp"
#include "ftb/print.hpp"
#include <cstdio>
#include <cstring>

namespace xml {

    struct Pipe {
        const char* attribute;
        Data_Type   type;
        void*       destination;
    };

    reader_function reader_functions[255];

    Array_List<Pipe> pipes;
    Stack<Parser_Stack_Entry> parser_stack;

    void parser_error(const char* fmt, ...) {
        println("%{color<}XML Parser error: %{>color}", console_red);
        va_list ap;
        va_start(ap, fmt);
        print_va_args(fmt, &ap);
        va_end(ap);

        println("\nParser stack (bottom to top):");
        for (Parser_Stack_Entry pse : parser_stack.array_list) {
            println("  { key: % 14s, open: %p, close: %p } ",
                    pse.key, pse.open_handler, pse.close_handler);
        }

        println("\nPipes (bottom to top):");
        for (Pipe p : pipes) {
            println("  { attribute: % 14s  } ",
                    p.attribute);
        }
        debug_break();
    }


    u32 skip_tag(char* xml, void* _) {
        return xml::parse_attributes(xml);
    }

    void init() {
        pipes.init();
        parser_stack.init();

        push_handler_to_parser_stack({
            .key = "?xml",
            .open_handler = skip_tag
        });
    }

    void deinit() {
        pipes.deinit();
        parser_stack.deinit();
    }

    void register_custom_reader_function(Data_Type t, reader_function fun) {
        reader_functions[(u8)t] = fun;
    }

    void push_handler_to_parser_stack(Parser_Stack_Entry e) {;
        parser_stack.push(e);
    }

    void pop_handler_from_parser_stack() {
        if (parser_stack.array_list.data) {
            parser_stack.pop();
        }
    }

    Parser_Stack_Entry get_handler(const char* key) {
        for (s32 i = parser_stack.array_list.count-1;
             i >= 0; --i)
        {
            Parser_Stack_Entry e = parser_stack.array_list[i];
            if (strcmp(e.key, key) == 0) {
                return e;
            }
        }

        return {};
    }

    Pipe get_pipe(char* attr) {
        for (Pipe p : pipes) {
            if (strcmp(p.attribute, attr) == 0) {
                return p;
            }
        }
        return {};
    }

    inline bool is_closing_tag(char* tag) {
        return tag[0] == '/';
    }

    inline bool is_quotes_char(char c) {
        return c == '\'' || c == '"';
    }

    inline bool is_whitespace(char c) {
        return c == ' '  || c == '\n' ||
            c == '\r' || c == '\t';
    }

    inline bool is_tag_char(char c) {
        return c == '<' || c == '>' || c == '/';
    }

    inline bool is_attrib_name_char(char c) {
        return
            c != 0            &&
            c != '='          &&
            !is_whitespace(c) &&
            !is_tag_char(c);
    }

    u32 eat_whitespace(char* xml) {
        u32 eaten = 0;
        while (is_whitespace(*xml)) {
            ++xml;
            ++eaten;
        }

        return eaten;
    }

    u32 eat_comment(char* xml) {
        // NOTE(Felix): Assumes we are on the < of the <!--
        u32 eaten = 2;
        while (xml[eaten] != '!' && xml[eaten+1] != '>') { // HACK(Felix): this +1 could run out of the string
            if (!xml[eaten]) {
                printf("Unexpected EOF in comment.\n");
                return eaten;
            }
            ++eaten;
        }

        return eaten;
    }

    u32 eat_string(char* xml) {
        // NOTE(Felix): Assumes we are on a " or '
        char to_search = *xml;

        u32 eaten = 1;
        bool escaped = false;
        while (xml[eaten] != to_search || escaped) {

            if (xml[eaten] == '\\') {
                if (escaped)
                    escaped = false;
                else
                    escaped = true;

            } else {
                if (escaped)
                    escaped = false;
            }
            ++eaten;
        }
        ++eaten; // eat the closing quote as well
        return eaten;
    }

    u32 eat_until_end_of_value(char* xml) {
        u32 eaten = 0;
        while (!is_whitespace(xml[eaten]) &&
               !is_tag_char(xml[eaten]))
        {
            ++eaten;
        }
        return eaten;
    }

    u32 eat_value(char* xml) {
        if (*xml == '"' || *xml == '\'')
            return eat_string(xml);
        else
            return eat_until_end_of_value(xml);
    }

    u32 eat_until_end_of_tag(char* xml) {
        // TODO(Felix): Nesting is not possible?
        s32 nesting_depth = 0;
        u32 eaten = 0;
        while (true) {
            if (!*xml) return eaten;

            if (*xml == '<') {
                if (*(xml+1) == '!') {
                    u32 comment = eat_comment(xml);
                    xml += comment;
                    eaten += comment;
                }
            } else if (*xml == '>')
                return eaten + 1;
            else if (*xml == '\'' || *xml == '\"') {
                u32 string_len = eat_string(xml);
                xml += string_len;
                eaten += string_len;
            }

            eaten++;
            xml++;

            if (nesting_depth == -1)
                return eaten;
        }
    }

    u32 read_string(char* xml, String* out_string) {
        // NOTE(Felix): Assumes we are on a " or '
        u32 length = eat_string(xml);

        // TODO(Felix): unescape string
        out_string->data = strndup(xml+1, length-2); // quotes are not part of the string
        out_string->length = length-2+1; // contains null terminator

        return length;

    }

    u32 read_int(char* xml, s32* out_int) {
        u32 length = eat_value(xml);
        if (is_quotes_char(*xml)) {
            char orig = xml[length];
            xml[length] = 0;
            defer {
                xml[length] = orig;
            };
            sscanf(xml+1, "%d", out_int);
        } else {
            char orig = xml[length+1];
            xml[length+1] = 0;
            defer {
                xml[length+1] = orig;
            };
            sscanf(xml, "%d", out_int);
        }
        return length;
    }

    u32 read_bool(char* xml, bool* out_bool) {
        // TODO(Felix): support unquoted one?
        String string;
        u32 read = read_string(xml, &string);
        defer {
            string.free();
        };
        // TODO(Felix): check tolower
        *out_bool = strcmp(string.data, "true") == 0;
        return read;
    }

    u32 peek_tag_name(char* xml, char** out_tag_name, u32* out_tag_len) {
        // NOTE(Felix): expects string to be starting after the '<'.

        u32 tag_name_length = 0;

        while (!is_whitespace(xml[tag_name_length]) &&
               xml[tag_name_length] != '>'          &&
               xml[tag_name_length] != '/')
        {
            ++tag_name_length;
        }

        *out_tag_name = strndup(xml, tag_name_length);
        *out_tag_len = tag_name_length;

        return tag_name_length;
    }

    void create_pipe(const char* attrib_name, xml::Data_Type type, void* dst) {
        pipes.append({attrib_name, type, dst});
    }

    u32 parse_attributes(char* attribs) {
        u32 total_parsed = 0;

        u32 parsed = 0;

        while (*attribs != '/' && *attribs != '>') {
            parsed = eat_whitespace(attribs);
            attribs += parsed;
            total_parsed += parsed;

            if (!*attribs) {
                printf("unexpeceted EOF in attribute list\n");
                return total_parsed;
            }

            // find end of attrib name
            u32 attrib_name_len = 0;
            while (is_attrib_name_char(*(attribs+attrib_name_len))) {
                ++attrib_name_len;
            }

            char* attrib_name = strndup(attribs, attrib_name_len);
            defer {
                free(attrib_name);
            };
            attribs += attrib_name_len;
            total_parsed += attrib_name_len;

            parsed = eat_whitespace(attribs);
            attribs += parsed;
            total_parsed += parsed;

            Pipe pipe = get_pipe(attrib_name);
            if (pipe.attribute) {
                // pipe was created
                if (*attribs == '=') {
                    // skip =
                    total_parsed++;
                    attribs++;
                    // skip whitespace
                    parsed = eat_whitespace(attribs);
                    attribs += parsed;
                    total_parsed += parsed;

                    // read thing
                    if (pipe.type == Data_Type::String) {
                        parsed = read_string(attribs, (String*)pipe.destination);
                    } else if (pipe.type == Data_Type::Boolean) {
                        parsed = read_bool(attribs, (bool*)pipe.destination);
                    } else if (pipe.type == Data_Type::Integer) {
                        parsed = read_int(attribs, (int*)pipe.destination);
                    }  else if (pipe.type == Data_Type::Maybe_Integer) {
                        Maybe<int>* mi = (Maybe<int>*)(pipe.destination);
                        parsed = read_int(attribs, &(mi->value));
                        mi->__exists = true;
                    }  else if (pipe.type == Data_Type::Maybe_Boolean) {
                        Maybe<bool>* mi = (Maybe<bool>*)(pipe.destination);
                        parsed = read_bool(attribs, &(mi->value));
                        mi->__exists = true;
                    } else {
                        // check user type
                        reader_function fun = reader_functions[(u8)pipe.type];
                        if (fun) {
                            parsed = fun(attribs, (void*)pipe.destination);
                        } else {
                            println("Pipe was created for the attribute %s "
                                    "however, the data type %d does not have a "
                                    "handler associated with it",
                                    pipe.attribute, (u8)pipe.type);
                        }
                    }

                    attribs += parsed;
                    total_parsed += parsed;
                } else {
                    // NOTE(Felix): if it is only the attrib and no value it's gotta
                    // be a bool
                    if (pipe.type != Data_Type::Boolean) {
                        printf("For attributes with no value, booleans have to be registered.\n");
                        return total_parsed;
                    }
                    *((bool*)pipe.destination) = true;
                }
            } else {
                // no pipe
                if (*attribs == '=') {
                    // skip value
                    {
                        // skip =
                        total_parsed++;
                        attribs++;
                        // skip whitespace
                        parsed = eat_whitespace(attribs);
                        attribs += parsed;
                        total_parsed += parsed;
                        // skip value
                        parsed = eat_until_end_of_value(attribs);
                        attribs += parsed;
                        total_parsed += parsed;
                    }
                }
                // else nothing to do
            }
        }

        if (*attribs == '/') {
            // skip /
            ++attribs;
            ++total_parsed;
        }
        // skip closing tag
        ++attribs;
        ++total_parsed;
        return total_parsed;
    }

    u32 parse(char* xml) {
        u32 total_parsed = 0;
        u32 parsed = 0;

        while (true) {
            parsed = eat_whitespace(xml);
            xml += parsed;
            total_parsed += parsed;

            if (!*xml) {
                break;
            }

            while (*xml != '<') {
                if (*xml == '"' || *xml == '\'')
                    xml += eat_string(xml);

                ++xml;
            }

            // if (*xml != '<') {
            //     parser_error("expected start of tag: '%.20s' at position %d\n",
            //                  xml, total_parsed);
            //     return parsed;
            // }

            pipes.clear();
            ++xml; // overstep <

            bool closing_tag = is_closing_tag(xml);
            if (closing_tag) {
                ++xml; // overstep /
            }

            char* tag;
            u32 tag_len;
            parsed = peek_tag_name(xml, &tag, &tag_len);
            defer {
                free(tag);
            };

            xml += parsed;
            total_parsed += parsed;

            Parser_Stack_Entry handler_entry = get_handler(tag);
            parser_function handler = nullptr;
            if (handler_entry.key) {
                if (closing_tag) {
                    handler = handler_entry.close_handler;
                } else {
                    handler = handler_entry.open_handler;
                }
            }

            if (handler)
                parsed = handler(xml, handler_entry.user_data);
            else {
                if (!closing_tag) {
                    printf("no tag handler for tag '%s'\n", tag);
                }
                parsed = eat_until_end_of_tag(xml);
            }

            xml += parsed;
        }

        return total_parsed;
    }
}
