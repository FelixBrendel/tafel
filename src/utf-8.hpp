#pragma once

#include "ftb/types.hpp"

struct UTF_8_Code_Point {
    u32 byte_length;  // [1 - 4]
    u32 code_point;   // 21-bit values
};

enum Unicode_Block {

};

UTF_8_Code_Point get_code_point(u32 cp);
UTF_8_Code_Point bytes_to_code_point(const byte*);

// NOTE(Felix): Assumes that the out_str has enough bytes allocated to be able
//   to hold UTF_8_Code_Point::byte_length many bytes, returns the number of
//   bytes written.
u32 code_point_to_bytes(UTF_8_Code_Point cp, char* out_string);
u32 code_point_to_bytes(u32 cp, char* out_string);
u32 get_byte_length_for_code_point(u32 cp);


/*
U+0000..U+007F	Basic Latin[g]
U+0080..U+00FF	Latin-1 Supplement[h]
U+0100..U+017F	Latin Extended-A
U+0180..U+024F	Latin Extended-B
U+0250..U+02AF	IPA Extensions
U+02B0..U+02FF	Spacing Modifier Letters
U+0300..U+036F	Combining Diacritical Marks
U+0370..U+03FF	Greek and Coptic
U+0400..U+04FF	Cyrillic
U+0500..U+052F	Cyrillic Supplement
U+0530..U+058F	Armenian
U+0590..U+05FF	Hebrew
U+0600..U+06FF	Arabic
U+0700..U+074F	Syriac
U+0750..U+077F	Arabic Supplement
U+0780..U+07BF	Thaana
U+07C0..U+07FF	NKo
U+0800..U+083F	Samaritan
U+0840..U+085F	Mandaic
U+0860..U+086F	Syriac Supplement
U+0870..U+089F	Arabic Extended-B
U+08A0..U+08FF	Arabic Extended-A
U+0900..U+097F	Devanagari
U+0980..U+09FF	Bengali
U+0A00..U+0A7F	Gurmukhi
U+0A80..U+0AFF	Gujarati
U+0B00..U+0B7F	Oriya
U+0B80..U+0BFF	Tamil
U+0C00..U+0C7F	Telugu
U+0C80..U+0CFF	Kannada
U+0D00..U+0D7F	Malayalam
U+0D80..U+0DFF	Sinhala
U+0E00..U+0E7F	Thai
U+0E80..U+0EFF	Lao
U+0F00..U+0FFF	Tibetan
U+1000..U+109F	Myanmar
U+10A0..U+10FF	Georgian
U+1100..U+11FF	Hangul Jamo
U+1200..U+137F	Ethiopic
U+1380..U+139F	Ethiopic Supplement
U+13A0..U+13FF	Cherokee
U+1400..U+167F	Unified Canadian Aboriginal Syllabics
U+1680..U+169F	Ogham
U+16A0..U+16FF	Runic
U+1700..U+171F	Tagalog
U+1720..U+173F	Hanunoo
U+1740..U+175F	Buhid
U+1760..U+177F	Tagbanwa
U+1780..U+17FF	Khmer
U+1800..U+18AF	Mongolian
U+18B0..U+18FF	Unified Canadian Aboriginal Syllabics Extended
U+1900..U+194F	Limbu
U+1950..U+197F	Tai Le
U+1980..U+19DF	New Tai Lue
U+19E0..U+19FF	Khmer Symbols
U+1A00..U+1A1F	Buginese
U+1A20..U+1AAF	Tai Tham
U+1AB0..U+1AFF	Combining Diacritical Marks Extended
U+1B00..U+1B7F	Balinese
U+1B80..U+1BBF	Sundanese
U+1BC0..U+1BFF	Batak
U+1C00..U+1C4F	Lepcha
U+1C50..U+1C7F	Ol Chiki
U+1C80..U+1C8F	Cyrillic Extended-C
U+1C90..U+1CBF	Georgian Extended
U+1CC0..U+1CCF	Sundanese Supplement
U+1CD0..U+1CFF	Vedic Extensions
U+1D00..U+1D7F	Phonetic Extensions
U+1D80..U+1DBF	Phonetic Extensions Supplement
U+1DC0..U+1DFF	Combining Diacritical Marks Supplement
U+1E00..U+1EFF	Latin Extended Additional
U+1F00..U+1FFF	Greek Extended
U+2000..U+206F	General Punctuation
U+2070..U+209F	Superscripts and Subscripts
U+20A0..U+20CF	Currency Symbols
U+20D0..U+20FF	Combining Diacritical Marks for Symbols
U+2100..U+214F	Letterlike Symbols
U+2150..U+218F	Number Forms
U+2190..U+21FF	Arrows
U+2200..U+22FF	Mathematical Operators
U+2300..U+23FF	Miscellaneous Technical
U+2400..U+243F	Control Pictures
U+2440..U+245F	Optical Character Recognition
U+2460..U+24FF	Enclosed Alphanumerics
U+2500..U+257F	Box Drawing
U+2580..U+259F	Block Elements
U+25A0..U+25FF	Geometric Shapes
U+2600..U+26FF	Miscellaneous Symbols
U+2700..U+27BF	Dingbats
U+27C0..U+27EF	Miscellaneous Mathematical Symbols-A
U+27F0..U+27FF	Supplemental Arrows-A
U+2800..U+28FF	Braille Patterns
U+2900..U+297F	Supplemental Arrows-B
U+2980..U+29FF	Miscellaneous Mathematical Symbols-B
U+2A00..U+2AFF	Supplemental Mathematical Operators
U+2B00..U+2BFF	Miscellaneous Symbols and Arrows
U+2C00..U+2C5F	Glagolitic
U+2C60..U+2C7F	Latin Extended-C
U+2C80..U+2CFF	Coptic
U+2D00..U+2D2F	Georgian Supplement
U+2D30..U+2D7F	Tifinagh
U+2D80..U+2DDF	Ethiopic Extended
U+2DE0..U+2DFF	Cyrillic Extended-A
U+2E00..U+2E7F	Supplemental Punctuation
U+2E80..U+2EFF	CJK Radicals Supplement
U+2F00..U+2FDF	Kangxi Radicals
U+2FF0..U+2FFF	Ideographic Description Characters
U+3000..U+303F	CJK Symbols and Punctuation
U+3040..U+309F	Hiragana
U+30A0..U+30FF	Katakana
U+3100..U+312F	Bopomofo
U+3130..U+318F	Hangul Compatibility Jamo
U+3190..U+319F	Kanbun
U+31A0..U+31BF	Bopomofo Extended
U+31C0..U+31EF	CJK Strokes
U+31F0..U+31FF	Katakana Phonetic Extensions
U+3200..U+32FF	Enclosed CJK Letters and Months
U+3300..U+33FF	CJK Compatibility
U+3400..U+4DBF	CJK Unified Ideographs Extension A
U+4DC0..U+4DFF	Yijing Hexagram Symbols
U+4E00..U+9FFF	CJK Unified Ideographs
U+A000..U+A48F	Yi Syllables
U+A490..U+A4CF	Yi Radicals
U+A4D0..U+A4FF	Lisu
U+A500..U+A63F	Vai
U+A640..U+A69F	Cyrillic Extended-B
U+A6A0..U+A6FF	Bamum
U+A700..U+A71F	Modifier Tone Letters
U+A720..U+A7FF	Latin Extended-D
U+A800..U+A82F	Syloti Nagri
U+A830..U+A83F	Common Indic Number Forms
U+A840..U+A87F	Phags-pa
U+A880..U+A8DF	Saurashtra
U+A8E0..U+A8FF	Devanagari Extended
U+A900..U+A92F	Kayah Li
U+A930..U+A95F	Rejang
U+A960..U+A97F	Hangul Jamo Extended-A
U+A980..U+A9DF	Javanese
U+A9E0..U+A9FF	Myanmar Extended-B
U+AA00..U+AA5F	Cham
U+AA60..U+AA7F	Myanmar Extended-A
U+AA80..U+AADF	Tai Viet
U+AAE0..U+AAFF	Meetei Mayek Extensions
U+AB00..U+AB2F	Ethiopic Extended-A
U+AB30..U+AB6F	Latin Extended-E
U+AB70..U+ABBF	Cherokee Supplement
U+ABC0..U+ABFF	Meetei Mayek
U+AC00..U+D7AF	Hangul Syllables
U+D7B0..U+D7FF	Hangul Jamo Extended-B
U+D800..U+DB7F	High Surrogates
U+DB80..U+DBFF	High Private Use Surrogates
U+DC00..U+DFFF	Low Surrogates
U+E000..U+F8FF	Private Use Area
U+F900..U+FAFF	CJK Compatibility Ideographs
U+FB00..U+FB4F	Alphabetic Presentation Forms
U+FB50..U+FDFF	Arabic Presentation Forms-A
U+FE00..U+FE0F	Variation Selectors
U+FE10..U+FE1F	Vertical Forms
U+FE20..U+FE2F	Combining Half Marks
U+FE30..U+FE4F	CJK Compatibility Forms
U+FE50..U+FE6F	Small Form Variants
U+FE70..U+FEFF	Arabic Presentation Forms-B
U+FF00..U+FFEF	Halfwidth and Fullwidth Forms
U+FFF0..U+FFFF	Specials
U+10000..U+1007F	Linear B Syllabary
U+10080..U+100FF	Linear B Ideograms
U+10100..U+1013F	Aegean Numbers
U+10140..U+1018F	Ancient Greek Numbers
U+10190..U+101CF	Ancient Symbols
U+101D0..U+101FF	Phaistos Disc
U+10280..U+1029F	Lycian
U+102A0..U+102DF	Carian
U+102E0..U+102FF	Coptic Epact Numbers
U+10300..U+1032F	Old Italic
U+10330..U+1034F	Gothic
U+10350..U+1037F	Old Permic
U+10380..U+1039F	Ugaritic
U+103A0..U+103DF	Old Persian
U+10400..U+1044F	Deseret
U+10450..U+1047F	Shavian
U+10480..U+104AF	Osmanya
U+104B0..U+104FF	Osage
U+10500..U+1052F	Elbasan
U+10530..U+1056F	Caucasian Albanian
U+10570..U+105BF	Vithkuqi
U+10600..U+1077F	Linear A
U+10780..U+107BF	Latin Extended-F
U+10800..U+1083F	Cypriot Syllabary
U+10840..U+1085F	Imperial Aramaic
U+10860..U+1087F	Palmyrene
U+10880..U+108AF	Nabataean
U+108E0..U+108FF	Hatran
U+10900..U+1091F	Phoenician
U+10920..U+1093F	Lydian
U+10980..U+1099F	Meroitic Hieroglyphs
U+109A0..U+109FF	Meroitic Cursive
U+10A00..U+10A5F	Kharoshthi
U+10A60..U+10A7F	Old South Arabian
U+10A80..U+10A9F	Old North Arabian
U+10AC0..U+10AFF	Manichaean
U+10B00..U+10B3F	Avestan
U+10B40..U+10B5F	Inscriptional Parthian
U+10B60..U+10B7F	Inscriptional Pahlavi
U+10B80..U+10BAF	Psalter Pahlavi
U+10C00..U+10C4F	Old Turkic
U+10C80..U+10CFF	Old Hungarian
U+10D00..U+10D3F	Hanifi Rohingya
U+10E60..U+10E7F	Rumi Numeral Symbols
U+10E80..U+10EBF	Yezidi
U+10F00..U+10F2F	Old Sogdian
U+10F30..U+10F6F	Sogdian
U+10F70..U+10FAF	Old Uyghur
U+10FB0..U+10FDF	Chorasmian
U+10FE0..U+10FFF	Elymaic
U+11000..U+1107F	Brahmi
U+11080..U+110CF	Kaithi
U+110D0..U+110FF	Sora Sompeng
U+11100..U+1114F	Chakma
U+11150..U+1117F	Mahajani
U+11180..U+111DF	Sharada
U+111E0..U+111FF	Sinhala Archaic Numbers
U+11200..U+1124F	Khojki
U+11280..U+112AF	Multani
U+112B0..U+112FF	Khudawadi
U+11300..U+1137F	Grantha
U+11400..U+1147F	Newa
U+11480..U+114DF	Tirhuta
U+11580..U+115FF	Siddham
U+11600..U+1165F	Modi
U+11660..U+1167F	Mongolian Supplement
U+11680..U+116CF	Takri
U+11700..U+1174F	Ahom
U+11800..U+1184F	Dogra
U+118A0..U+118FF	Warang Citi
U+11900..U+1195F	Dives Akuru
U+119A0..U+119FF	Nandinagari
U+11A00..U+11A4F	Zanabazar Square
U+11A50..U+11AAF	Soyombo
U+11AB0..U+11ABF	Unified Canadian Aboriginal Syllabics Extended-A
U+11AC0..U+11AFF	Pau Cin Hau
U+11C00..U+11C6F	Bhaiksuki
U+11C70..U+11CBF	Marchen
U+11D00..U+11D5F	Masaram Gondi
U+11D60..U+11DAF	Gunjala Gondi
U+11EE0..U+11EFF	Makasar
U+11FB0..U+11FBF	Lisu Supplement
U+11FC0..U+11FFF	Tamil Supplement
U+12000..U+123FF	Cuneiform
U+12400..U+1247F	Cuneiform Numbers and Punctuation
U+12480..U+1254F	Early Dynastic Cuneiform
U+12F90..U+12FFF	Cypro-Minoan
U+13000..U+1342F	Egyptian Hieroglyphs
U+13430..U+1343F	Egyptian Hieroglyph Format Controls
U+14400..U+1467F	Anatolian Hieroglyphs
U+16800..U+16A3F	Bamum Supplement
U+16A40..U+16A6F	Mro
U+16A70..U+16ACF	Tangsa
U+16AD0..U+16AFF	Bassa Vah
U+16B00..U+16B8F	Pahawh Hmong
U+16E40..U+16E9F	Medefaidrin
U+16F00..U+16F9F	Miao
U+16FE0..U+16FFF	Ideographic Symbols and Punctuation
U+17000..U+187FF	Tangut
U+18800..U+18AFF	Tangut Components
U+18B00..U+18CFF	Khitan Small Script
U+18D00..U+18D7F	Tangut Supplement
U+1AFF0..U+1AFFF	Kana Extended-B
U+1B000..U+1B0FF	Kana Supplement
U+1B100..U+1B12F	Kana Extended-A
U+1B130..U+1B16F	Small Kana Extension
U+1B170..U+1B2FF	Nushu
U+1BC00..U+1BC9F	Duployan
U+1BCA0..U+1BCAF	Shorthand Format Controls
U+1CF00..U+1CFCF	Znamenny Musical Notation
U+1D000..U+1D0FF	Byzantine Musical Symbols
U+1D100..U+1D1FF	Musical Symbols
U+1D200..U+1D24F	Ancient Greek Musical Notation
U+1D2E0..U+1D2FF	Mayan Numerals
U+1D300..U+1D35F	Tai Xuan Jing Symbols
U+1D360..U+1D37F	Counting Rod Numerals
U+1D400..U+1D7FF	Mathematical Alphanumeric Symbols
U+1D800..U+1DAAF	Sutton SignWriting
U+1DF00..U+1DFFF	Latin Extended-G
U+1E000..U+1E02F	Glagolitic Supplement
U+1E100..U+1E14F	Nyiakeng Puachue Hmong
U+1E290..U+1E2BF	Toto
U+1E2C0..U+1E2FF	Wancho
U+1E7E0..U+1E7FF	Ethiopic Extended-B
U+1E800..U+1E8DF	Mende Kikakui
U+1E900..U+1E95F	Adlam
U+1EC70..U+1ECBF	Indic Siyaq Numbers
U+1ED00..U+1ED4F	Ottoman Siyaq Numbers
U+1EE00..U+1EEFF	Arabic Mathematical Alphabetic Symbols
U+1F000..U+1F02F	Mahjong Tiles
U+1F030..U+1F09F	Domino Tiles
U+1F0A0..U+1F0FF	Playing Cards
U+1F100..U+1F1FF	Enclosed Alphanumeric Supplement
U+1F200..U+1F2FF	Enclosed Ideographic Supplement
U+1F300..U+1F5FF	Miscellaneous Symbols and Pictographs
U+1F600..U+1F64F	Emoticons
U+1F650..U+1F67F	Ornamental Dingbats
U+1F680..U+1F6FF	Transport and Map Symbols
U+1F700..U+1F77F	Alchemical Symbols
U+1F780..U+1F7FF	Geometric Shapes Extended
U+1F800..U+1F8FF	Supplemental Arrows-C
U+1F900..U+1F9FF	Supplemental Symbols and Pictographs
U+1FA00..U+1FA6F	Chess Symbols
U+1FA70..U+1FAFF	Symbols and Pictographs Extended-A
U+1FB00..U+1FBFF	Symbols for Legacy Computing
U+20000..U+2A6DF	CJK Unified Ideographs Extension B
U+2A700..U+2B73F	CJK Unified Ideographs Extension C
U+2B740..U+2B81F	CJK Unified Ideographs Extension D
U+2B820..U+2CEAF	CJK Unified Ideographs Extension E
U+2CEB0..U+2EBEF	CJK Unified Ideographs Extension F
U+2F800..U+2FA1F	CJK Compatibility Ideographs Supplement
U+30000..U+3134F	CJK Unified Ideographs Extension G
U+E0000..U+E007F	Tags
U+E0100..U+E01EF	Variation Selectors Supplement
U+F0000..U+FFFFF	Supplementary Private Use Area-A
U+100000..U+10FFFF	Supplementary Private Use Area-B  
*/
