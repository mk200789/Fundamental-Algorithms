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

char green[] = "#00FF00";
char red[] = "#A80000";
char black[] = "#000000";


int orientation(int ax, int ay, int bx, int by, int cx, int cy)
{
	return (ax*by) + (bx*cy) + (cx*ay) - (ay*bx) - (by*cx) - (cy*ax);
}

double find_distance(int x1, int y1, int x2, int y2)
{
	return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
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
		while(1){
			ch = fgetc(fp);
			if(feof(fp)){
				break;
			}
			if (ch == '\n'){
				line_count++;
			}
		}
		rewind(fp);
	}


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
				int m[line_count][2];

				for(i=0;i<=line_count;i++){
					//store formatted input file in array m
					fscanf(fp, "%d,%d", &m[i][0], &m[i][1]);
					//printf("%d %d\n", m[i][0], m[i][1]);

					//Magnify points by 2
					m[i][0] = (m[i][0]*4);
					m[i][1] = (m[i][1]*4);

					//printf("Scaled coordinates: %d, %d\n", m[i][0], m[i][1]);
				}

				for(i=0;i<line_count;i++){
					/* draw lines */
					XDrawLine(display, win, green_gc, m[i][0], m[i][1], m[i+1][0], m[i+1][1]);
				}
				//XDrawLine(display, win, green_gc, m[i][0], m[i][1], m[0][0], m[0][1]);
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
				if (report.xbutton.button == Button1){
					/* left click */
					XFillArc( display, win, black_gc, x, y, win_width/200, win_width/200, 0, 360*64);
				}
				else{
					//XFillArc( display, win, red_gc, x, y, win_width/150, win_width/150, 0, 360*64);
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

					int vertex[line_count][2];
					int total_intersection[line_count][2];

					for(i=0;i<=line_count;i++){
						//store formatted input file in array m
						printf("STORING1\n");
						fscanf(fp, "%d,%d", &vertex[i][0], &vertex[i][1]);

						vertex[i][0] = (vertex[i][0]*4);
						vertex[i][1] = (vertex[i][1]*4);
					}
					rewind(fp);

					

					for(i=0; i<line_count; i++){
						printf("COMPARING\n");

						ABS = orientation(vertex[i][0], vertex[i][1], vertex[i+1][0], vertex[i+1][1], start_x, start_y);
						ABT = orientation(vertex[i][0], vertex[i][1], vertex[i+1][0], vertex[i+1][1], target_x, target_y);
						STA = orientation(start_x, start_y, target_x, target_y, vertex[i][0], vertex[i][1]);
						STB = orientation(start_x, start_y, target_x, target_y, vertex[i+1][0], vertex[i+1][1]);

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
							//break;
						}
						else{
							printf("Lines does not intersects.\n");
							//break;
						}
					}

					//stores the distance of the points it intersects
					int h[1][2];
					printf("total intersection: %d\n", count_intersect);
					double distance2;
					
					if (count_intersect > 0){
						printf("IF1\n");

						for(i = 0; i < count_intersect+1; i++){
							printf("%d, %d\n", total_intersection[i][0], total_intersection[i][1]);
							distance1 = find_distance(start_x, start_y, total_intersection[i][0], total_intersection[i][1]);
							printf("The distance to %d, %d is %f\n", total_intersection[i][0], total_intersection[i][1] ,distance1);


							if (i == 0){
								printf("ZERO\n");
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
						printf("IF2\n");
						XDrawLine(display, win, black_gc, start_x, start_y, target_x, target_y);
						count = -1;
					}

					

					//rewind(fp);
					count_intersect = 0;
				}
				else{
					printf("Count equal or more than 2\n");
				}
				count++;
				
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

