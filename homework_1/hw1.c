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
#include <math.h>

Display *display;
Window win;
GC gc;

int main(int argc, char *argv[]){
	display = XOpenDisplay(NULL);

	//Creating window
	win = XCreateSimpleWindow(display, RootWindow(display, 0), 1, 1, 250, 500, \
		0, BlackPixel (display, 0), WhitePixel (display, 0));
	
	//Maps window on screen
	XMapWindow(display, win);
	XFlush(display);
	
	//Sleep for 10 seconds befor closing
	sleep(10);
	return 0;
}

