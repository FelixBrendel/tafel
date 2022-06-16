#pragma once

#include "ftb/types.hpp"

struct UTF_8_Code_Point {
    u32 byte_length;  // [1 - 4]
    u32 code_point;   // 21-bit values
};

UTF_8_Code_Point get_code_point(u32 cp);
UTF_8_Code_Point bytes_to_code_point(const byte*);

// NOTE(Felix): Assumes that the out_str has enough bytes allocated to be able
//   to hold UTF_8_Code_Point::byte_length many bytes, returns the number of
//   bytes written.
u32 code_point_to_bytes(UTF_8_Code_Point cp, char* out_string);
u32 code_point_to_bytes(u32 cp, char* out_string);
u32 get_byte_length_for_code_point(u32 cp);
