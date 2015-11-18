/* 
	Compiles with command line  gcc -o assign1 hw1.c -lX11 -lm -L/usr/X11R6/lib 
	Run : ./assign input.txt
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

typedef int bool;
#define true 1
#define false 0

int intersect = false;

Display *display;
Window win;
GC green_gc, red_gc, black_gc;
XEvent report;
XColor green_col, red_col, black_col;
Colormap colormap;

unsigned int win_width, win_height;

//store the x,y values of target and start point
unsigned int start_x, start_y , target_x, target_y;

//count keep track of the left clicks
int count, count_intersect;

long ABS, ABT, STA, STB;
int PAB, PBC, PAC, CAB, BAC, ABC;

char green[] = "#00FF00";
char red[] = "#A80000";
char black[] = "#000000";


int orientation(int ax, int ay, int bx, int by, int cx, int cy)
{
	return (ax*by) + (bx*cy) + (cx*ay) - (ay*bx) - (by*cx) - (cy*ax);
}


int is_inTrangle(int px, int py, int ax, int ay, int bx, int by, int cx, int cy)
{
	int r1, r2, r3, r4, r5, r6;
	r1 = orientation(px, py, ax, ay, bx, by)*orientation(cx, cy, ax, ay, bx, by);
	r2 = orientation(px, py, bx, by, cx, cy)*orientation(ax, ay, bx, by, cx, cy);
	r3 = orientation(px, py, ax, ay, cx, cy)*orientation(bx, by, ax, ay, cx, cy);
	return  r1, r2, r3;
}


double find_distance(int x1, int y1, int x2, int y2)
{
	return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}


int check_if_in_triangle(int line_count, int m[][6], int x, int y){
	int i;
	for (i = 0; i <= line_count; i++){
		//printf("####### check if point in triangle #######\n");
		PAB = orientation(x, y, m[i][0], m[i][1], m[i][2], m[i][3]);
		PBC = orientation(x, y, m[i][2], m[i][3], m[i][4], m[i][5]);
		PAC = orientation(x, y, m[i][0], m[i][1], m[i][4], m[i][5]);
		CAB = orientation(m[i][4], m[i][5], m[i][0], m[i][1], m[i][2], m[i][3]);
		ABC = orientation(m[i][0], m[i][1], m[i][2], m[i][3], m[i][4], m[i][5]);
		BAC = orientation(m[i][2], m[i][3], m[i][0], m[i][1], m[i][4], m[i][5]);
		
		if (PAB*CAB > 0  && PBC*ABC > 0 && PAC*BAC > 0 ){
			//point is in the triangle
			return 1;
		}
	}
	return 0;
}


int check_intersect(int line_count, int vertex[][6], int start_x, int start_y, int target_x, int target_y){

	int i, j;
	int val = 0;

	printf("======================START======================\n");
	printf("target: (%d, %d)\n", target_x, target_y);

	for (i=0; i<=line_count; i++){

		for (j=0; j<6; j+=2){
			//printf("%d\n", j);
			if (j == 4){
				//printf("Ax:%d, Ay:%d, Bx:%d, By:%d\n", vertex[i][j], vertex[i][j+1], vertex[i][0], vertex[i][1]);
				ABS = orientation(vertex[i][j], vertex[i][j+1], vertex[i][0], vertex[i][1], start_x, start_y);
				ABT = orientation(vertex[i][j], vertex[i][j+1], vertex[i][0], vertex[i][1], target_x, target_y);
				STA = orientation(start_x, start_y, target_x, target_y, vertex[i][j], vertex[i][j+1]);
				STB = orientation(start_x, start_y, target_x, target_y, vertex[i][0], vertex[i][1]);
			}
			else{
				//printf("Ax:%d, Ay:%d, Bx:%d, By:%d\n", vertex[i][j], vertex[i][j+1], vertex[i][j+2], vertex[i][j+3]);
				ABS = orientation(vertex[i][j], vertex[i][j+1], vertex[i][j+2], vertex[i][j+3], start_x, start_y);
				ABT = orientation(vertex[i][j], vertex[i][j+1], vertex[i][j+2], vertex[i][j+3], target_x, target_y);
				STA = orientation(start_x, start_y, target_x, target_y, vertex[i][j], vertex[i][j+1]);
				STB = orientation(start_x, start_y, target_x, target_y, vertex[i][j+2], vertex[i][j+3]);
			}

			//printf("ABST*ABT: %lu, STA*STB: %lu\n", ABS*ABT, STA*STB);
			
			if ((ABS*ABT)<0 && (STA*STB)<0){
				
				if (j == 4){
					printf("destination2: %d, %d, line: (%d,%d) (%d,%d)\n", target_x, target_y, vertex[i][j], vertex[i][j+1], vertex[i][0], vertex[i][1]);
					printf("ABST*ABT: %lu, STA*STB: %lu\n", ABS*ABT, STA*STB);
				}
				else{
					printf("destination1: %d, %d, line: (%d,%d) (%d,%d)\n", target_x, target_y, vertex[i][j], vertex[i][j+1], vertex[i][j+2], vertex[i][j+3]);
					printf("ABST*ABT: %lu, STA*STB: %lu\n", ABS*ABT, STA*STB);
				}
				
				val = 1;
				
			}
	
		}
	}

	return val;
	
}


int * start_graph(int line_count, int vertex[][6], int start_x, int start_y, int target_x, int target_y){

	int i, j;
	int is_intersect;

	for (i=0; i<=line_count; i++){

		for (j=0; j<6; j+=2){

			if (check_if_in_triangle(line_count, vertex, vertex[i][j], vertex[i][j+1]) == 0){

				is_intersect = check_intersect(line_count, vertex, start_x, start_y, vertex[i][j], vertex[i][j+1]);
				

				if (is_intersect == 0){

					XDrawLine(display, win, black_gc, start_x, start_y, vertex[i][j], vertex[i][j+1]);
				}

			}
		}
	}

}

int main(int argc, char *argv[]){
	display = XOpenDisplay(NULL);
	win_height = 500;
	win_width = 500;
	count = -1;
	count_intersect = 0;

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
		//obtaining line count
		while(!feof(fp)){
			ch = fgetc(fp);
			if (ch == '\n'){
				line_count++;
			}
		}
		rewind(fp);
	}

	int m[line_count][6];

	//Creating window
	win = XCreateSimpleWindow(display, RootWindow(display, 0), 1, 1, win_width, win_height, 10, WhitePixel (display, 0), WhitePixel (display, 0));
	
	//Maps window on screen
	XMapWindow(display, win);

	colormap = DefaultColormap(display, 0);
	green_gc = XCreateGC(display, win, 0, 0);
	red_gc = XCreateGC(display, win, 0, 0);
	black_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, green, &green_col);
	XParseColor(display, colormap, red, &red_col);
	XParseColor(display, colormap, black, &black_col);
	XAllocColor(display, colormap, &green_col);
	XAllocColor(display, colormap, &red_col);
	XAllocColor(display, colormap, &black_col);
	XSetForeground(display, green_gc, green_col.pixel);
	XSetForeground(display, red_gc, red_col.pixel);
	XSetForeground(display, black_gc, black_col.pixel);

	XSelectInput(display, win, ExposureMask | KeyPressMask | ButtonPressMask);


	XFlush(display);
	
	while(1){
		XNextEvent(display, &report);
		switch(report.type){
			case Expose:
			{	
				for (i = 0; i <= line_count; i++){
					fscanf(fp, "%*s ( %d, %d) ( %d, %d) (%d, %d)", &m[i][0], &m[i][1], &m[i][2], &m[i][3], &m[i][4], &m[i][5]);
				}

				//for (i = 0; i <= line_count; i++){
				for (i = 0; i <= line_count; i++){
					//Draw the triangles
					XDrawLine(display, win, green_gc, m[i][0], m[i][1], m[i][2], m[i][3]);
					XDrawLine(display, win, green_gc, m[i][2], m[i][3], m[i][4], m[i][5]);
					XDrawLine(display, win, green_gc, m[i][4], m[i][5], m[i][0], m[i][1]);
				}

				rewind(fp);
				printf("exposed\n");
				XFlush(display);
				break;
			}
			case ButtonPress:
				printf("Button press %d, %d.\n",report.xbutton.x, report.xbutton.y);
				int x, y;
				double distance1;
				x = report.xbutton.x;
				y = report.xbutton.y;
				int inTriangle;

				if (report.xbutton.button == Button1){
					/* left click */
					count++;
					if (count > 1){
						count = 0;
					}
					inTriangle = check_if_in_triangle(line_count, m, x, y);

					if (inTriangle == 0){
						XFillArc( display, win, black_gc, x, y, win_width/200, win_width/200, 0, 360*64);
					}
					

				}
				else{
					printf("Closing Window.\n");
					XDestroyWindow(display, win);
					XCloseDisplay(display);
					exit(1);
				}


				printf("count: %d\n", count);
				if (count == 0){
					start_x = x;
					start_y = y;
				}
				else if (count == 1){
					target_x = x;
					target_y = y;

					printf("Count less than 2\n");
					printf("point x: %d, point y: %d\n", start_x, start_y);

					int vertex[line_count][6];
					int * nearest_triangles;
					int * result_line_seg;

					for(i=0;i<=line_count;i++){
						//store formatted input file in array m
						fscanf(fp, "%*s ( %d, %d) ( %d, %d) (%d, %d)", &vertex[i][0], &vertex[i][1], &vertex[i][2], &vertex[i][3], &vertex[i][4], &vertex[i][5]);					
					}
					rewind(fp);

					start_graph(line_count, vertex, start_x, start_y, target_x, target_y);

					count = -1;
				}

				if (inTriangle != 0){
					count = -1;
				}
				
				//Draw(argv);
				XFlush(display);
				break;
			default:
				break;
		}
	}
	fclose(fp);
	return 0;
}