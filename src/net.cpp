#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include "ftb/arraylist.hpp"
#include "net.h"
#include "utf-8.hpp"

CURL *curl;
CURLcode res;

bool net_init() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    return curl != 0;
}

void net_deinit() {
    curl_easy_cleanup(curl);
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

size_t WriteCallback(void *data, size_t size, size_t nmemb, void *userp){
    size_t realsize = size * nmemb;
    String* mem = (String*)userp;

    char* ptr = (char*)realloc((void*)mem->data, mem->length + realsize + 1);
    if(ptr == NULL)
        return 0;  /* out of memory! */

    mem->data = ptr;
    memcpy(&(mem->data[mem->length]), data, realsize);
    mem->length += realsize;
    mem->data[mem->length] = 0;

    return realsize;
}

Response net_request(Request req) {
    Response r {};

    curl_easy_setopt(curl, CURLOPT_URL, req.url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,  (void*)&(r.response));
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void*)&(r.header));

    curl_slist *header_args = NULL;
    if (req.authorization || req.accept) {
        String_Builder sb;
        sb.init();
        defer {
            sb.deinit();
        };

        if (req.authorization) {
            sb.append("Authorization: ");
            sb.append(req.authorization);
            char* str = sb.build();
            defer {
                sb.clear();
                free(str);
            };
            header_args = curl_slist_append(header_args, str);
        }
        if (req.accept) {
            sb.append("Accept: ");
            sb.append(req.accept);
            char* str = sb.build();
            defer {
                sb.clear();
                free(str);
            };
            header_args = curl_slist_append(header_args, str);
        }
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_args);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, (void*)(&r.response_code));
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME,    (void*)(&r.elapsed_time));

    curl_slist_free_all(header_args);

    /* Check for errors */
    if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));


    return r;
}

bool is_url_char(const char c) {
    return
        ('0' <= c && c <= '9') ||
        ('a' <= c && c <= 'z') ||
        ('A' <= c && c <= 'A') ||
        (c == '-') || (c == '.') ||
        (c == '_') || (c == '~');
}

union Two_Hex_Chars {
    u16  _u16;
    char _char[2];
};

inline Two_Hex_Chars byte_to_hex_chars(byte b) {
    Two_Hex_Chars result;

    byte nibble1 = b >> 4;
    byte nibble2 = b & 0b00001111;

    if (nibble1 <= 9) {
        result._char[0] = '0' + nibble1;
    } else {
        result._char[0] = 'A' + nibble1 - 10;
    }

    if (nibble2 <= 9) {
        result._char[1] = '0' + nibble2;
    } else {
        result._char[1] = 'A' + nibble2 - 10;
    }

    return result;
}

inline byte hex_char_to_byte(char hex) {
    if (hex >= '0' && hex <= '9') {
        return hex - '0';
    } else if (hex >= 'A' && hex <= 'F') {
        return hex - 'A' + 10;
    } else {
        return hex - 'a' + 10;
    }
}

inline byte hex_chars_to_byte(Two_Hex_Chars hex) {
    byte b;

    b  = hex_char_to_byte(hex._char[0]) << 4;
    b |= hex_char_to_byte(hex._char[1]) << 0;

    return b;
}

// münchen hbf -> m%C3%BCnchen%20hbf
String url_encode_string(const char* c) {
    u64 len = strlen(c);

    String result {
        .data   = (char*)malloc(len*3 + 1), // worst case
        .length = 0
    };

    char* write_head = result.data;

    while (*c) {
        if (is_url_char(c[0])) {
            write_head[0] = c[0];
            ++write_head;
        } else {
            Two_Hex_Chars hex = byte_to_hex_chars(*c);
            write_head[0] = '%';
            write_head[1] = hex._char[0];
            write_head[2] = hex._char[1];
            write_head += 3;
        }
        ++c;
    }

    *write_head = '\0';
    result.length = write_head - result.data;

    return result;
}

// m%C3%BCnchen%20hbf -> münchen hbf
String url_decode_string(String s, bool do_inline) {
    u64 len = s.length;
    char* c = s.data;

    String result;

    if (do_inline) {
        result = s;
    } else {
        result = {
            .data   = (char*)malloc(len + 1), // worst case
            .length = 0
        };
    }

    char* write_head = result.data;

    while (*c) {
        if (c[0] == '%') {
            Two_Hex_Chars hex;
            hex._char[0] = c[1];
            hex._char[1] = c[2];

            byte b = hex_chars_to_byte(hex);

            write_head[0] = b;
            c += 3;
        } else {
            write_head[0] = c[0];
            ++c;
        }
        ++write_head;
    }

    *write_head = '\0';
    result.length = write_head - result.data;

    return result;
}


// münchen is a city in baveria -> m&#252;nchen is a city in baveria
String html_encode_string(String) {
    return {};
}

String html_decode_string(String s, bool do_inline) {
    // NOTE(Felix): for now only supports number-kind of encoding like in
    // M&#252;nchen with &#<unicode-code-point>;

    char* c = s.data;
    u64 len = strlen(c);

    String result;

    if (do_inline) {
        result = s;
    } else {
        result = {
            .data   = (char*)malloc(len + 1), // worst case
            .length = 0
        };
    }

    char* write_head = result.data;

    while (*c) {
        if (c[0] == '&' && c[1] == '#') {
            char* number_start = c + 2;
            char* number_end = number_start;
            while (number_end[0] >= '0' && number_end[0] <= '9') {
                ++number_end;
            }

            u32 cp;
            {
                char end = number_end[0];
                number_end[0] = 0;
                defer {
                    number_end[0] = end;
                };

                sscanf(number_start, "%u", &cp);
            }

            if (number_end[0] == ';') {
                u32 written = code_point_to_bytes(cp, write_head);
                write_head += written;
                c = number_end + 1;
                continue;
            }
        }
        write_head[0] = c[0];
        ++c;

        ++write_head;
    }

    *write_head = '\0';

    result.length = write_head - result.data;

    return result;

}
