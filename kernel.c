#include "common.h"
#include "screen.h"
#include "string.h"

void kernel_main()
{
    screen_t screen;

    screen_init(&screen);
    screen_clear(&screen);
    screen_putstr(&screen, "Welcome to KAOS!");

    while (1);
}
