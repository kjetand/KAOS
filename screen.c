#include "common.h"
#include "screen.h"
#include "string.h"

static void update_cursor(screen_t *screen);
static int8_t translate_x(screen_t *screen, int8_t x);
static int8_t translate_y(screen_t *screen, int8_t y);
static int8_t get_color(screen_t *screen);
static int16_t get_video_memory_index(int8_t x, int8_t y);

void screen_init(screen_t *screen)
{
    screen_set_position(screen, 0, 0);
    screen_set_dimensions(screen, SCREEN_WIDTH, SCREEN_HEIGHT);
    screen_set_cursor(screen, 0, 0);
    screen_set_foreground(screen, COLOR_WHITE);
    screen_set_background(screen, COLOR_BLACK);
}

void screen_set_position(screen_t *screen, int8_t x, int8_t y)
{
    screen->x = x;
    screen->y = y;
}

void screen_set_dimensions(screen_t *screen, int8_t width, int8_t height)
{
    screen->width = width;
    screen->height = height;
}

void screen_set_background(screen_t *screen, vga_color_t color)
{
    screen->background = color;
}

void screen_set_foreground(screen_t *screen, vga_color_t color)
{
    screen->foreground = color;
}

void screen_set_cursor(screen_t *screen, int8_t x, int8_t y)
{
    screen->cursor_x = x;
    screen->cursor_y = y;
    update_cursor(screen);
}

void screen_clear_line(screen_t *screen, int8_t line_number)
{
    int8_t *vidmem = VIDEO_MEMORY_ADDR;
    int8_t  actual_y = translate_y(screen, line_number);
    
    int16_t start = get_video_memory_index(screen->x, actual_y);
    int16_t end = start + (screen->width - 1) * 2;

    int16_t i;

    for (i = start; i <= end; i += 2)
    {
        vidmem[i] = ASCII_NUL;
        vidmem[i+1] = get_color(screen);
    }
}

void screen_clear(screen_t *screen)
{
    int8_t i;

    for (i = 0; i < screen->height; i++)
    {
        screen_clear_line(screen, i);
    }
    screen_set_cursor(screen, 0, 0);
}

void screen_scroll(screen_t *screen)
{
    int8_t *vidmem = VIDEO_MEMORY_ADDR;
    int8_t  prev_y, curr_y, row;
    int16_t prev_pos, curr_pos;

    for (row = 1; row < screen->height; row++)
    {
        prev_y = translate_y(screen, row - 1);
        curr_y = translate_y(screen, row);
        prev_pos = get_video_memory_index(screen->x, prev_y);
        curr_pos = get_video_memory_index(screen->x, curr_y);

        memcpy(&vidmem[prev_pos], &vidmem[curr_pos], screen->width * 2);
    }
    screen_clear_line(screen, screen->height - 1);
}

void screen_putchar(screen_t *screen, char c)
{
    int8_t *vidmem = VIDEO_MEMORY_ADDR;
    int8_t  x = translate_x(screen, screen->cursor_x);
    int8_t  y = translate_y(screen, screen->cursor_y);
    int16_t pos = get_video_memory_index(x, y);

    switch (c)
    {
    case ASCII_LF:
        screen->cursor_x = 0;
        screen->cursor_y++;
        break;
    default:
        vidmem[pos]   = c;
        vidmem[pos+1] = get_color(screen);
        screen->cursor_x++;
    }

    if (screen->cursor_x >= screen->width)
    {
        screen->cursor_x = 0;
        screen->cursor_y++;
    }

    if (screen->cursor_y >= screen->height)
    {
        screen_scroll(screen);
        screen->cursor_x = 0;
        screen->cursor_y = screen->height - 1;
        screen_clear_line(screen, screen->height - 1);
    }
    screen_set_cursor(screen, screen->cursor_x, screen->cursor_y);
}

void screen_putstr(screen_t *screen, char* str)
{
    char *currentChar = str;

    while (*currentChar != ASCII_NUL)
    {
        screen_putchar(screen, *currentChar);
        currentChar++;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Helper functions
///////////////////////////////////////////////////////////////////////////////

static void update_cursor(screen_t *screen)
{
    int8_t  x = translate_x(screen, screen->cursor_x);
    int8_t  y = translate_y(screen, screen->cursor_y);
    int16_t pos = x + (y * SCREEN_WIDTH);

    outb(VGA_CRT_INDEX, VGA_CRT_CURSOR_LOW);
    outb(VGA_CRT_DATA, pos & 0xFF);
    outb(VGA_CRT_INDEX, VGA_CRT_CURSOR_HIGH);
    outb(VGA_CRT_DATA, (pos >> 8) & 0xFF);
}

static int8_t translate_x(screen_t *screen, int8_t x)
{
    return screen->x + x;
}

static int8_t translate_y(screen_t *screen, int8_t y)
{
    return screen->y + y;
}

static int8_t get_color(screen_t *screen)
{
    int8_t foreground = (int8_t) screen->foreground;
    int8_t background = (int8_t) screen->background;
    return (foreground & 0x0F) | (background << 4);
}

static int16_t get_video_memory_index(int8_t x, int8_t y)
{
    return 2 * ((y * SCREEN_WIDTH) + x);
}
