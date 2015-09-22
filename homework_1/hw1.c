/* 
	Compiles with command line  gcc -o test hw1.c -lX11 -lm -L/usr/X11R6/lib 
	Homework #1
	Wan Kim Mok
	Due: September 30, 2015
*/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Display *display;
Window win;
GC green_gc;
XEvent report;
XColor green_col;
Colormap colormap;

char green[] = "#00FF00";

int main(int argc, char *argv[]){
	display = XOpenDisplay(NULL);

	//Creating window
	win = XCreateSimpleWindow(display, RootWindow(display, 0), 1, 1, 500, 500, 10, BlackPixel (display, 0), BlackPixel (display, 0));
	
	//Maps window on screen
	XMapWindow(display, win);

	colormap = DefaultColormap(display, 0);
	green_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, green, &green_col);
	XAllocColor(display, colormap, &green_col);
	XSetForeground(display, green_gc, green_col.pixel);

	XSelectInput(display, win, ExposureMask | KeyPressMask | ButtonPressMask);

	//XDrawRectangle(display, win, green_gc, 10, 10, 398, 398);


	XFlush(display);
	
	while(1){
		XNextEvent(display, &report);
		switch(report.type){
			case Expose:
				//printf("Exposed.");
				XDrawRectangle(display, win, green_gc, 50, 50, 398, 398);
				XDrawLine(display, win, green_gc, 10, 10, 400, 400);
				XFlush(display);
				break;
			case KeyPress:
				//when q is presss program is closed.
				if ((XLookupKeysym(&report.xkey, 0) == XK_q)){
					exit(0);
				}
				break;
			case ButtonPress:
				printf("Button press %d, %d.\n",report.xbutton.x, report.xbutton.y);
				XDrawPoint(display, win, green_gc, report.xbutton.x, report.xbutton.y);
				break;
		}
	}
	return 0;
}

