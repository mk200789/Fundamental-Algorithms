/* 
	Compiles and run with command lines
			gcc -o assign1 hw1_working.c -lX11 -lm -L/usr/X11R6/lib
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
#include <limits.h>

#define MAX_VERTICES 600
#define MAX_TRIANGLES 100

typedef enum { false, true } bool;

typedef struct{
	int x;
	int y;
}Point;

typedef struct{
	Point p;
	Point q;
	Point r;
}Triangle;

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
char *win_name_string = "Homework 1";
char *icon_name_string = "Icon for homework 1 window";
unsigned long valuemask = 0;

XEvent report;

GC gc, green_gc, red_gc, black_gc, light_purple_gc, white_gc;
XColor green_col, red_col, black_col, light_purple_col, white_col;
Colormap colormap;


//vertices that will be used for dijkstra
int vertices_info[MAX_VERTICES][2];
//holds point .from where to where
Point vertices[MAX_VERTICES][2];
//holds triangles
Triangle triangles[MAX_TRIANGLES];

Point point[MAX_TRIANGLES];

int num_line_segment;
int line_count;
int num_point;
int graph[MAX_VERTICES][MAX_VERTICES];
int parent[MAX_VERTICES];
Point vv[MAX_VERTICES][MAX_VERTICES][2];

char white[] = "#FFFFFF";
char green[] = "#00FF00";
char red[] = "#A80000";
char black[] = "#000000";
char light_purple[] = "#FFCCFF";

////////////////////////////////////////////////////////////////////////////////////////////////////

int orientation(Point a, Point b, Point c){

	return (a.x*b.y) + (b.x*c.y) + (c.x*a.y) - (a.y*b.x) - (b.y*c.x) - (c.y*a.x);
}

int find_distance(Point a, Point b){
	/*
		Returns the distance between two points.
	*/
	return (int) sqrt((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y));
}

bool isSamePoint (Point p, Point q) {
    if (p.x == q.x && p.y == q.y)
        return true;
    return false;
}

bool isIntriangle(int line_count, Point p1){
	/*
		Checks if a point(vertex) is within another triangle.
		Return 1 if point is n triangle, else return 0.
	*/
	int PAB, PBC, PAC, CAB, BAC, ABC;
	int i;
	for (i = 0; i < line_count; i++){

		PAB = orientation(p1, triangles[i].p, triangles[i].q);
		PBC = orientation(p1, triangles[i].q, triangles[i].r);
		PAC = orientation(p1, triangles[i].p, triangles[i].r);
		CAB = orientation(triangles[i].r, triangles[i].p, triangles[i].q);
		ABC = orientation(triangles[i].p, triangles[i].q, triangles[i].r);
		BAC = orientation(triangles[i].q, triangles[i].p, triangles[i].r);
		
		if (PAB*CAB > 0  && PBC*ABC > 0 && PAC*BAC > 0 ){
			//point is in the triangle
			return true;
		}
	}
	return false;
}

bool isIntriangle1(Point p1, Point a, Point b, Point c){
	/*
		Checks if a point(vertex) is within another triangle.
		Return 1 if point is n triangle, else return 0.
	*/

	if (orientation(p1, a, b)*orientation(c, a, b) > 0  && 
		orientation(p1, b, c)*orientation(a, b, c) > 0 && 
		orientation(p1, a, c)*orientation(b, a, c) > 0 ){
		//point is in the triangle
		return true;
	}

	return false;
}

bool isIntersect(Point P, Point Q, Point R, Point S){
	if (isSamePoint(P, R) && isSamePoint(Q,S)){
		return true;
	}
	if (orientation(P, Q, R)*orientation(P, Q, S)<0 && orientation(R,S,P)*orientation(R, S, Q)<0){
		return true;
	}
	return false;
}

int ccw(Point a, Point b, Point c){
	return (c.y-a.y) * (b.x-a.x) > (b.y - a.y)*(c.x  - a.x);
}

int intersect1(Point a, Point b, Point c, Point d){
	return ccw(a,c,d) != ccw(b,c,d) && ccw(a,b,c) != ccw(a,b,d);
}


void start_graph(Point start, Point target){
	int i,j, k;
	Point p, q;
	bool intersect = false;
	
	for(i=0; i<num_point; i++){
		for(j=0; j<num_point; j++){
			p = point[i];
			q = point[j];

			if (isIntriangle(line_count, p) || isIntriangle(line_count, q)){}
			else{
				for(k=0; k<line_count; k++){

					if (isIntersect(p, q, triangles[k].p, triangles[k].q) || isIntersect(p, q, triangles[k].q, triangles[k].r) || isIntersect(p, q, triangles[k].r, triangles[k].p)){

						if (intersect1(p, q, triangles[k].p, triangles[k].q) || intersect1(p, q, triangles[k].q, triangles[k].r) || intersect1(p, q, triangles[k].r, triangles[k].p)){
							intersect = true;
							break;
						}
					}
				}
				if(!intersect){
					graph[i][j] = find_distance(p,q);
					vv[i][j][0] = p;
					vv[i][j][1] = q;
				}
				intersect = false;
			}
		}
	}

	return;
}

int minimum_distance(int distance[], int processed[]){
	int min = INT_MAX; 
	int min_index=0;
	int i;

	for(i=0; i<num_point; i++){
		if(processed[i] == 0 && distance[i]< min){
			min = distance[i]; 
			min_index = i;
		}
	}
	return min_index;
}

void dijkstra(int graph[][MAX_VERTICES], int src){
	int i, j, v;
	int processed[num_point], distance[num_point];
	int temp_v;

	//initialize parent
	for(i=0;i<MAX_VERTICES; i++){
		parent[i]= INT_MAX;//999999;
	}
	
	//initialize
	for(i=0; i<num_point; i++){
		processed[i]=0; 
		distance[i]= INT_MAX;//999999;
	}

	distance[src] = 0;

	for(i=0; i<num_point-1; i++){
		int u = minimum_distance(distance, processed);
		processed[u] = 1;

		for (v=0; v< num_point; v++){
			//printf("%d\n", graph[u][v]);
			if (!processed[v] && graph[u][v] && distance[u] != INT_MAX && distance[u] < distance[v]){
				distance[v] = distance[u] + graph[u][v];
				parent[v] = u;
			}
		}
	}
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]){
	FILE *fp;
	int i, j;
	int click_count = -1;
	num_line_segment = 0;
	num_point =0;
	line_count = 0;

	//store the x,y values of target and start point
	Point start, target, p1;
	
	fp = fopen(argv[1], "r");

	if (fp == NULL){
		printf("There's no input file.\n");
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
		
		//temp holds for x, y
		int x1, y1, x2,y2, x3, y3;
		char t;
		fscanf(fp, "%s (%d,%d) (%d,%d) (%d,%d)", &t, &x1, &y1, &x2, &y2, &x3, &y3);
		
		//obtaining line count
		while(!feof(fp)){
			//save
			if (t == 84){
				//save triangles
				triangles[line_count].p.x = x1+100; triangles[line_count].p.y = y1+100;
				triangles[line_count].q.x = x2+100; triangles[line_count].q.y = y2+100;
				triangles[line_count].r.x = x3+100; triangles[line_count++].r.y = y3+100;
			}
			fscanf(fp, "%s (%d,%d) (%d,%d) (%d,%d)", &t, &x1, &y1, &x2, &y2, &x3, &y3);
		}
		rewind(fp);
	}

	for (i=0; i<line_count; i++){
		printf("(%d , %d) (%d, %d) (%d, %d)\n", triangles[i].p.x, triangles[i].p.y, triangles[i].q.x, triangles[i].q.y, triangles[i].r.x, triangles[i].r.y);
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
	class_hints -> res_class = "homework1";
	
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

	printf("Total triangles: %d\n", line_count);

	while(1){
		XNextEvent(display, &report);
		switch(report.type){
			case Expose:
			{	

				for (i=0; i< line_count ; i++){
					//Draw the triangles
					XDrawLine(display, win, green_gc, triangles[i].p.x, triangles[i].p.y, triangles[i].q.x, triangles[i].q.y);
					XDrawLine(display, win, green_gc, triangles[i].q.x, triangles[i].q.y, triangles[i].r.x, triangles[i].r.y);
					XDrawLine(display, win, green_gc, triangles[i].r.x, triangles[i].r.y, triangles[i].p.x, triangles[i].p.y);
				}

				XFlush(display);
				break;
			}

			case ButtonPress:
			{
				p1.x = report.xbutton.x;
				p1.y = report.xbutton.y;

				if (report.xbutton.button == Button1){
					click_count++;
					
					if(isIntriangle(line_count, p1)){
						//if inside triangle reset click_count to -1
						printf("in triangle!\n");
						click_count = -1;
					}
					else{
						printf("outside triangle :)\n");
					}

				}
				else{
					printf("Closing Window.\n");
					XDestroyWindow(display, win);
					XCloseDisplay(display);
					exit(1);
				}

				if (click_count == 0){
					//first click;
					start = p1;
					//point[num_point++] = start;
				}
				else if (click_count == 1){
					//second click
					target = p1;
					
					for(i=0; i<line_count; i++){
						point[num_point++] = triangles[i].p; 
						point[num_point++] = triangles[i].q; 
						point[num_point++] = triangles[i].r;
					}

					point[num_point++] = start;
					point[num_point++] = target;

					printf("start: (%d, %d). target: (%d, %d).\n", start.x, start.y, target.x, target.y);

					start_graph(start, target);

					dijkstra(graph, num_point-2);

					int index = num_point-1;
					int vertex_count = 0;

					Point result_vertices[MAX_VERTICES][2];
					int distance[MAX_VERTICES];
					
					while(index != num_point-2){
						if(parent[index] == INT_MAX){
							break;
						}

						printf("%d\n", vertex_count);
						if (vertex_count == 0){
							result_vertices[vertex_count][0] = point[index];
							result_vertices[vertex_count][1] = point[index];
							distance[vertex_count++] = 0;
						}	
						else {
							result_vertices[vertex_count][0] = result_vertices[vertex_count-1][1];
							result_vertices[vertex_count][1] = point[index];
							distance[vertex_count++] = find_distance(result_vertices[vertex_count][0], result_vertices[vertex_count][1]);
						}

						index = parent[index];
					}

					result_vertices[vertex_count][0] = result_vertices[vertex_count-1][1];
					result_vertices[vertex_count][1] = point[num_point-2];
					distance[vertex_count++] = find_distance(result_vertices[vertex_count-1][1], point[num_point-2]);

					for (i=0; i<vertex_count; i++){
						printf("%d %d to %d %d distance: %d\n", result_vertices[i][0].x, result_vertices[i][0].y, result_vertices[i][1].x, result_vertices[i][1].y, distance[i]);
						XDrawLine(display, win, black_gc, result_vertices[i][0].x, result_vertices[i][0].y, result_vertices[i][1].x, result_vertices[i][1].y);
					}

					printf("complete\n");

					click_count = -1;
				}
				else{

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