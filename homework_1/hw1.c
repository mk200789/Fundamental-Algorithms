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

int ABS, ABT, STA, STB;
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
		printf("%d, %d, %d, %d, %d, %d\n", m[i][0], m[i][1], m[i][2], m[i][3], m[i][4], m[i][5]);
		
		printf("####### check if point in triangle #######\n");
		PAB = orientation(x, y, m[i][0], m[i][1], m[i][2], m[i][3]);
		PBC = orientation(x, y, m[i][2], m[i][3], m[i][4], m[i][5]);
		PAC = orientation(x, y, m[i][0], m[i][1], m[i][4], m[i][5]);
		CAB = orientation(m[i][4], m[i][5], m[i][0], m[i][1], m[i][2], m[i][3]);
		ABC = orientation(m[i][0], m[i][1], m[i][2], m[i][3], m[i][4], m[i][5]);
		BAC = orientation(m[i][2], m[i][3], m[i][0], m[i][1], m[i][4], m[i][5]);
		
		if (PAB*CAB > 0  && PBC*ABC > 0 && PAC*BAC > 0 ){
			printf("POINT IS IN THE TRIANGLE.\n");
			printf("pab * cab = %d\n", PAB*CAB);
			printf("pbc * abc = %d\n", PBC*ABC);
			printf("pac * bac = %d\n", PAC*BAC);
			return 1;
		}
	}
	return 0;
}

int * nearest_line_seg(int line_count, int vertex[][6], int x, int y){

	int i;
	static int result_vertex[3]; //first two will be the value of the vertex, last will contain the distance

	for (i = 0; i <= line_count; i++){
		printf("%d\n", result_vertex[2]);
		if (result_vertex[2] == 0){
			//deal with the first triangle
			//printf("first triangle\n");
			result_vertex[2] = find_distance(x, y, vertex[i][0], vertex[i][1]);
			result_vertex[0] = vertex[i][0];
			result_vertex[1] = vertex[i][1];
		}
		else{
			//remaining triangles
			//printf("remaining triangles\n");
			if (find_distance(x, y, vertex[i][0], vertex[i][1]) < result_vertex[2]){
				result_vertex[2] = find_distance(x, y, vertex[i][0], vertex[i][1]);
				result_vertex[0] = vertex[i][0];
				result_vertex[1] = vertex[i][1];
			}
		}
		
		if (find_distance(x, y, vertex[i][2], vertex[i][3]) < result_vertex[2]){
			result_vertex[2] = find_distance(x, y, vertex[i][2], vertex[i][3]);
			result_vertex[0] = vertex[i][2];
			result_vertex[1] = vertex[i][3];
		}
		
		if (find_distance(x,y, vertex[i][4], vertex[i][5]) < result_vertex[2]){
			result_vertex[2] = find_distance(x,y, vertex[i][4], vertex[i][5]);
			result_vertex[0] = vertex[i][4];
			result_vertex[1] = vertex[i][5];
		}
	}
	printf("closes vertex is (%d, %d) with distance of %d\n", result_vertex[0], result_vertex[1], result_vertex[2]);

	//returns the vertices closest along its distance
	return result_vertex;//, first_distance;
}

int main(int argc, char *argv[]){
	display = XOpenDisplay(NULL);
	win_height = 500;
	win_width = 500;
	count = 0;
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
					
					inTriangle = check_if_in_triangle(line_count, m, x, y);

					//if point isn't in the triangle draw dot
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
					printf("point x: %d, point y: %d\n", x, y);

					int vertex[line_count][6];
					int total_intersection[line_count][2];
					int *result_vertex;
					//int distance_of_nearest_vertex;

					for(i=0;i<=line_count;i++){
						//store formatted input file in array m
						fscanf(fp, "%*s ( %d, %d) ( %d, %d) (%d, %d)", &vertex[i][0], &vertex[i][1], &vertex[i][2], &vertex[i][3], &vertex[i][4], &vertex[i][5]);					
					}
					rewind(fp);


					//check any intersection with the nearest triangle to it
					result_vertex = nearest_line_seg(line_count, vertex, start_x, start_y);
					printf("closes vertex is (%d, %d) with distance of %d\n", result_vertex[0], result_vertex[1], result_vertex[2]);
					/*
					for(i = 0; i <= line_count; i++ ){
						printf("%d, %d, %d, %d, %d, %d\n", vertex[i][0], vertex[i][1], vertex[i][2], vertex[i][3], vertex[i][4], vertex[i][5]);
						result_vertex = nearest_line_seg(line_count, vertex, start_x, start_y);
						printf("closes vertex is (%d, %d) with distance of %d\n", result_vertex[0], result_vertex[1], result_vertex[2]);
						
						ABS = orientation(vertex[i][2], vertex[i][3], vertex[i][4], vertex[i][5], start_x, start_y);
						ABT = orientation(vertex[i][2], vertex[i][3], vertex[i][4], vertex[i][5], target_x, target_y);
						STA = orientation(start_x, start_y, target_x, target_y, vertex[i][2], vertex[i][3]);
						STB = orientation(start_x, start_y, target_x, target_y, vertex[i][4], vertex[i][5]);

						//printf("%d\n", ABS*ABT);
						//printf("%d\n", STA*STB);
						

						if((ABS*ABT) < 0 && (STA*STB) < 0){
							printf("Lines intersects.\n");
							count = -1;

							total_intersection[count_intersect][0]= vertex[i][0];
							total_intersection[count_intersect][1]= vertex[i][1];
							total_intersection[count_intersect+1][0]= vertex[i+1][0];
							total_intersection[count_intersect+1][1]= vertex[i+1][1];
							printf("%d, %d\n", total_intersection[count_intersect][0], total_intersection[count_intersect][1]);
							printf("%d, %d\n", total_intersection[count_intersect+1][0], total_intersection[count_intersect+1][1]);
							count_intersect++;
						}
						else{
							printf("Lines does not intersects.\n");
						}
						
					}
					*/
					//check intersection with the nearest triangle
					//ABS = orientation

					//stores the distance of the points it intersects
					int h[1][2];
					printf("total intersection: %d\n", count_intersect);
					double distance2;
					
					if (count_intersect > 0){

						for(i = 0; i < count_intersect+1; i++){
							printf("%d, %d\n", total_intersection[i][0], total_intersection[i][1]);
							distance1 = find_distance(start_x, start_y, total_intersection[i][0], total_intersection[i][1]);
							printf("The distance to %d, %d is %f\n", total_intersection[i][0], total_intersection[i][1] ,distance1);


							if (i == 0){
								h[0][0] = total_intersection[i][0];
								h[0][1] = total_intersection[i][1];
							}
							else{
								distance2 = find_distance(start_x, start_y, h[0][0], h[0][1]);
								printf("%f\n", distance1);
								printf("%f\n", distance2);
								if(distance1 < distance2){
									h[0][0] = total_intersection[i][0];
									h[0][1] = total_intersection[i][1];
								}
							}
						}

						printf("%d, %d, %f\n", h[0][0], h[0][1], find_distance(start_x, start_y, h[0][0], h[0][1]));
						XDrawLine(display, win, black_gc, start_x, start_y, h[0][0], h[0][1]);
					}
					else{
						XDrawLine(display, win, black_gc, start_x, start_y, target_x, target_y);
						count = -1;
					}

					
					//rewind(fp);
					count_intersect = 0;
				}
				else{
					printf("Count equal or more than 2\n");
				}
				if (inTriangle == 0){
					count++;
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

