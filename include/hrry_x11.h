#ifndef HRRY_X11_H
#define HRRY_X11_H
#include "hrry_stdinc.h"
#include <X11/Xlib.h> //linx clipboard exposure

// WIP

void show_utf8_prop(Display* dpy, Window w, Atom p);

unsigned char* get_utf8_prop(Display* dpy, Window w, Atom p);

int get();

unsigned char* getX11Clipboard();


#endif
