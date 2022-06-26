#pragma once
#include "ftb/types.hpp"
#include "ftb/bucket_allocator.hpp"

struct Response {
    long   response_code;
    double elapsed_time;
    String header;
    String response;

    void free() {
        if (header)   header.free();
        if (response) response.free();
    }
};

struct Request {
    const char* url;
    const char* accept;

    u32          header_arg_count;
    const char** header_args;
};

bool net_init();
void net_deinit();
Response net_request(Request);


// münchen is a city in baveria -> m&#252;nchen is a city in baveria
String html_encode_string(String);
String html_decode_string(String, bool do_inline);

// münchen is a city in baveria -> m%C3%BCnchen%20is%20a%20city%20in%20baveria
String url_encode_string(const char*);
String url_decode_string(const char*, bool do_inline);
