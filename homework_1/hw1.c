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
GC green_gc, red_gc, black_gc, light_purple_gc;
XEvent report;
XColor green_col, red_col, black_col, light_purple_col;
Colormap colormap;

unsigned int win_width, win_height;

//store the x,y values of target and start point
unsigned int start_x, start_y , target_x, target_y;

//count keep track of the left clicks
int count, count_intersect;

long ABS, ABT, STA, STB;
int PAB, PBC, PAC, CAB, BAC, ABC;

//vertices that will be used for dijkstra
double valid_vertex[200][7];
int k;

char green[] = "#00FF00";
char red[] = "#A80000";
char black[] = "#000000";
char light_purple[] = "#FFCCFF";


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
				
				val = 1;
				
			}
	
		}
	}

	return val;
	
}

int expand(int line_count, int vertex[][6], int m, int n, int current_x, int current_y){
	
	int i, j, l;
	
	for (i=0; i<=line_count; i++){
		for (j=0; j<6; j+=2){
			if (i==m && j==n){
				printf("skip to (%d, %d)\n", vertex[i][j], vertex[i][j+1]);
			}
			else{
				//printf("check if exist: %d %d\n", vertex[i][j], vertex[i][j+1]);
				
				if (check_if_in_triangle(line_count, vertex, vertex[i][j], vertex[i][j+1]) == 0){
					//printf("point not in triangle\n");
					if (check_intersect(line_count, vertex, current_x, current_y, vertex[i][j], vertex[i][j+1])== 0){
						//printf("does not intersect\n");
						if( vertex_exist(vertex[i][j], vertex[i][j+1], current_x, current_y) == 0){
							//printf("vertex dne\n");
							printf("dne : (%d, %d) to (%d, %d)\n", current_x, current_y, vertex[i][j], vertex[i][j+1]);
							//XDrawLine(display, win, light_purple_gc, current_x, current_y, vertex[i][j], vertex[i][j+1]);
							valid_vertex[k][0] = current_x;
							valid_vertex[k][1] = current_y;
							valid_vertex[k][2] = vertex[i][j];
							valid_vertex[k][3] = vertex[i][j+1];
							valid_vertex[k][4] = find_distance(current_x, current_y, vertex[i][j], vertex[i][j+1]);
							valid_vertex[k][5] = -1.0;
							valid_vertex[k++][6] = -1.0;

							if (vertex_exist(current_x, current_y, vertex[i][j], vertex[i][j+1]) == 0){
								valid_vertex[k][0] = vertex[i][j];
								valid_vertex[k][1] = vertex[i][j+1];
								valid_vertex[k][2] = current_x;
								valid_vertex[k][3] = current_y;
								valid_vertex[k][4] = find_distance(current_x, current_y, vertex[i][j], vertex[i][j+1]);
								valid_vertex[k][5] = -1.0;
								valid_vertex[k++][6] = -1.0;
							}
						}
						else{
							//printf("vertex exists\n");
						}
					}
					else{
						//printf("does intersect\n");
					}
				}
			}
		}
	}
}

void valid_vertices(int line_count, int vertex[][6], int i, int current_x, int current_y){
	//get all the valid vertices in a new list
	
	printf("========================= VALID_VERTICES() =========================\n");

	int m, n;

	for (m=0; m<=line_count; m++){

		for(n=0; n<6; n+=2){

			int f = m;
			printf("F: %d\n", f);

			while(vertex[f][n]!= -5 && f<=line_count+1){

				if (check_if_in_triangle(line_count, vertex, vertex[f][n], vertex[f][n+1]) == 0){
					
					expand(line_count, vertex, f, n, vertex[f][n], vertex[f][n+1]);
				}
				f++; 
			}
		}
	}
}

int vertex_exist(int x, int y, int current_x, int current_y){

	int i;
	if (current_x == x && current_y == y){
		return 1;
	}
	else{
		for (i=0; i<k; i++){
			if (valid_vertex[i][0] == x && valid_vertex[i][1] == y && valid_vertex[i][2] == x && valid_vertex[i][3] == y){
				//includes distance from and to itself
				return 1;
			}
			else if(valid_vertex[i][0] == current_x && valid_vertex[i][1] == current_y && valid_vertex[i][2] == x && valid_vertex[i][3] == y){
				// there's an exact vertex with same from and to information/direction
				return 1;
			}
		}
	}

	return 0;
}

void end_graph(int line_count, int vertex[][6], int target_x, int target_y){
	
	int i, j;

	for(i=0; i<=line_count; i++){
		for(j=0; j<6; j+=2){
			if(check_if_in_triangle(line_count, vertex, vertex[i][j], vertex[i][j+1])==0){

				if(check_intersect(line_count, vertex, vertex[i][j], vertex[i][j+1], target_x, target_y) == 0){
					//printf("%d %d %d %d\n", vertex[i][j], vertex[i][j+1],target_x, target_y);
					if (vertex_exist(vertex[i][j], vertex[i][j+1], target_x, target_y) == 0){
						//printf("%d %d %d %d\n", vertex[i][j], vertex[i][j+1],target_x, target_y);
						valid_vertex[k][0] = vertex[i][j];
						valid_vertex[k][1] = vertex[i][j+1];
						valid_vertex[k][2] = target_x;
						valid_vertex[k][3] = target_y;
						valid_vertex[k][4] = find_distance(vertex[i][j], vertex[i][j+1], target_x, target_y);
						valid_vertex[k][5] = -1.0;
						valid_vertex[k++][6] = -2.0;
					}
				}
			}
		}
	}
}

void start_graph(int line_count, int vertex[][6], int start_x, int start_y, int target_x, int target_y){

	int i, j;

	//store the start point in valid_vertex 
	valid_vertex[k][0] = start_x;	// from x value
	valid_vertex[k][1] = start_y;   // from y value
	valid_vertex[k][2] = start_x;	// to x value
	valid_vertex[k][3] = start_y;	// to y value
	valid_vertex[k][4] = 0.0;		// distance from point a to b
	valid_vertex[k][5] = -1.0;		// total distance
	valid_vertex[k++][6] = 0.0;		// -2.0 target , 1.0 has visited not included, 0.0 using, -1.0 not yet visited


	for (i=0; i<=line_count; i++){

		for (j=0; j<6; j+=2){

			if (check_if_in_triangle(line_count, vertex, vertex[i][j], vertex[i][j+1]) == 0){

				if (check_intersect(line_count, vertex, start_x, start_y, vertex[i][j], vertex[i][j+1]) == 0){
					//XDrawLine(display, win, black_gc, start_x, start_y, vertex[i][j], vertex[i][j+1]);
					printf("to (%d, %d)\n", vertex[i][j], vertex[i][j+1]);
					
					valid_vertex[k][0] = start_x;
					valid_vertex[k][1] = start_y;
					valid_vertex[k][2] = vertex[i][j];
					valid_vertex[k][3] = vertex[i][j+1];
					valid_vertex[k][4] = find_distance(vertex[i][j], vertex[i][j+1], start_x, start_y);
					valid_vertex[k][5] = -1.0;
					valid_vertex[k++][6] = -1.0;
					
					valid_vertices(line_count, vertex, i, vertex[i][j], vertex[i][j+1]);

				}
			}
		}
	}
}


int smallest_length(int total, int length_list[]){

	printf("========================= SMALLEST_LENGTH() =========================\n");
	
	double smallest;
	int i, j, m, goal;
	printf("j23: %d\n", length_list[0]);

	if (valid_vertex[length_list[0]][6] == -2.0){
	}
	else{
		smallest = valid_vertex[length_list[0]][5];
		valid_vertex[length_list[0]][6] = 0.0;
	}
	m = length_list[0];
	goal = 0;

	for (i=0; i<total; i++){
		
		//printf("weight: %f ____ %f __%f\n", valid_vertex[length_list[i]][4], valid_vertex[length_list[i]][5], valid_vertex[length_list[i]][6]);

		j = length_list[i];
		printf("good: (%d %d) to (%d %d) distance: %f   weight: %f  endcode: %f\n", (int)valid_vertex[j][0], (int)valid_vertex[j][1], (int)valid_vertex[j][2], (int)valid_vertex[j][3], valid_vertex[j][4], valid_vertex[j][5], valid_vertex[j][6]);
		
		if (valid_vertex[j][6] == -2.0){
			valid_vertex[m][6] = -1.0;
			smallest = valid_vertex[j][5];
			//valid_vertex[j][6] = 0.0;
			valid_vertex[j][6] = -4.0;
			//printf("%f\n", valid_vertex[j][6]);
			m = j;
			//printf("GOAL!");
			goal = 1;
			break;
		}
		if (valid_vertex[j][6] == -1.0){
			if (valid_vertex[j][5] <= smallest){
				valid_vertex[m][6] = -1.0;
				valid_vertex[j][6] = 0.0;
				smallest = valid_vertex[j][5];
				m = j;
			}
		}
	}

	printf("smallest: %f %d %d last %f\n", smallest, m, goal, valid_vertex[m][6]);
	if (goal == 1){
		//printf("BINGO!\n");
		return -100;
	}
	else{
		return m;
	}
}

void check_reverse_seg(int index){
	printf("========================= CHECK_REVERSE_SEG() =========================\n");
	int i, p;

	for(i=0; i<k; i++){
		
		if (valid_vertex[i][0] == valid_vertex[index][2] && valid_vertex[i][1] == valid_vertex[index][3] && valid_vertex[i][2] == valid_vertex[index][0] && valid_vertex[i][3] == valid_vertex[index][1]){
			//if there's a reverse set it it to seen
			printf("reverse: (%d, %d) to (%d, %d)\n", (int)valid_vertex[i][0], (int)valid_vertex[i][1], (int)valid_vertex[i][2], (int)valid_vertex[i][3]);
			valid_vertex[i][6] = 1.0;
		}

		if (valid_vertex[i][0] == valid_vertex[index][0] && valid_vertex[i][1] == valid_vertex[index][1]){
			//if (valid_vertex[i][6] != 0.0 && valid_vertex[i][6] != -2.0){
				//printf("1. anything besides those who initially come first to this location: (%d, %d) to (%d, %d)\n", (int)valid_vertex[i][0], (int)valid_vertex[i][1], (int)valid_vertex[i][2], (int)valid_vertex[i][3]);
				//valid_vertex[i][6] = 1.0;

				for (p=0; p<k; p++){
					if (valid_vertex[p][0] == valid_vertex[i][2] && valid_vertex[p][1] == valid_vertex[i][3] && valid_vertex[p][2] == valid_vertex[i][0] && valid_vertex[p][3] == valid_vertex[i][1]){
						if (valid_vertex[p][6] != 0.0 && valid_vertex[p][6] != -2.0 && valid_vertex[i][6] != 0.0 && valid_vertex[i][6] != -2.0){
							printf("2. anything besides those who initially come first to this location: (%d, %d) to (%d, %d) %f\n", (int)valid_vertex[p][0], (int)valid_vertex[p][1], (int)valid_vertex[p][2], (int)valid_vertex[p][3], valid_vertex[p][6]);
							valid_vertex[p][6] = 1.0;
						}
					}
				}
			//}
		}
	}
}

int check_alt_path_to_point(int index, int index_of_smallest){

	printf("========================= CHECK_ALT_PATH_TO_POINT() =========================\n");

	int i, temp_k, j, m;
	int temp[6];
	
	temp_k = k;
	j = 0;

	printf("index: %d      index_of_smallest: %d\n", index, index_of_smallest);

	for(i=0; i<temp_k; i++){
		
		//printf("%d %d\n", (int)valid_vertex[i][0], (int)valid_vertex[i][1]);
		printf("%d == %d && %d == %d  \n", (int)valid_vertex[i][0], (int)valid_vertex[index][0], (int)valid_vertex[i][1], (int)valid_vertex[index][1]);
		if (valid_vertex[i][0] == valid_vertex[index][0] && valid_vertex[i][1] == valid_vertex[index][1]){
			
			printf("(%d %d) to (%d %d) %f\n", (int)valid_vertex[i][0], (int)valid_vertex[i][1], (int)valid_vertex[i][2], (int)valid_vertex[i][3], valid_vertex[i][6]);
			if (valid_vertex[i][6] != 0.0 && valid_vertex[i][6] != 1.0){

				//vertex_exist(int x, int y, int current_x, int current_y)
				printf("exist?: %d\n" ,vertex_exist(valid_vertex[i][2], valid_vertex[i][3], valid_vertex[i][0], valid_vertex[i][1]));
				
				//if (vertex_exist(valid_vertex[i][0], valid_vertex[i][1], valid_vertex[i][2], valid_vertex[i][3]) == 0){
				if (vertex_exist(valid_vertex[i][2], valid_vertex[i][3], valid_vertex[i][0], valid_vertex[i][1]) == 1){
					//printf("(%d %d) to (%d %d) %f\n", (int)valid_vertex[i][0], (int)valid_vertex[i][1], (int)valid_vertex[i][2], (int)valid_vertex[i][3], valid_vertex[i][6]);
					//temp[j] = k;
					valid_vertex[k][0] = valid_vertex[i][0];
					valid_vertex[k][1] = valid_vertex[i][1];
					valid_vertex[k][2] = valid_vertex[i][2];
					valid_vertex[k][3] = valid_vertex[i][3];
					valid_vertex[k][4] = valid_vertex[i][4];
					valid_vertex[k][5] = valid_vertex[i][4] + valid_vertex[index_of_smallest][4];
					valid_vertex[k][6] = 1.0;
					
					printf("%f vs %f\n", valid_vertex[i][4]+valid_vertex[index_of_smallest][4], valid_vertex[k][5]);
					temp[j] = k;
					printf("temp[]: %d and j: %d\n", temp[j], j);
					j++;
					k++;
				}
			}
		}
	}

	//after finding alternative set the vertex to visited
	valid_vertex[index][6] = 1.0;
	
	if (j){
		m = smallest_length(j, temp);
		return m;
	}else{
		printf("FIGURE OUT %d: %d \n", j, index);
		return -100;
	}
	
}

int dijkstra(int index_of_smallest){

	printf("========================= DIJKSTRA() =========================\n");
	int i, j,m, goal;
	int temp[6];
	j=0;


	for (i=0; i<k; i++){
		printf("%d == %d && %d == %d\n", (int)valid_vertex[i][0], (int)valid_vertex[index_of_smallest][2], (int)valid_vertex[i][1], (int)valid_vertex[index_of_smallest][3]);
		if (valid_vertex[i][0] == valid_vertex[index_of_smallest][2] && valid_vertex[i][1] == valid_vertex[index_of_smallest][3]){
			//neighboring vertex
			if (valid_vertex[i][6] != 1.0 && valid_vertex[i][6] != 0.0 ){
				
				valid_vertex[i][5] = valid_vertex[index_of_smallest][4] + valid_vertex[i][4];

				temp[j] = i;
				j++;
			}
		}
	}

	if (j){
		//there is a list of neighbors
		m = smallest_length(j, temp);
		printf("There are neighbors: %d\n", m);
		if (m != -100){
			check_reverse_seg(m);
		}
		return m;
	}
	else{
		// no neighbors
		printf("There is no neighbor: @ %d, %f\n", m, valid_vertex[m][6]);
		m = check_alt_path_to_point(m, index_of_smallest);
		//set to visit which is the value of 1.0 and deadend
		printf("G: %d\n", m);
		//return m;
		return -100;
	}
}


void shortest_path(){

	int i, j, index_of_smallest, next_index, h1, h2, goal;
	int temp[6];// holds which vertex
	double smallest;
	j = 0;
	
	//initial neighbor vertices
	for(i=0; i<k; i++){
		if(valid_vertex[i][0] == valid_vertex[0][2] && valid_vertex[i][1] == valid_vertex[0][3]){
			if (valid_vertex[i][2] == valid_vertex[0][0]  && valid_vertex[i][3] == valid_vertex[0][1]){
			}
			else{
				printf("(%d %d) \n", (int)valid_vertex[i][2], (int)valid_vertex[i][3]);				
				
				//the vertex not visited yet so check the distance
				valid_vertex[i][5] = valid_vertex[0][4] + valid_vertex[i][4]; // checked, so set this to the same value as valid_vertex[i][4]
				temp[j] = i; 
				j++;
				
			}
		}
	}

	index_of_smallest = smallest_length(j, temp);
	printf("BEGIN: SMALLEST %d\n", index_of_smallest);

	int b;
	b = index_of_smallest;

	while(1){
		b = dijkstra(b);
		if (b == -100){
			break;
		}
		
	}
}


int main(int argc, char *argv[]){
	display = XOpenDisplay(NULL);
	win_height = 700;
	win_width = 700;
	count = -1;
	count_intersect = 0;
	k = 0;

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

	//int m[line_count][6];
	int m[line_count+1][6];

	//Creating window
	win = XCreateSimpleWindow(display, RootWindow(display, 0), 1, 1, win_width, win_height, 10, WhitePixel (display, 0), WhitePixel (display, 0));
	
	//Maps window on screen
	XMapWindow(display, win);

	colormap = DefaultColormap(display, 0);
	green_gc = XCreateGC(display, win, 0, 0);
	red_gc = XCreateGC(display, win, 0, 0);
	black_gc = XCreateGC(display, win, 0, 0);
	light_purple_gc = XCreateGC(display, win, 0, 0);
	XParseColor(display, colormap, green, &green_col);
	XParseColor(display, colormap, red, &red_col);
	XParseColor(display, colormap, black, &black_col);
	XParseColor(display, colormap, light_purple, &light_purple_col);
	XAllocColor(display, colormap, &green_col);
	XAllocColor(display, colormap, &red_col);
	XAllocColor(display, colormap, &black_col);
	XAllocColor(display, colormap, &light_purple_col);
	XSetForeground(display, green_gc, green_col.pixel);
	XSetForeground(display, red_gc, red_col.pixel);
	XSetForeground(display, black_gc, black_col.pixel);
	XSetForeground(display, light_purple_gc, light_purple_col.pixel);

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

				m[line_count+1][0] = -5;

				printf("line_count: %d\n", line_count);

				
				int j;
				
				for (i = 0; i <= line_count ; i++){
					for (j=0; j<6; j+=2){
						m[i][j] = m[i][j] + 100;
						m[i][j+1] = m[i][j+1] + 100;
					}
				}
				

				for (i = 0; i <=line_count; i++){
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

					if (inTriangle == 1){
						count = -1;
					}
				}
				else if (count == 1){

					if (inTriangle == 0){
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
						
						int j;
						
						for (i = 0; i <= line_count ; i++){
							for (j=0; j<6; j+=2){
								vertex[i][j] = vertex[i][j] + 100;
								vertex[i][j+1] = vertex[i][j+1] + 100;
							}
						}
						

						start_graph(line_count, vertex, start_x, start_y, target_x, target_y);

						end_graph(line_count, vertex, target_x, target_y);
						
						/*
						for (i= 0; i<k; i++){
							printf("From (%d, %d) to (%d, %d) with distance of %f.\n", (int)valid_vertex[i][0], (int)valid_vertex[i][1], (int)valid_vertex[i][2], (int)valid_vertex[i][3], valid_vertex[i][4]);
							XDrawLine(display, win, light_purple_gc, valid_vertex[i][0], valid_vertex[i][1], valid_vertex[i][2], valid_vertex[i][3]);
						}
						*/
						
						
						printf(" total lines %d\n", k);
						
						/*
						for (i= 0; i<k; i++){
							if  (valid_vertex[i][0] != 0){
								XDrawLine(display, win, light_purple_gc, valid_vertex[i][0], valid_vertex[i][1], valid_vertex[i][2], valid_vertex[i][3]);
							}
						}
						*/

						
						for(i=0; i<k; i++){
							//printf("from (%d, %d) to (%d, %d) is %f long and %f.\n", (int)valid_vertex[i][0], (int)valid_vertex[i][1], (int)valid_vertex[i][2], (int)valid_vertex[i][3], valid_vertex[i][4], valid_vertex[i][5]);
							printf("from (%d, %d) to (%d, %d) is %f long and weight: %f and has visited value: %d.\n", (int)valid_vertex[i][0], (int)valid_vertex[i][1], (int)valid_vertex[i][2], (int)valid_vertex[i][3], valid_vertex[i][4], valid_vertex[i][5], (int)valid_vertex[i][6]);
						}

						
						shortest_path();

						printf("apply shortest_path()\n");

						
						for(i=0; i<k; i++){
							printf("from (%d, %d) to (%d, %d) is %f long and weight: %f and has visited value: %d.\n", (int)valid_vertex[i][0], (int)valid_vertex[i][1], (int)valid_vertex[i][2], (int)valid_vertex[i][3], valid_vertex[i][4], valid_vertex[i][5], (int)valid_vertex[i][6]);
						}
						
						
						printf("The shortest path: \n");
						for (i= 0; i<k; i++){
							if  (valid_vertex[i][6] == 0.0 || valid_vertex[i][6] == -4.0){
								printf("from (%d, %d) to (%d, %d) is %f long and last %f.\n", (int)valid_vertex[i][0], (int)valid_vertex[i][1], (int)valid_vertex[i][2], (int)valid_vertex[i][3], valid_vertex[i][4], valid_vertex[i][6]);		
								XDrawLine(display, win, light_purple_gc, valid_vertex[i][0], valid_vertex[i][1], valid_vertex[i][2], valid_vertex[i][3]);
							}
						}
						
						
						printf(" total lines2 %d\n", k);
						
						
						printf("DONE\n");
						


						count = -1;
					}
					else{
						count -= 1;
					}
				}

				//if (inTriangle != 0){
				//	count = -1;
				//}
				
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