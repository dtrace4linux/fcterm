#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
        Display *dpy;
        Window win;
        Colormap cmap;
        XEvent e;
        XftFont *xfthuge;
        XftColor color;
        XftDraw *draw;
        XRenderColor kindofblue = { 0x4c00, 0x7800, 0x9900, 0xffff};
        char *msg = "Hello, World!";
        int scr;

        /* open connection with the server */
        dpy = XOpenDisplay(NULL);
        if (dpy == NULL) {
                fprintf(stderr, "Cannot open display\n");
                exit(1);
        }

        scr = DefaultScreen(dpy);
        cmap = DefaultColormap(dpy, scr);

        /* create window */
        win = XCreateSimpleWindow(dpy, RootWindow(dpy, scr), 100, 100, 700,
                300, 1, BlackPixel(dpy, scr), WhitePixel(dpy, scr));

        /* select kind of events we are interested in */
        XSelectInput(dpy, win, ExposureMask | KeyPressMask);

        /* map (show) the window */
        XMapWindow(dpy, win);

        xfthuge = XftFontOpen(dpy, scr,
                XFT_FAMILY, XftTypeString, "dejavuserifcondensed",
                XFT_SIZE, XftTypeDouble, 100.0,
                NULL);

        XftColorAllocValue(dpy, DefaultVisual(dpy, scr), cmap,
                &kindofblue, &color);

        draw = XftDrawCreate(dpy, win, DefaultVisual(dpy, scr), cmap);

        /* event loop */
        while (1) {
                XNextEvent(dpy, &e);

                /* draw or redraw the window */
                if (e.type == Expose)
                        XftDrawStringUtf8(draw, &color, xfthuge, 10, 100,
                                (FcChar8*)msg, strlen(msg));

                /* exit on key press */
                if (e.type == KeyPress)
                        break;
        }

        XftDrawDestroy(draw);

        /* close connection to server */
        XCloseDisplay(dpy);

        return 0;

}

