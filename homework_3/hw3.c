/* 
	Compiles with command line  gcc -o assign3 hw3.c -lX11 -lm -L/usr/X11R6/lib
	Note: Make sure your input file does not have an empty line, or else error will occur. 
	Run : ./assign3 test_in
	Homework #3
	Wan Kim Mok
*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Display *display;
Screen *screen;
unsigned int display_width, display_height;
Window win;
unsigned int win_width, win_height;
char *display_name = NULL;
int screen_name;
int screen_num;
int border_width;
int win_x, win_y;

XWMHints *wm_hints;
XClassHint *class_hints;
XSizeHints *size_hints;
XTextProperty win_name, icon_name;
char *win_name_string = "Homework 2";
char *icon_name_string = "Icon for homework 2 window";
unsigned long valuemask = 0;

XEvent report;

GC gc, green_gc, red_gc, black_gc, light_purple_gc, white_gc;
XColor green_col, red_col, black_col, light_purple_col, white_col;
Colormap colormap;

char white[] = "#FFFFFF";
char green[] = "#00FF00";
char red[] = "#A80000";
char black[] = "#000000";
char light_purple[] = "#FFCCFF";


int main(int argc, char *argv[]){
	FILE *fp;
	char c;
	int temp_line_count = 1;
	int line_count = 0; //holds the actual lines that aren't empty

	fp = fopen(argv[1], "r");

	int previous_val = 0;
	if (fp == NULL){
		//If there's no file, print message and exit.
		printf("There's no file.\n");
		exit(0);
	}
	else{
		if( (display = XOpenDisplay(display_name)) == NULL ){ 
			printf("Could not open display. \n"); 
			exit(-1);
		}
		printf("Connected to X server  %s\n", XDisplayName(display_name) );
		
		screen_num = DefaultScreen(display);
		screen = DefaultScreenOfDisplay(display);
		colormap = XDefaultColormap(display, screen_num);
		display_width = DisplayWidth(display, screen_num);
		display_height = DisplayHeight(display, screen_num);

		while(!feof(fp)){
			c = fgetc(fp);

			if (c == '\n'){
				temp_line_count++;
			}
		}
		rewind(fp);
	}


	//Creating window
	border_width = 10;
	win_x = 0; win_y = 0;
	win_width = display_width/2;
	win_height = display_height * 0.8;
	//win_height = (int)(win_width/1.7); //rectangular window

	printf("window width: %d\n window height: %d\n", display_width, display_height);

	//win = XCreateSimpleWindow(display, RootWindow(display, 0), 1, 1, win_width, win_height, 10, WhitePixel (display, 0), WhitePixel (display, 0));
	win = XCreateSimpleWindow(display, RootWindow(display, screen_num), 
		win_x, win_y, win_width, win_height, border_width,
		BlackPixel(display, screen_num), WhitePixel(display, screen_num));
	

	//Maps window on screen
	size_hints = XAllocSizeHints();
	wm_hints = XAllocWMHints();
	class_hints = XAllocClassHint();
	if (size_hints == NULL || wm_hints == NULL || class_hints == NULL){
		printf("Error allocating memory for hints\n");
		exit(-1);
	}

	size_hints -> flags = PPosition | PSize | PMinSize;
	size_hints -> min_width = 60;
	size_hints -> min_height = 60;

	XStringListToTextProperty(&win_name_string, 1, &win_name);
	XStringListToTextProperty(&icon_name_string, 1, &icon_name);

	wm_hints -> flags = StateHint | InputHint;
	wm_hints -> initial_state = NormalState;
	wm_hints -> input = False;

	class_hints -> res_name = "x_use_example";
	class_hints -> res_class = "homework2";
	
	XSetWMProperties(display, win, &win_name, &icon_name, argv, argc, size_hints, wm_hints, class_hints );
	XSelectInput(display, win, ExposureMask | KeyPressMask | ButtonPressMask);

	// put on screen
	XMapWindow(display, win);
	XFlush(display);

	//graphics setup
	green_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, green, &green_col);
	if (XAllocColor(display, colormap, &green_col) == 0){
		printf("Failed to get color green\n");
		exit(-1);
	}
	else{
		printf("Success green!\n");
		XSetForeground(display, green_gc, green_col.pixel);

	}

	red_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, red, &red_col);
	if (XAllocColor(display, colormap, &red_col) == 0){
		printf("Failed to get color red\n");
		exit(-1);
	}
	else{
		printf("Success red!\n");
		XSetForeground(display, red_gc, red_col.pixel);
	}


	black_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, black, &black_col);
	if (XAllocColor(display, colormap, &black_col) == 0){
		printf("Failed to get color black\n");
		exit(-1);
	}
	else{
		printf("Success black!\n");
		XSetForeground(display, black_gc, black_col.pixel);
	}


	light_purple_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, light_purple, &light_purple_col);
	if (XAllocColor(display, colormap, &light_purple_col) == 0){
		printf("Failed to get color light purple\n");
		exit(-1);
	}
	else{
		printf("Success light purple!\n");
		XSetForeground(display, light_purple_gc, light_purple_col.pixel);
	}


	white_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, white, &white_col);	
	if (XAllocColor(display, colormap, &white_col) == 0){
		printf("Failed to get color white\n");
		exit(-1);
	}
	else{
		printf("Success white!\n");
		XSetForeground(display, white_gc, white_col.pixel);	
	}






	printf("total raw line count: %d\n", temp_line_count);
	int i;
	//stores variable v or h
	char temp[temp_line_count];
	//stores the input in a 2d array
	int m[temp_line_count][4];

	for (i=0; i <temp_line_count; i++){
		fscanf(fp, "%s %*d, %*d, %*d", &temp[i]);
	}
	rewind(fp);

	for (i=0; i< temp_line_count; i++){
		if (temp[i] == 104){
			//horizal line segment
			fscanf(fp, "%*s %d, %d, %d", &m[i][1], &m[i][0], &m[i][2]);
			m[i][3] = m[i][1];
			line_count++;
		}
		else if (temp[i] == 118){
			//vertical line segment
			fscanf(fp, "%*s %d, %d, %d", &m[i][0], &m[i][1], &m[i][3]);
			m[i][2] = m[i][0];
			line_count++;
		}
	}
	rewind(fp);

	printf("total processed line count: %d\n", line_count);

	for (i=0; i<line_count; i++){
		printf("(%d, %d) to (%d , %d)\n", m[i][0], m[i][1], m[i][2], m[i][3]);
	}



while(1){
		XNextEvent(display, &report);
		switch(report.type){
			case Expose:
			{	
				for (i = 0; i <line_count; i++){
					//Draw the line segments
					XDrawLine(display, win, green_gc, m[i][0], m[i][1], m[i][2], m[i][3]);
				}

				XFlush(display);
				break;
			}
			default:
				break;
		}
	}
	fclose(fp);
	return 0;
}