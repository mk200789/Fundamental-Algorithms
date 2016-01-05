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
#include <limits.h>

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

//array holding vertical line segment
//first 4 holds value of x1, x2, y1, y2
int v_line_segment[200][4];
//array holding horizontal line segment
int h_line_segment[200][4];
//segments subgraphs v_segment_graph[][][0] holds distance and v_segment_graph[][][1] connected to
int v_segment_graph[200][200][2]; 
int h_segment_graph[200][200][2];



int horizontal_distance(int p1[], int p2[]){
	return abs(p1[0]-p2[0]);
}

int vertical_distance(int p1[], int p2[]){
	return abs(p1[1]-p2[1]);
}

int orientation(int ax, int ay, int bx, int by, int cx, int cy){
	
	//	If return value is equal to 0, vertices are collinear. [0]
	//	If return value is more than  0, triangle is counterclockwise. [1]
	//	If return value is less than  0, triangle is clockwise. [-1]
	        
    double d = ((ax*by) + (bx*cy) + (cx*ay) - (ay*bx) - (by*cx) - (cy*ax));
    
    if (d == 0){
        return 0;           
    }
    else if (d > 0){
    	return 1;
    }       
    else{
    	return -1;
    }         
}


int isSame_segment(int ax, int ay, int bx, int by){
	if (ax == bx && ay == by){
		//same line segment
		return 1;
	}
	else{
		return 0;
	}
}

int check_intersect(int v[], int h[]){
	/*
		Checks if the given line intersects with any of the triangle's line.
		Returns 1 if interesects, else return 0.
	*/

	int ABS = orientation(v[0], v[1], v[2], v[3], h[0], h[1]);
	int ABT = orientation(v[0], v[1], v[2], v[3], h[2], h[3]);
	int STA = orientation(h[0], h[1], h[2], h[3], v[0], v[1]);
	int STB = orientation(h[0], h[1], h[2], h[3], v[2], v[3]);
	//printf("ABS*ABT: %d. STA*STB: %d\n", ABS*ABT, STA*STB);
	if (isSame_segment(v[0], v[1], h[0], h[1]) == 1 && isSame_segment(v[2], v[3], h[2], h[3]) == 1){
		return 1;
	}
	if (ABS != ABT && STA != STB){
		return 1;
	}

	return 0;
	
}

void vertical_MST(v_count){
	//Prim's algorith: Adjacency Matrix Implementation.
	int i, j, k;
	// visited[][0] keep track of visited vertices ,
	// visited[][1] stores only visited index for constructing our vertical MST
	int visited[v_count][2];
	int minimum_path[v_count]; //stores the minimum path (edge)

	//default settings
	for(i=0; i<v_count; i++){
		//set minimum to large number
		minimum_path[i] = 999999;
		//set all vertices -1 , false
		visited[i][0] = -1;
	}

	//include the first vertex in our vertical MST
	minimum_path[0] = 0;
	visited[0][1] = -1;

	for(i=0; i< v_count-1; i++){
		int min = 999999;
		int min_index;
		//finding the minimum distance
		for(j=0; j<v_count; j++){
			//if unvisited compare min path distance
			if(visited[j][0] == -1 && minimum_path[j] < min){
				//printf("swap\n");
				min = minimum_path[j];
				min_index = j;
			}
		}

		visited[min_index][0] = 1; //set visited at this index to true
		//printf("min_index: %d\n", min_index);

		//update path
		for(k=0; k<v_count; k++){
			if (v_segment_graph[min_index][k][0] && visited[k][0] == -1 && v_segment_graph[min_index][k][0] < minimum_path[k]){
				visited[k][1] = min_index;
				minimum_path[k] = v_segment_graph[min_index][k][0];
			}
		}
	}
/*
	for (i=0; i<v_count;i++){
		printf("%d\n", visited[i][1]);
	}
*/
	return;
}

//generate path for vertical segments
void graph_vertical_segments(h_count, v_count){
	//you define a graph Gh with the horizontal segments as vertices, and join two
	//of these vertices by an edge if they are intersected by the same vertical segment; the
	//length of this edge is their distance along the segment.
	int h, i, j;
	
	//set default
	for (i=0; i<v_count; i++){
		for(j=0; j<v_count; j++){
			v_segment_graph[i][j][0] = 0; //distance default 0
			v_segment_graph[i][j][1] = -1; //connection to defaults -1
		}
	}

	for (i=0; i<v_count; i++){
		for (h=0; h<h_count; h++){
			for (j=i+1; j<v_count; j++){
				if (check_intersect(v_line_segment[i], h_line_segment[h]) == 1 && check_intersect(v_line_segment[j], h_line_segment[h]) == 1){
					//printf("%d, %d\n", v_line_segment[i][0], horizontal_distance(v_line_segment[i], v_line_segment[j]));
					//save distance and which horizontal segment connected to
					v_segment_graph[i][j][0] = horizontal_distance(v_line_segment[i], v_line_segment[j]);
					v_segment_graph[i][j][1] = h;

					v_segment_graph[j][i][0] = horizontal_distance(v_line_segment[i], v_line_segment[j]);
					v_segment_graph[j][i][1] = h;

					//printf("%d\n", v_segment_graph[i][j][0]);
				}
			}
		}
	}
	return;
}

void graph_horizontal_segments(h_count, v_count){
	//you define a graph Gh with the vertical segments as vertices, and join two
	//of these vertices by an edge if they are intersected by the same horizontal segment; the
	//length of this edge is their distance along the segment.
	int v, i, j;

	//set default
	for (i=0; i<h_count; i++){
		for(j=0; j<h_count; j++){
			h_segment_graph[i][j][0] = 0; //distance default 0
			h_segment_graph[i][j][1] = -1; //connection to defaults -1
		}
	}

	for (i=0; i<h_count; i++){
		for (v=0; v<v_count; v++){
			for (j=i+1; j<h_count; j++){
				if (check_intersect(h_line_segment[i], v_line_segment[v]) == 1 && check_intersect(h_line_segment[j], v_line_segment[v]) == 1){
					//printf("%d, %d\n", v_line_segment[i][0], horizontal_distance(v_line_segment[i], v_line_segment[j]));
					//save distance and which horizontal segment connected to
					h_segment_graph[i][j][0] = horizontal_distance(h_line_segment[i], h_line_segment[j]);
					h_segment_graph[i][j][1] = v;
					//backtracking purpose
					h_segment_graph[j][i][0] = h_segment_graph[i][j][0];
					h_segment_graph[j][i][1] = v;
					//h_line_segment[i][4] = vertical_distance(h_line_segment[i], h_line_segment[j]);
					//h_line_segment[i][5] = v;
				}
			}
		}
	}
	return;
}


int main(int argc, char *argv[]){
	FILE *fp;
	char c;
	int temp_line_count = 1;
	int line_count = 0; //holds the actual lines that aren't empty
	int h_count = 0;
	int v_count = 0;

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
		//printf("Success green!\n");
		XSetForeground(display, green_gc, green_col.pixel);

	}

	red_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, red, &red_col);
	if (XAllocColor(display, colormap, &red_col) == 0){
		//printf("Failed to get color red\n");
		exit(-1);
	}
	else{
		//printf("Success red!\n");
		XSetForeground(display, red_gc, red_col.pixel);
	}


	black_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, black, &black_col);
	if (XAllocColor(display, colormap, &black_col) == 0){
		printf("Failed to get color black\n");
		exit(-1);
	}
	else{
		//printf("Success black!\n");
		XSetForeground(display, black_gc, black_col.pixel);
	}


	light_purple_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, light_purple, &light_purple_col);
	if (XAllocColor(display, colormap, &light_purple_col) == 0){
		printf("Failed to get color light purple\n");
		exit(-1);
	}
	else{
		//printf("Success light purple!\n");
		XSetForeground(display, light_purple_gc, light_purple_col.pixel);
	}


	white_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, white, &white_col);	
	if (XAllocColor(display, colormap, &white_col) == 0){
		printf("Failed to get color white\n");
		exit(-1);
	}
	else{
		//printf("Success white!\n");
		XSetForeground(display, white_gc, white_col.pixel);	
	}






	printf("total raw line count: %d\n", temp_line_count);
	int i;
	//stores variable v or h
	char temp[temp_line_count];

	for (i=0; i <temp_line_count; i++){
		fscanf(fp, "%s %*d, %*d, %*d", &temp[i]);
	}
	rewind(fp);

	for (i=0; i< temp_line_count; i++){
		if (temp[i] == 104){
			fscanf(fp, "%*s %d, %d, %d", &h_line_segment[h_count][1], &h_line_segment[h_count][0], &h_line_segment[h_count][2]);
			h_line_segment[h_count][3] = h_line_segment[h_count][1];
			h_count++;
			line_count++;
		}
		if (temp[i] == 118){
			fscanf(fp, "%*s %d, %d, %d", &v_line_segment[v_count][0], &v_line_segment[v_count][1], &v_line_segment[v_count][3]);
			v_line_segment[v_count][2] = v_line_segment[v_count][0];
			v_count++;
			line_count++;
		}
	}
	rewind(fp);

	printf("total processed line count: %d\n", line_count);

	graph_vertical_segments(h_count, v_count);
	vertical_MST(v_count);


/*
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
*/
	fclose(fp);
	return 0;
}