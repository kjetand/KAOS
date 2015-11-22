#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "common.h"

#define VIDEO_MEMORY_ADDR ((int8_t *) 0xB8000)

/* VGA data register ports */
#define VGA_CRT_DATA 0x3D5 // CRT Controller VGA data register

/* VGA index register ports */
#define VGA_CRT_INDEX 0x3D4 // CRT Controller VGA index register

/* VGA CRT controller registers */
#define VGA_CRT_CURSOR_HIGH 0x0E
#define VGA_CRT_CURSOR_LOW 0x0F

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

typedef enum vga_color
{
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15,
} vga_color_t;

typedef struct
{
    int8_t x;
    int8_t y;
    int8_t width;
    int8_t height;
    int8_t cursor_x;
    int8_t cursor_y;
    vga_color_t foreground;
    vga_color_t background;
} screen_t;

void screen_init(screen_t *screen);
void screen_set_position(screen_t* screen, int8_t x, int8_t y);
void screen_set_dimensions(screen_t *screen, int8_t width, int8_t height);
void screen_set_background(screen_t *screen, vga_color_t color);
void screen_set_foreground(screen_t *screen, vga_color_t color);
void screen_set_cursor(screen_t *screen, int8_t x, int8_t y);
void screen_clear_line(screen_t *screen, int8_t line_number);
void screen_clear(screen_t *screen);
void screen_scroll(screen_t *screen);
void screen_putchar(screen_t *screen, char c);
void screen_putstr(screen_t *screen, char* str);

#endif
