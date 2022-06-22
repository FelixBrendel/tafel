#pragma once

#define MAX_HEIGHT_FONT         41
#define MAX_WIDTH_FONT          32
#define OFFSET_BITMAP

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

//ASCII
typedef struct {
  const uint8_t *table;
  uint16_t char_px_width;
  uint16_t char_px_height;

} UTF8_FONT;


/* extern UTF8_FONT Font24; */
/* extern UTF8_FONT Font20; */
/* extern UTF8_FONT Font16; */
extern UTF8_FONT UTF8_Font12;
/* extern UTF8_FONT Font8; */

#ifdef __cplusplus
}
#endif
