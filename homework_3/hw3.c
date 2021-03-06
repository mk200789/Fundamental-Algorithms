/* 
	Compiles with command line  gcc -o assign3 hw3.c -lX11 -lm -L/usr/X11R6/lib
	Run : ./assign3 test_in2
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
char *win_name_string = "Homework 3";
char *icon_name_string = "Icon for homework 3 window";
unsigned long valuemask = 0;

XEvent report;

GC gc, green_gc, red_gc, black_gc, blue_gc;
XColor green_col, red_col, black_col, blue_col;
Colormap colormap;

char green[] = "#1F8728";
char red[] = "#FF3333";
char black[] = "#000000";
char blue[] = "#0000FF";

//array holding vertical/horizontal line segment
//first 4 holds value of x1, x2, y1, y2
int v_line_segment[200][4];
int h_line_segment[200][4];
//segments subgraphs v_segment_graph[][][0] holds distance 
//and v_segment_graph[][][1] connected to
int v_segment_graph[200][200][2]; 
int h_segment_graph[200][200][2];
//holds intersecting lines
int intersections[200][200][3];
int intersections_point[200][2];
int graph[400][400];
//keep track of intersections
int count_intersection;

int total_h, total_v, total;



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


int check_intersect(int v[], int h[]){
	/*
		Checks if the given line intersects with any of the triangle's line.
		Returns 1 if interesects, else return 0.
	*/

	int ABS = orientation(v[0], v[1], v[2], v[3], h[0], h[1]);
	int ABT = orientation(v[0], v[1], v[2], v[3], h[2], h[3]);
	int STA = orientation(h[0], h[1], h[2], h[3], v[0], v[1]);
	int STB = orientation(h[0], h[1], h[2], h[3], v[2], v[3]);
	
	if ((h[0] == v[0] && v[1] == h[1]) && (h[3] == v[3] && v[2] == h[3])){
		return 1;
	}
	if (ABS != ABT && STA != STB){
		return 1;
	}

	return 0;
	
}

void printMST(int visited[][2], int count, int who){
	//who: 1 is for vertical , 0 is for horizontal
	int i, distance;
	if (who == 1){
		//vertical
		for (i=1; i< count; i++){
			distance = v_segment_graph[i][visited[i][1]][1];
	        //printf("x1: %d, y1: %d\n", v_line_segment[visited[i][1]][0], h_line_segment[distance][1]);
	        //printf("x2: %d, y2: %d\n", v_line_segment[i][0], h_line_segment[distance][1]);
			XDrawLine(display, win, blue_gc, v_line_segment[visited[i][1]][0], h_line_segment[distance][1],v_line_segment[i][0], h_line_segment[distance][1]);
			total_v += v_segment_graph[i][visited[i][1]][0];

		}
	}
	else if (who == 0){
		//horizontal
		for (i=1; i<count; i++){
			distance = h_segment_graph[i][visited[i][1]][1];
			XDrawLine(display, win, green_gc, 
				v_line_segment[distance][0], h_line_segment[i][1],
				v_line_segment[distance][0], h_line_segment[visited[i][1]][1]);
			total_h += h_segment_graph[i][visited[i][1]][0];
		}
	}
	else if (who == 2){
		//uniting
		for (i=1; i<count; i++){
			distance = graph[i][visited[i][1]];
			XDrawLine(display, win, red_gc, 
				intersections_point[i][0], intersections_point[i][1],
				intersections_point[visited[i][1]][0], intersections_point[visited[i][1]][1]);
			total += distance;
		}
	}
	return;
}

void vertical_MST(int v_count){
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

		//update path
		for(k=0; k<v_count; k++){
			if (v_segment_graph[min_index][k][0] && visited[k][0] == -1 && v_segment_graph[min_index][k][0] < minimum_path[k]){
				visited[k][1] = min_index;
				minimum_path[k] = v_segment_graph[min_index][k][0];
			}
		}
	}

	printMST(visited, v_count, 1);
	return;
}

void horizontal_MST(int h_count){
	//Prim's algorith: Adjacency Matrix Implementation.
	int i, j, k;
	// visited[][0] keep track of visited vertices ,
	// visited[][1] stores only visited index for constructing our vertical MST
	int visited[h_count][2];
	int minimum_path[h_count]; //stores the minimum path (edge)

	//default settings
	for(i=0; i<h_count; i++){
		//set minimum to large number
		minimum_path[i] = 999999;
		//set all vertices -1 , false
		visited[i][0] = -1;
	}

	//include the first vertex in our vertical MST
	minimum_path[0] = 0;
	visited[0][1] = -1;

	for(i=0; i<h_count-1; i++){
		int min = 999999;
		int min_index;
		//finding the minimum distance
		for(j=0; j<h_count; j++){
			//if unvisited compare min path distance
			if(visited[j][0] == -1 && minimum_path[j] < min){
				min = minimum_path[j];
				min_index = j;
			}
		}

		visited[min_index][0] = 1; //set visited at this index to true

		//update path
		for(k=0; k<h_count; k++){
			if (h_segment_graph[min_index][k][0] && visited[k][0] == -1 && h_segment_graph[min_index][k][0] < minimum_path[k]){
				visited[k][1] = min_index;
				minimum_path[k] = h_segment_graph[min_index][k][0];
			}
		}
	}

	printMST(visited, h_count, 0);
	return;
}

//generate path for vertical segments
void graph_vertical_segments(int h_count, int v_count){
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

					v_segment_graph[j][i][0] = v_segment_graph[i][j][0];
					v_segment_graph[j][i][1] = h;
				}
			}
		}
	}
	return;
}

//generate path for horizontal segments
void graph_horizontal_segments(int h_count, int v_count){
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
					//save distance and which horizontal segment connected to
					h_segment_graph[i][j][0] = vertical_distance(h_line_segment[i], h_line_segment[j]);
					h_segment_graph[i][j][1] = v;
					//backtracking purpose
					h_segment_graph[j][i][0] = h_segment_graph[i][j][0];
					h_segment_graph[j][i][1] = v;
				}
			}
		}
	}
	return;
}
void graph_segments(int h_count, int v_count){
	int i, j, k;

	//set default values for intersections[]
	for (i=0; i<v_count; i++){
		for(j=0; j<h_count; j++){
			intersections[i][j][0] = -1; //x value
			intersections[i][j][1] = -1; //y value
			intersections[i][j][2] = -1; //index value
		}
	}

	for (i=0; i<h_count; i++){
		for(j=0; j<v_count; j++){
			//if line intersects, store in intersections[]
			if (check_intersect(h_line_segment[i], v_line_segment[j]) == 1){
				
				intersections[i][j][0] = v_line_segment[j][0];
				intersections[i][j][1] = h_line_segment[i][1];
				intersections[i][j][2] = count_intersection;

				intersections_point[count_intersection][0] = intersections[i][j][0];
				intersections_point[count_intersection][1] = intersections[i][j][1];
				
				count_intersection++;
			}
		}
	}
	int temp_i, temp_j;

	//vertical connected intersections
	for (i=0; i<v_count; i++){
		for (j=0; j<h_count; j++){
			for(k=j+1; k<h_count; k++){
				if(intersections[j][i][0] != -1 && intersections[k][i][0] != -1){
					temp_i = intersections[j][i][2];
					temp_j = intersections[k][i][2];

					graph[temp_i][temp_j] = vertical_distance(intersections[j][i], intersections[k][i]);
					graph[temp_j][temp_i] = graph[temp_i][temp_j];
				}
			}
		}
	}

	//horizontal connected intersections
	for (i=0; i<h_count; i++){
		for (j=0; j<v_count; j++){
			for(k=j+1; k<v_count; k++){
				if(intersections[i][j][0] != -1 && intersections[i][k][0] != -1){
					temp_i = intersections[i][j][2];
					temp_j = intersections[i][k][2];

					graph[temp_i][temp_j] = horizontal_distance(intersections[i][j], intersections[i][k]);
					graph[temp_j][temp_i] = graph[temp_i][temp_j];
				}
			}
		}
	}

	return;
}

void graph_MST(){
	//Prim's algorith: Adjacency Matrix Implementation.
	int i, j, k;
	// visited[][0] keep track of visited vertices ,
	// visited[][1] stores only visited index for constructing our vertical MST
	int visited[count_intersection][2];
	int minimum_path[count_intersection]; //stores the minimum path (edge)

	//default settings
	for(i=0; i<count_intersection; i++){
		//set minimum to large number
		minimum_path[i] = 999999;
		//set all vertices -1 , false
		visited[i][0] = -1;
	}

	//include the first vertex in our vertical MST
	minimum_path[0] = 0;
	visited[0][1] = -1;

	for(i=0; i<count_intersection-1; i++){
		int min = 999999;
		int min_index;
		//finding the minimum distance
		for(j=0; j<count_intersection; j++){
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
		for(k=0; k<count_intersection; k++){
			if (graph[min_index][k] && visited[k][0] == -1 && graph[min_index][k] < minimum_path[k]){
				//printf("hheya: %d\n", graph[min_index][k]);
				visited[k][1] = min_index;
				minimum_path[k] = graph[min_index][k];
			}
		}
	}

	printMST(visited, count_intersection, 2);

	return;
}



int main(int argc, char *argv[]){
	FILE *fp;
	char c;
	int temp_line_count = 1;
	int line_count = 0; //holds the actual lines that aren't empty
	int h_count = 0;
	int v_count = 0;
	int count=0;
	count_intersection = 0;
	total_h = 0;
	total_v = 0;
	total = 0;

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
	XSetLineAttributes(display, red_gc, 2, LineSolid, CapRound, JoinRound);
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



	printf("total raw line count: %d\n", temp_line_count);
	int i;
	//stores variable v or h
	char temp[temp_line_count];
	//stores array to display
	int m[temp_line_count][4];

	for (i=0; i <temp_line_count; i++){
		fscanf(fp, "%s %*d, %*d, %*d", &temp[i]);
	}
	rewind(fp);

	for (i=0; i< temp_line_count; i++){
		if (temp[i] == 104){
			fscanf(fp, "%*s %d, %d, %d", &h_line_segment[h_count][1], &h_line_segment[h_count][0], &h_line_segment[h_count][2]);
			h_line_segment[h_count][3] = h_line_segment[h_count][1];
			m[line_count][0]= h_line_segment[h_count][0];
			m[line_count][1]= h_line_segment[h_count][1];
			m[line_count][2]= h_line_segment[h_count][2];
			m[line_count][3]= h_line_segment[h_count][3];
			h_count++;
			line_count++;
		}
		if (temp[i] == 118){
			fscanf(fp, "%*s %d, %d, %d", &v_line_segment[v_count][0], &v_line_segment[v_count][1], &v_line_segment[v_count][3]);
			v_line_segment[v_count][2] = v_line_segment[v_count][0];
			m[line_count][0]= v_line_segment[v_count][0];
			m[line_count][1]= v_line_segment[v_count][1];
			m[line_count][2]= v_line_segment[v_count][2];
			m[line_count][3]= v_line_segment[v_count][3];
			v_count++;
			line_count++;
		}
	}
	rewind(fp);

	printf("total processed line count: %d\n", line_count);


	while(1){
		XNextEvent(display, &report);
		switch(report.type){
			case Expose:
			{	
				for (i = 0; i <line_count; i++){
					//Draw the line segments
					XDrawLine(display, win, black_gc, m[i][0], m[i][1], m[i][2], m[i][3]);
				}

				XFlush(display);
				break;
			}
			case ButtonPress:
			{
				if (report.xbutton.button == Button1){
					if (count == 0){
						//left click
						graph_vertical_segments(h_count, v_count);
						vertical_MST(v_count);
						graph_horizontal_segments(h_count, v_count);
						horizontal_MST(h_count);
						graph_segments(h_count, v_count);
						graph_MST();
						printf("total_v: %d\n", total_v);
						printf("total_h: %d\n", total_h);
						printf("solutions lower bound: %d\n", total_v+total_h);
						printf("total: %d\n", total);
						printf("ration on screen: %f\n", (double)total/(total_v+total_h));
					}
					count++;
				}
				else{
					printf("Closing Window.\n");
					XDestroyWindow(display, win);
					XCloseDisplay(display);
					exit(1);
				}
			}
			default:
				break;
		}
	}

	fclose(fp);
	return 0;
}