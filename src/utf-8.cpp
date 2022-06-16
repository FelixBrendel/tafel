#include "utf-8.hpp"

UTF_8_Code_Point bytes_to_code_point(const byte* b) {
    UTF_8_Code_Point cp;

    if ((*b & 0b10000000) == 0) {
        // ascii
        return {
            .byte_length = 1,
            .code_point  = *b
        };
    } else if ((*b & 0b11100000) == 0b11000000) {
        // 2 byte
        return {
            .byte_length = 2,
            .code_point  = (u32)(b[0] & 0b00011111) << 6 |
                                (b[1] & 0b00111111),
        };
    } else if ((*b & 0b11110000) == 0b11100000) {
        // 3 byte
        return {
            .byte_length = 3,
            .code_point  = (u32)(b[0] & 0b00001111) << 12 |
                                (b[1] & 0b00111111) <<  6 |
                                (b[2] & 0b00111111),
        };
    } else {
        // 4 byte
        return {
            .byte_length = 3,
            .code_point  = (u32)(b[0] & 0b00000111) << 18 |
                                (b[1] & 0b00111111) << 12 |
                                (b[2] & 0b00111111) <<  6 |
                                (b[3] & 0b00111111),
        };
    }
}

u32 code_point_to_bytes(UTF_8_Code_Point cp, char* out_string) {
    byte* bytes = (byte*)out_string;
    if (cp.byte_length == 1) {
        bytes[0] = cp.code_point;
    } else if (cp.byte_length == 2) {
        bytes[0] = (((cp.code_point >> 6) & 0b00011111) | 0b11000000); // [6 - 10]
        bytes[1] = (((cp.code_point >> 0) & 0b00111111) | 0b10000000); // [0 -  5]
    } else if (cp.byte_length == 3) {
        bytes[0] = (((cp.code_point >> 12) & 0b00001111) | 0b11100000); // [12 - 15]
        bytes[1] = (((cp.code_point >>  6) & 0b00111111) | 0b10000000); // [ 6 - 11]
        bytes[2] = (((cp.code_point >>  0) & 0b00111111) | 0b10000000); // [ 0 - 5]
    } else {
        bytes[0] = (((cp.code_point >> 18) & 0b00000111) | 0b11110000); // [18 - 20]
        bytes[1] = (((cp.code_point >> 12) & 0b00111111) | 0b10000000); // [12 - 17]
        bytes[2] = (((cp.code_point >>  6) & 0b00111111) | 0b10000000); // [ 6 - 11]
        bytes[3] = (((cp.code_point >>  0) & 0b00111111) | 0b10000000); // [ 0 -  5]
    }
    return cp.byte_length;
}

u32 get_byte_length_for_code_point(u32 cp) {
    if (cp < 0x80)    return 1;
    if (cp < 0x0800)  return 2;
    if (cp < 0x10000) return 3;
    else              return 4;
}

u32 code_point_to_bytes(u32 cp, char* out_string) {
    UTF_8_Code_Point u_cp;
    u_cp.code_point = cp;
    u_cp.byte_length = get_byte_length_for_code_point(cp);
    return code_point_to_bytes(u_cp, out_string);
}
