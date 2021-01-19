#include <stdio.h>
#include <X11/Xlib.h>

int
main(const int argc, const char *argv[])
{

    Display *display;
    Screen *screen;

    // open a display
    display = XOpenDisplay(NULL);

    // return the number of available screens
    int count_screens = ScreenCount(display);

    printf("Total count screens: %d\n", count_screens);


    for (int i = 0; i < count_screens; ++i) {
        screen = ScreenOfDisplay(display, i);
        printf("\tScreen %d: %dX%d\n", i + 1, screen->width, screen->height);
    }

    // close the display
    XCloseDisplay(display);

   return 0;
}


