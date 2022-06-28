#include <time.h>
#include <signal.h>  // signal()
#include <stdlib.h>  // exit() malloc() free()
#include <stdint.h>

#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"
#include "ImageData.h"
#include "Debug.h"
#include "EPD_2in9_V2.h"

#include "../../display.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define array_length(arr) sizeof((arr))/sizeof((arr)[0])

 /* EPD_2IN9_V2_WIDTH  :: 128 */
 /* EPD_2IN9_V2_HEIGHT :: 296 */

const char** headings[] = {
    [GERMAN] = (const char*[]){
        [LINE]        = "Linie",
        [DEST]        = "Ziel",
        [TRACK]       = "Gleis",
        [IN_MIN]      = "in min",
        [PLUS_MINUS]  = "",
        [DELAY]       = ""
    },
    [ENGLISH] = (const char*[]){
        [LINE]        = "Line",
        [DEST]        = "Destination",
        [TRACK]       = "Track",
        [IN_MIN]      = "in min",
        [PLUS_MINUS]  = "",
        [DELAY]       = ""
    },
};

void  Handler(int signo) {
    //System Exit
    printf("\r\nHandler:exit\r\n");
    DEV_Module_Exit();

    exit(0);
}

UBYTE *BlackImage;

int demo() {
    {
        /* printf("EPD_2IN9_V2_test Demo\r\n"); */
        /* if(DEV_Module_Init()!=0){ */
        /*     return -1; */
        /* } */

        /* printf("e-Paper Init and Clear...\r\n"); */
        /* EPD_2IN9_V2_Init(); */

        struct timespec start={0,0}, finish={0,0};
        clock_gettime(CLOCK_REALTIME,&start);
        EPD_2IN9_V2_Clear();
        clock_gettime(CLOCK_REALTIME,&finish);
        printf("%ld S\r\n",finish.tv_sec-start.tv_sec);

        //Create a new image cache
        /* UBYTE *BlackImage; */
        /* UWORD Imagesize = ((EPD_2IN9_V2_WIDTH % 8 == 0)? (EPD_2IN9_V2_WIDTH / 8 ): (EPD_2IN9_V2_WIDTH / 8 + 1)) * EPD_2IN9_V2_HEIGHT; */
        /* if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) { */
            /* printf("Failed to apply for black memory...\r\n"); */
            /* return -1; */
        /* } */
        printf("Paint_NewImage\r\n");
        Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
        Paint_Clear(WHITE);

#if 0   // show bmp
        Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 0, WHITE);
        printf("show window BMP-----------------\r\n");
        Paint_SelectImage(BlackImage);
        GUI_ReadBmp("./pic/100x100.bmp", 10, 10);
        EPD_2IN9_V2_Display(BlackImage);
        DEV_Delay_ms(3000);

        printf("show bmp------------------------\r\n");
        Paint_SelectImage(BlackImage);
        GUI_ReadBmp("./pic/2in9.bmp", 0, 0);
        EPD_2IN9_V2_Display(BlackImage);
        DEV_Delay_ms(3000);
#endif

#if 1  //show image for array
        Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
        printf("show image for array\r\n");
        Paint_SelectImage(BlackImage);
        Paint_Clear(WHITE);
        Paint_DrawBitMap(gImage_2in9);

        EPD_2IN9_V2_Display(BlackImage);
        DEV_Delay_ms(3000);
#endif

#if 1  // Drawing on the image
        Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
        printf("Drawing\r\n");
        //1.Select Image
        Paint_SelectImage(BlackImage);
        Paint_Clear(WHITE);

        // 2.Drawing on the image
        printf("Drawing:BlackImage\r\n");
        Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
        Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
        Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);

        Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
        Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

        Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
        Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

        Paint_DrawCircle(45, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
        Paint_DrawCircle(105, 95, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);

        Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
        Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

        Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
        Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);

        Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);
        Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);

        /* Paint_DrawString_CN(130, 0,"你好abc", &Font12CN, BLACK, WHITE); */
        /* Paint_DrawString_CN(130, 20, "微雪电子", &Font24CN, WHITE, BLACK); */

        EPD_2IN9_V2_Display_Base(BlackImage);
        DEV_Delay_ms(3000);
#endif

#if 1   //Partial refresh, example shows time
        Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
        printf("Partial refresh\r\n");
        Paint_SelectImage(BlackImage);
        Paint_Clear(BLACK);

        PAINT_TIME sPaint_time;
        sPaint_time.Hour = 12;
        sPaint_time.Min = 34;
        sPaint_time.Sec = 56;
        UBYTE num = 10;
        for (;;) {
            sPaint_time.Sec = sPaint_time.Sec + 1;
            if (sPaint_time.Sec == 60) {
                sPaint_time.Min = sPaint_time.Min + 1;
                sPaint_time.Sec = 0;
                if (sPaint_time.Min == 60) {
                    sPaint_time.Hour =  sPaint_time.Hour + 1;
                    sPaint_time.Min = 0;
                    if (sPaint_time.Hour == 24) {
                        sPaint_time.Hour = 0;
                        sPaint_time.Min = 0;
                        sPaint_time.Sec = 0;
                    }
                }
            }
            Paint_ClearWindows(150, 80, 150 + Font20.Width * 7, 80 + Font20.Height, WHITE);
            Paint_DrawTime(150, 80, &sPaint_time, &Font20, BLACK, WHITE);

            num = num - 1;
            if(num == 0) {
                break;
            }
            EPD_2IN9_V2_Display_Partial(BlackImage);
            DEV_Delay_ms(500);//Analog clock 1s
        }
#endif

        printf("Clear...\r\n");
        /* EPD_2IN9_V2_Init(); */
        /* EPD_2IN9_V2_Clear(); */

        printf("Goto Sleep...\r\n");
        EPD_2IN9_V2_Sleep();
        /* free(BlackImage); */
        /* BlackImage = NULL; */
        DEV_Delay_ms(2000);//important, at least 2s
        // close 5V
        /* printf("close 5V, Module enters 0 power consumption ...\r\n"); */
        /* DEV_Module_Exit(); */
        return 0;
    }
}

int init_font() {
    display_message("initting font");
    unsigned char* ttf_buffer = (unsigned char*)malloc(1<<25);

    stbtt_fontinfo font;
    unsigned char *bitmap;
    int w,h,i,j,c = 'a', s = 50;

    FILE* font_file = fopen("./Sono-Medium.ttf", "rb");
    if (!font_file) {
        fprintf(stderr, "could not open ttf file");
        return 1;
    }

    fread(ttf_buffer, 1, 1<<25, font_file);
    fclose(font_file);

    stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));

    auto test = [&](int codepoint) {
        bitmap = stbtt_GetCodepointBitmap(&font, 0,stbtt_ScaleForPixelHeight(&font, s), codepoint, &w, &h, 0,0);

        for (j=0; j < h; ++j) {
            for (i=0; i < w; ++i)
                putchar(" .:ioVM@"[bitmap[j*w+i]>>5]);
            putchar('\n');
        }

        printf("width: %d, height: %d\n", w, h);
    };

    test('a');
    test('W');
    test('@');
    test('/');
    test(0xe4);// ü

    return 0;
}

int init_display () {
    printf("EPD_2IN9_V2_test Demo\r\n");
    //Create a new image cache

    UWORD Imagesize = ((EPD_2IN9_V2_WIDTH % 8 == 0)? (EPD_2IN9_V2_WIDTH / 8 ): (EPD_2IN9_V2_WIDTH / 8 + 1)) * EPD_2IN9_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
    Paint_Clear(WHITE);

    init_font();

    return 0;
}

void deinit_display() {
    /* printf("Clear...\r\n"); */
    /* EPD_2IN9_V2_Init(); */
    /* EPD_2IN9_V2_Clear(); */

    free(BlackImage);
    BlackImage = NULL;
}

/* void display_utf8_string(UWORD x, UWORD y, const char* string, UTF8_Font* font, UWORD foreground, UWORD background) { */

/* } */


void display_sleep() {
    printf("Goto Sleep...\r\n");
    EPD_2IN9_V2_Sleep();
    DEV_Delay_ms(2000);//important, at least 2s
    // close 5V
    printf("close 5V, Module enters 0 power consumption ...\r\n");
    DEV_Module_Exit();
}

int display_wake_up() {
    if(DEV_Module_Init()!=0){
        return -1;
    }

    printf("e-Paper Init and Clear...\r\n");
    EPD_2IN9_V2_Init();

    return 0;
}

void display_message(const char* message) {
    display_wake_up();
    {
        Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
        Paint_SelectImage(BlackImage);
        Paint_Clear(WHITE);

        Paint_DrawString_EN(5, 50, message, &Font12, WHITE, BLACK);
        // display_utf8_string(5, 50, message, &UTF8_Font12, WHITE, BLACK);

        EPD_2IN9_V2_Display_Base(BlackImage);
    }
    display_sleep();
}

void draw_signed_number(int x, int y, int number, sFONT* font, UWORD Color_Background, UWORD Color_Foreground, bool draw_plus) {
    if(number < 0) {
        Paint_DrawString_EN(x,   y, "-",     font, Color_Background, Color_Foreground);
        Paint_DrawNum(      x+7, y, -number, font, Color_Foreground, Color_Background);
    } else if (number > 0) {
        if (draw_plus) {
            Paint_DrawString_EN(x,   y, "+",    font, Color_Background, Color_Foreground);
            Paint_DrawNum(      x+7, y, number, font, Color_Foreground, Color_Background);
        } else {
            Paint_DrawNum(      x,   y, number, font, Color_Foreground, Color_Background);
        }
    } else {
        Paint_DrawString_EN(x, y, "0", font, Color_Background, Color_Foreground);
    }
}


void display_timetable(Simple_Timetable timetable, Language lang, Font_Size gs) {
    display_wake_up();
    {

        Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
        printf("Drawing\r\n");
        //1.Select Image
        Paint_SelectImage(BlackImage);
        Paint_Clear(WHITE);

        printf("%d\n", EPD_2IN9_V2_HEIGHT);
        /* Paint_DrawRectangle(1, 1, EPD_2IN9_V2_HEIGHT, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY); */

        uint16_t y_base_line = 0;
        uint16_t y_base_line_increment = 18;
        uint16_t x_base_lines[] = {
            [LINE]        = 5,
            [DEST]        = 60,
            [TRACK]       = 200,
            [IN_MIN]      = 250,
            [PLUS_MINUS]  = 275,
            [DELAY]       = 282
        };

        Paint_DrawString_EN(x_base_lines[LINE],   y_base_line, headings[lang][LINE],   &Font12, WHITE, BLACK);
        Paint_DrawString_EN(x_base_lines[DEST],   y_base_line, headings[lang][DEST],   &Font12, WHITE, BLACK);
        Paint_DrawString_EN(x_base_lines[TRACK],  y_base_line, headings[lang][TRACK],  &Font12, WHITE, BLACK);
        Paint_DrawString_EN(x_base_lines[IN_MIN], y_base_line, headings[lang][IN_MIN], &Font12, WHITE, BLACK);
        y_base_line += y_base_line_increment;

        Paint_DrawLine(1, y_base_line, EPD_2IN9_V2_HEIGHT, 18, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
        y_base_line += 4;

        for(unsigned i = 0; i < timetable.num_entries && i < 6; i++) {
            Paint_DrawString_EN(x_base_lines[LINE],   y_base_line, timetable.entries[i].line,         &Font12, WHITE, BLACK);
            Paint_DrawString_EN(x_base_lines[DEST],   y_base_line, timetable.entries[i].destination,  &Font12, WHITE, BLACK);
            Paint_DrawString_EN(x_base_lines[TRACK],  y_base_line, timetable.entries[i].track,        &Font12, WHITE, BLACK);

            draw_signed_number(x_base_lines[IN_MIN],  y_base_line, timetable.entries[i].planned_time, &Font12, WHITE, BLACK, false);
            if(timetable.entries[i].time_delta != 0)
                draw_signed_number(x_base_lines[PLUS_MINUS],  y_base_line, timetable.entries[i].time_delta, &Font12, WHITE, BLACK, true);

            /* if(timetable.entries[i].planned_time < 0) { */
            /*     Paint_DrawString_EN(x_base_lines[IN_MIN],   y_base_line, "-", &Font12, WHITE, BLACK); */
            /*     Paint_DrawNum(      x_base_lines[IN_MIN]+7, y_base_line, -timetable.entries[i].planned_time, &Font12, BLACK, WHITE); */
            /* } else { */
            /*     Paint_DrawNum(      x_base_lines[IN_MIN],   y_base_line, timetable.entries[i].planned_time, &Font12, BLACK, WHITE); */
            /* } */

            /* if(timetable.entries[i].time_delta != 0) { */
            /*     Paint_DrawString_EN(x_base_lines[PLUS_MINUS], y_base_line, timetable.entries[i].time_delta > 0 ? "+" : "-", &Font12, WHITE, BLACK); */
            /*     Paint_DrawNum(      x_base_lines[DELAY],      y_base_line, abs(timetable.entries[i].time_delta),            &Font12, BLACK, WHITE); */
            /* } */
            y_base_line += y_base_line_increment;
        }


        EPD_2IN9_V2_Display_Base(BlackImage);
     }
    display_sleep();
}

/* int main(void) { */
/*     signal(SIGINT, Handler); */

/*     init_display(); */

/*     display_timetable((Simple_Timetable){ */
/*             { */
/*                 (Simple_Timetable_Entry){ */
/*                     .line = "S2", */
/*                     .destination = "Muenchen Hbf", */
/*                     .track = 3, */
/*                     .planned_time = 12, */
/*                     .time_delta = 5 */
/*                 }, */
/*                 (Simple_Timetable_Entry){ */
/*                     .line = "RE 1", */
/*                     .destination = "Nuernberg Hbf", */
/*                     .track = 4, */
/*                     .planned_time = 13, */
/*                     .time_delta = -1 */
/*                 }, */
/*                 (Simple_Timetable_Entry){ */
/*                     .line = "RB 16", */
/*                     .destination = "Dachau", */
/*                     .track = 5, */
/*                     .planned_time = 14, */
/*                     .time_delta = 0 */
/*                 }, */
/*                 (Simple_Timetable_Entry){ */
/*                     .line = "RB 16", */
/*                     .destination = "Dachau", */
/*                     .track = 5, */
/*                     .planned_time = 14, */
/*                     .time_delta = 0 */
/*                 }, */
/*                 (Simple_Timetable_Entry){ */
/*                     .line = "RB 16", */
/*                     .destination = "Dachau", */
/*                     .track = 5, */
/*                     .planned_time = 14, */
/*                     .time_delta = 0 */
/*                 }, */
/*                 (Simple_Timetable_Entry){ */
/*                     .line = "RB 16", */
/*                     .destination = "Dachau", */
/*                     .track = 5, */
/*                     .planned_time = 14, */
/*                     .time_delta = 0 */
/*                 }, */
/*             } */
/*         }, ENGLISH); */
/*     deinit_display(); */

/*     return 0; */
/* } */
