/* 
	Compiles with command line  gcc -o test hw1.c -lX11 -lm -L/usr/X11R6/lib 
	Run : ./test input.txt
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
GC green_gc, red_gc;
XEvent report;
XColor green_col, red_col;
Colormap colormap;

char green[] = "#00FF00";
char red[] = "#A80000";


void Draw(char *argv[]){
	FILE *fp;
	char buff[1000];
	int ch, i;
	int line_count = 0;
	
	fp = fopen(argv[1], "r");

	if (fp == NULL){
		printf("There's no input file.\n");
		exit(0);
	}
	else{
		//printf("%s\n", argv[1]);
		while(1){
			ch = fgetc(fp);
			if(feof(fp)){
				break;
			}
			if (ch == '\n'){
				line_count++;
			}
			//printf("%c\n", ch);
		}

		rewind(fp);

		printf("Total lines:  %d\n", line_count+1);

		/* go through characters per line */
		int m[line_count][2];

		//XDrawLine(display, win, green_gc, 2, 2, 4,2);
		
		for(i=0;i<=line_count;i++){
			//store formatted input file in array m
			fscanf(fp, "%d,%d", &m[i][0], &m[i][1]);
			//printf("%d %d\n", m[i][0], m[i][1]);

			//Magnify points by 2
			//m[i][0] = (m[i][0]*2);
			//m[i][1] = (m[i][1]*2);

			printf("Scaled coordinates: %d, %d\n", m[i][0], m[i][1]);
		}

		for(i=0;i<line_count;i++){
			/* draw lines */
			XDrawLine(display, win, green_gc, m[i][0], m[i][1], m[i+1][0], m[i+1][1]);
		}
		//XDrawLine(display, win, green_gc, m[i][0], m[i][1], m[0][0], m[0][1]);
		rewind(fp);
		
		printf("Close file.");
		fclose(fp);
	}
}

int main(int argc, char *argv[]){
	display = XOpenDisplay(NULL);

	//Creating window
	win = XCreateSimpleWindow(display, RootWindow(display, 0), 1, 1, 500, 500, 10, WhitePixel (display, 0), WhitePixel (display, 0));
	
	//Maps window on screen
	XMapWindow(display, win);

	colormap = DefaultColormap(display, 0);
	green_gc = XCreateGC(display, win, 0, 0);
	red_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, green, &green_col);
	XParseColor(display, colormap, red, &red_col);
	XAllocColor(display, colormap, &green_col);
	XAllocColor(display, colormap, &red_col);
	XSetForeground(display, green_gc, green_col.pixel);
	XSetForeground(display, red_gc, red_col.pixel);

	XSelectInput(display, win, ExposureMask | KeyPressMask | ButtonPressMask);

	//XDrawRectangle(display, win, green_gc, 10, 10, 398, 398);
	//Draw(argv);


	XFlush(display);
	
	while(1){
		XNextEvent(display, &report);
		switch(report.type){
			case Expose:
				//printf("Exposed.");
				//XDrawRectangle(display, win, green_gc, 50, 50, 398, 398);
				//XDrawLine(display, win, green_gc, 10, 10, 400, 400);
				//XDrawLine(display, win, green_gc, 1, 1, 400, 400);
				Draw(argv);
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
				if (report.xbutton.button == Button1){
					/* left click */
					XDrawPoint(display, win, green_gc, report.xbutton.x, report.xbutton.y);
				}
				else{
					XDrawPoint(display, win, red_gc, report.xbutton.x, report.xbutton.y);
				}
				break;
			default:
				break;
		}
	}
	return 0;
}

