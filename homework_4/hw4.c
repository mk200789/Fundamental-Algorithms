/* 
	Compiles and run with command lines
			gcc -o assign hw4.c -lX11 -lm -L/usr/X11R6/lib
			./assign test_in
	Homework #1
	Wan Kim Mok
	Due: September 30, 2015
	valgrind --leak-check=full ./assign1 test_in
*/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef enum { false, true } bool;
#define MAX_VERTICES 200

typedef struct{
	int x;
	int y;
}Point;

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
char *win_name_string = "Homework 4";
char *icon_name_string = "Icon for homework 4 window";

XEvent report;

GC gc, green_gc, red_gc, black_gc, blue_gc;
XColor green_col, red_col, black_col, blue_col;
Colormap colormap;

Point point[MAX_VERTICES];
Point line_segment[MAX_VERTICES][MAX_VERTICES][2];
int DEST = 1;
int ORIG = 0;
int num_line_segment;
int line_count;

char green[] = "#1F8728";
char red[] = "#FF3333";
char black[] = "#000000";
char blue[] = "#0000FF";


int find_distance(Point a, Point b){
	/*
		Returns the distance between two points using Euclidean.
	*/
	return (int) sqrt((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y));
}

void TSP(){
	return;
}

void get_segment(){
	printf("get_segment()\n");
	int i, j;
	for(i=0; i<line_count; i++){
		for (j=0; j<line_count; j++){
			line_segment[i][j][ORIG] = point[i];
			line_segment[i][j][DEST] = point[j];
			printf("%d %d\n", line_segment[i][j][ORIG].x, line_segment[i][j][ORIG].y);
		}
	}
	return;
}

void HeldKarp(){
	//Use Held Karp algorithm for points less than 20
	return;
}

int main(int argc, char *argv[]){

	FILE *fp;
	fp = fopen(argv[1], "r");
	int i, x, y;

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
	class_hints -> res_class = "homework3";
	
	XSetWMProperties(display, win, &win_name, &icon_name, argv, argc, size_hints, wm_hints, class_hints );
	XSelectInput(display, win, ExposureMask | KeyPressMask | ButtonPressMask);

	// put on screen
	XMapWindow(display, win);
	XFlush(display);

	//graphics setup
	green_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, green, &green_col);
	XSetLineAttributes(display, green_gc, 4, LineSolid, CapRound, JoinRound);
	if (XAllocColor(display, colormap, &green_col) == 0){
		printf("Failed to get color green\n");
		exit(-1);
	}
	else{
		XSetForeground(display, green_gc, green_col.pixel);

	}

	red_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, red, &red_col);
	//XSetLineAttributes(display, red_gc, 2, LineSolid, CapRound, JoinRound);
	if (XAllocColor(display, colormap, &red_col) == 0){
		printf("Failed to get color red\n");
		exit(-1);
	}
	else{
		XSetForeground(display, red_gc, red_col.pixel);
	}


	black_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, black, &black_col);
	if (XAllocColor(display, colormap, &black_col) == 0){
		printf("Failed to get color black\n");
		exit(-1);
	}
	else{
		XSetForeground(display, black_gc, black_col.pixel);
	}

	blue_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, blue, &blue_col);	
	XSetLineAttributes(display, blue_gc, 4, LineSolid, CapRound, JoinRound);
	if (XAllocColor(display, colormap, &blue_col) == 0){
		printf("Failed to get color blue\n");
		exit(-1);
	}
	else{
		XSetForeground(display, blue_gc, blue_col.pixel);	
	}

	line_count = 0;
	bool hasInput = false;
	bool complete = false;

	if (fp == NULL){
		//enter points by mouse click
		printf("There's no file. Left click graph to enter a point and right click to end input phase.\n");
	}
	else{
		//using input file
		printf("Reading file input coordinates\n");
		fscanf(fp, "%d %d", &x, &y);
		while(!feof(fp)){
			point[line_count].x = x;
			point[line_count++].y = y;
			fscanf(fp, "%d %d", &x, &y);
		}
		complete = hasInput = true;
		rewind(fp);

		printf("Total points: %d\n", line_count);

		if (line_count < 20){
			//If the number of points is less than 20, you use the Held-Karp algorithm 
			//to compute the optimum TSP tour.
			get_segment();
		}
		else{
			//If the number is greater than 20, you sort the points according to the First
			//coordinate, divide the set into groups of at most twenty, and solve each group 
			//optimally; after that, you find the optimal connection between the groups.
		}

	}


	while(1){
		XNextEvent(display, &report);
		switch(report.type){
			case Expose:
			{	
				if (line_count){
					for (i=0; i<line_count;i++){
						XFillArc( display, win, black_gc, point[i].x, point[i].y, win_width/200, win_width/200, 0, 360*64);
					}

					int j;
					for(i=0; i<line_count; i++){
						for(j=0; j<line_count; j++){
							XDrawLine(display, win, red_gc, line_segment[i][j][ORIG].x, line_segment[i][j][ORIG].y, line_segment[i][j][DEST].x, line_segment[i][j][DEST].y);
						}
					}
				}

				XFlush(display);
				break;
			}

			case ButtonPress:
			{

				if (report.xbutton.button == Button1){
					//left click

					if (!hasInput){
						//save input points via left mouseclick
						point[line_count].x = report.xbutton.x;
						point[line_count].y = report.xbutton.y;
						XFillArc( display, win, black_gc, point[line_count].x, point[line_count].y, win_width/200, win_width/200, 0, 360*64);
						printf("Input (%d, %d)\n", point[line_count].x, point[line_count].y);
						line_count++;
					}

				}
				else{
					//right click : 1) end input points 2)close window
					if (!hasInput && !complete){
						printf("End input\n");
						printf("Total points: %d\n", line_count);

						if (line_count < 0){
							//If the number of points is less than 20, you use the Held-Karp algorithm 
							//to compute the optimum TSP tour.
						}
						else{
							//If the number is greater than 20, you sort the points according to the First
							//coordinate, divide the set into groups of at most twenty, and solve each group 
							//optimally; after that, you find the optimal connection between the groups.
						}
						complete = true;
					}
					else{
						// close graphics
						printf("Closing Window.\n");
						XDestroyWindow(display, win);
						XCloseDisplay(display);
						exit(1);
					}
					
				}
				
				break;
			}
			default:
				break;
		}
	}
	fclose(fp);
	return 0;
}